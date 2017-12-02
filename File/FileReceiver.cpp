#include "FileReceiver.h"

FileReceiver::FileReceiver(TCPSocket s, boost::filesystem::path p) : controlS(std::move(s)), transferStart( std::chrono::system_clock::now() ) {
	dest = p;
	dest=dest.generic_path();
	overallSent = 0;
	overallSize = 0;
	success = true;
	
	/*It is necessary to call this method here so that the file or directory name is available as soon as possible.*/
	receiveTransferRequest();

}

FileReceiver::~FileReceiver() {
	/*Do not stop until threads gracefully close. If you want to interrupt the transfer, call stop() and only after you can destroy the object.*/
	for ( uint16_t i = 0; i < transferThreads.size(); ++i )
		if ( transferThreads[i].joinable() )
			transferThreads[i].join();
	transferThreads.clear();

	for ( auto it = fileSockets.begin(); it != fileSockets.end(); ++it )
		it->Close();
	fileSockets.clear();

	for ( auto it = fileServerSockets.begin(); it != fileServerSockets.end(); ++it )
		it->Close();
	fileServerSockets.clear();
}

bool FileReceiver::receive() {
	try {
		/*Here the connection has been accepted, so it is possible to establish data connections and close the control one.
		Moreover, it makes sense to evaluate the the starting time. */
		transferStart.store( std::chrono::system_clock::now() );
		tradeport(); //Throws de
		controlS.Close();
		
		prepareSockets();

		for ( uint16_t  i = 0; i < fileSockets.size(); ++i ) {
			/*Creates as many threads as the number of available sockets.*/
			transferThreads.push_back( thread( &FileReceiver::threadReceive, this, i ) );
		}
		/*Now wait for the threads to do their job.*/
		for ( uint16_t  i = 0; i < transferThreads.size(); ++i )
			if ( transferThreads[i].joinable() )
				transferThreads[i].join();
		transferThreads.clear();

	}catch(std::exception de ){
		success.store( false );
	}

	/*Independently from the status of the transfer, release the network resources. */

	for ( auto it = fileSockets.begin(); it != fileSockets.end(); ++it )
		it->Close();
	fileSockets.clear();

	for ( auto it = fileServerSockets.begin(); it != fileServerSockets.end(); it++ )
		it->Close();
	fileServerSockets.clear();
	if ( !success.load() && !transferName.empty()) {
		/*Transfer failed, delete whatever has been received.*/
		try {
            boost::filesystem::remove_all( localFile );
		} catch ( ... ) {
			/*There is nothing to recover here, the transfer has failed, if the remove fails than some junk will remain on the user filesystem.*/
		}

	}

	return success.load();
}

void FileReceiver::receiveTransferRequest() {
	lock_guard<mutex> l( transferDetailsMutex ); /*This structure may be invoked from outside by another thread, so it has to be protected.*/
	
	/*The sender sends a DIRTREQ or FILEREQ, the size of the name, the name. */
	vector<char> reqV( 9 );
    struct timeval timeout; timeout.tv_sec = 2; timeout.tv_usec = 0;
	if ( !controlS.Receive( reqV, 9, timeout ) )
		throw std::domain_error( "Can't receive initialization request. " );

	string requestS( reqV.begin(), reqV.begin() + 7 );
	if ( requestS.compare( "DIRTREQ" ) == 0 )
		transferType = FT_DIRECTORY;
	else if ( requestS.compare( "FILEREQ" ) == 0 )
		transferType = FT_FILE;
	else
		throw std::domain_error( "Wrong protocol. " );

	uint16_t transferNameSize;
	memcpy( &transferNameSize, reqV.data() + 7, sizeof( transferNameSize ) );
	transferNameSize = ntohs( transferNameSize );

	reqV.resize( transferNameSize );
    timeout.tv_sec = 2; timeout.tv_usec = 0;
	if ( !controlS.Receive( reqV, transferNameSize, timeout ) )
		throw std::domain_error( "Can't receive the name. " );

	transferName.assign( reqV.begin(), reqV.end());
}

void FileReceiver::tradeport() {
	vector<char> msg(6);

    /*Protocol: 
     *Request: PORT, number of ports
     *Response: PORT, random choosen ports*/
    struct timeval timeout; timeout.tv_sec = 2; timeout.tv_usec = 0;
	if(!controlS.Receive(msg, 6, timeout))
		throw std::domain_error("Can't trade ports. ");
	

	string cmd(msg.begin(), msg.begin() + 4);
	if(cmd.compare("PORT")!=0)
		throw std::domain_error("Wrong protocol. ");
	uint16_t portsAsked;
	memcpy(&portsAsked, msg.data() + 4, sizeof(portsAsked));
	uint16_t nThreads = thread::hardware_concurrency()/2;
	portsAsked = ntohs(portsAsked);
	if (nThreads >= portsAsked) {
		nThreads = portsAsked;
	} else {
		nThreads = thread::hardware_concurrency()/2;
	}
	cmd.assign("PORT");
	msg.assign(cmd.begin(), cmd.end());
	msg.resize(msg.size() + 2);
	nThreads = htons(nThreads);
	memcpy(msg.data() + 4, &nThreads, sizeof(nThreads));
	if(!controlS.Send(msg))
		throw std::domain_error("Wrong protocol. ");
	nThreads = ntohs( nThreads );

	msg.resize(2*nThreads);
	srand((int16_t)time(0));
	uint16_t testPort;
	for (uint16_t i = 0; i < nThreads;) {
		testPort = (rand() % 16383) + 49152;
		try {
			fileServerSockets.push_back(TCPServerSocket(testPort));
			testPort = htons(testPort);
			memcpy(msg.data() + i*2, &testPort, sizeof(testPort));
			++i;
		}catch (...) {
		/*This catch HAS to be empty. If TCPServerSocket(...) throws an exception, nothing is done and
		 *another port is tried. */
		}
	}
	controlS.Send(msg);

}

void FileReceiver::prepareSockets() {
/*Creates data connections*/
	for (uint16_t i = 0; i < fileServerSockets.size(); i++)
		fileSockets.push_back(TCPSocket(fileServerSockets[i].Accept()));

}

void FileReceiver::threadReceive(uint16_t i){
    vector<char> msgV(4);
    string msgS;
    struct timeval timeout1; timeout1.tv_sec = 2; timeout1.tv_usec = 0;
    struct timeval timeout2; timeout2.tv_sec = 2; timeout2.tv_usec = 0;
	boost::filesystem::fstream file;
	while ( success.load() ) {
		if ( !fileSockets[i].Receive( msgV, 4, timeout1 ) ) {
			success.store( false );
			return;
		}
		msgS.assign( msgV.begin(), msgV.end() );

		if ( msgS.compare( "FILE" ) == 0 ) {
		/*A file is being received here. Receive the name, receive the dimension, receive the file. */
			try {
				if ( !fileSockets[i].Receive( msgV, 4, timeout2 ) ) {
					success.store( false );
					return;
				}
				uint32_t filePathSize;
				memcpy( &filePathSize, msgV.data(), sizeof( filePathSize ) );
				filePathSize = ntohl( filePathSize );
				msgV.clear();
				if ( !fileSockets[i].Receive( msgV, filePathSize, timeout2 ) ) {
					success.store( false );
					return;
				}

				boost::filesystem::path filePath( msgV.begin(), msgV.end() );
				
				/*If the file exists, try to put an increasing number until the name is free.*/
				uint8_t duplicate=1;
				if ( boost::filesystem::exists( dest.generic() / filePath ) ) {
					while ( boost::filesystem::exists( dest.generic() / (filePath.stem().string() + "(" + to_string( duplicate ) + ")" + filePath.extension().string()) ) ) {
						++duplicate;
					}
					filePath = filePath.stem().string() + "(" + to_string( duplicate ) + ")" + filePath.extension().string();
				}
                if(transferType!=FT_DIRECTORY)
                    localFile=dest / filePath;
				file.open(dest.generic() / filePath, ios::out | ios::binary );
				if ( !file.is_open() ) {
					success.store( false );
					return;
				}
				msgV.clear();
				if ( !fileSockets[i].Receive( msgV, 8, timeout2 ) ) {
					success.store( false );
					return;
				}

				uint64_t fileSize;
				memcpy( &fileSize, msgV.data(),  sizeof( fileSize ) );
				fileSize = boost::endian::big_to_native( fileSize );
				if ( overallSize == 0 ) {
					/*This means that it is a single file being sent, not a directory, 
                                          so the overallSize is given by the file size. This is needed to
					  expose to the outside the progress.*/
					overallSize.store( fileSize );
				}
				
				/*Receive chunks until the file is complete.*/
				uint32_t chunkSize;
				uint32_t padding;
				if ( fileSize >= 32768 ) {
					chunkSize = 32768;
					padding = fileSize%chunkSize;
				} else {
					chunkSize = fileSize;
					padding = 0;
				}
				uint64_t progress = fileSize;
				vector<char> fileChunk( chunkSize );
				while ( progress > padding && success.load() ) {
					if ( !fileSockets[i].Receive(fileChunk, chunkSize, timeout2) )
						throw std::domain_error( "Can't receive fileChunk. " );
					file.write( fileChunk.data(), chunkSize );					
					progress = progress - chunkSize;
					overallSent.fetch_add( chunkSize );
				}

				if ( padding != 0 && success.load() ) {
					fileChunk.resize( padding );
					if ( !fileSockets[i].Receive( fileChunk, padding, timeout2 ) )
						throw std::domain_error( "Can't receive last chunk. " );
					file.write( fileChunk.data(), padding );
					
				}
				overallSent.fetch_add( padding );
			} catch ( std::domain_error de ) {
				/*An error happened during the transmission. Stop the other threads and release resources. */
				success.store( false );
				if ( file.is_open() )
					file.close();
				return;
			}
			file.close();
	
		} else if ( msgS.compare( "QUIT" ) == 0 ) {
			/*The thread on the other side has nothing else to transfer, so it tells to stop.
		         *The overall dimension is not enough, because maybe the bytes received are less than
		         *the overall dimension, but just because other threads are still receiving. */
			fileSockets[i].Close();
			return;

		} else if ( msgS.compare( "SIZE" ) == 0 ) {
			/*Used only for directories. It is needed to know the overall size of files and directories and compute the progress. */
			if ( !fileSockets[i].Receive( msgV, 8, timeout2 ) ) {
				success.store( false );
				return;
			} 

			uint64_t tempSize;
			memcpy( &tempSize, msgV.data(), sizeof( tempSize ) );
			overallSize.store( boost::endian::big_to_native( tempSize) );

			vector<char> ackMessage;
			ackMessage.push_back( 'A' ); ackMessage.push_back( 'C' ); ackMessage.push_back( 'K' );
			if ( !fileSockets[0].Send( ackMessage ) )
				throw std::domain_error( "Connection closed before the SIZE ACK. " );

		} else if ( msgS.compare( "DIRT" ) == 0 ) {
			/*A directory is going to be transferred. The tree of directories is computes, so that they are created by the 
			 *receiver before actually receving the files inside.*/
			uint32_t paySize;
			if ( !fileSockets[i].Receive( msgV, 4, timeout2 ) ) {
				success.store( false );
				return;
			}
			memcpy( &paySize, msgV.data(), sizeof( paySize ) );
			paySize = ntohl( paySize );

			if ( !fileSockets[i].Receive( msgV, paySize, timeout2 ) ) {
				success.store( false );
				return;
			}
			string wmsgS;
			wmsgS.assign(msgV.begin(), msgV.end());
			uint32_t pos1=0, pos2 = 0;
			boost::filesystem::path tempPath;
			/*The first path received is the main folder. It is checked if it is a duplicated
			 * and a number is put next to the name. Than all other directories are computed
			 * inside it.*/
			pos2 = wmsgS.find( '^', pos1 );
			tempPath = wmsgS.substr( pos1, pos2 - pos1 );

			uint8_t duplicate = 1;
			if ( boost::filesystem::exists( dest / tempPath.generic() ) ) {
				while ( boost::filesystem::exists( dest / (tempPath.string() + "(" + to_string( duplicate ) + ")") ) ) {
					++duplicate;
				}
				tempPath = tempPath.string() + "(" + to_string( duplicate ) + ")";
			}
			dest = dest / tempPath;
            localFile=dest;
			pos1 = pos2 + 1;
			boost::filesystem::create_directory( dest );

			while ( pos1 < wmsgS.size() ) {
				pos2=wmsgS.find( '^' , pos1 );
				tempPath = wmsgS.substr( pos1, pos2 - pos1 );
				boost::filesystem::create_directories( dest / tempPath );
				pos1 = pos2 + 1;
			}

			vector<char> ackMessage;
			ackMessage.push_back( 'A' ); ackMessage.push_back( 'C' ); ackMessage.push_back( 'K' );
			if ( !fileSockets[0].Send( ackMessage ) )
				success.store( false );
			
		} else {
			success.store( false );
		}
	}
	
	
}

void FileReceiver::getStatus( transferStatus& t) {
    if ( !success.load() ) {
        t.progress = FT_ERROR;
        t.speed = 0;
        t.secondsLeft = 0;
        return;
    }

    uint64_t overallSizeTemp = overallSize.load();
    uint64_t overallSentTemp = overallSent.load();
    std::chrono::seconds timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>( std::chrono::system_clock::now() - transferStart.load() ));

    /* Getting the progress */
    if ( overallSentTemp == 0 ) {
        t.progress = 0;
    } else {
        t.progress = (overallSentTemp * 100) / overallSizeTemp;
        if ( t.progress >= 100 ) {
            t.progress = FT_COMPLETE;
            t.secondsLeft = 0;
            t.speed = 0;
            return;
        }
    }

    /* Getting the speed */
    if ( timeElapsed.count() <= 0 ) {
        t.speed = 0;
    } else {
        t.speed = static_cast<double>(overallSentTemp) / (1048576 * timeElapsed.count());
        if ( t.speed < 0 )
            t.speed = 0;
    }

    if ( t.speed == 0 )
        t.secondsLeft = 0;
    else
        t.secondsLeft = static_cast<uint32_t>((overallSizeTemp - overallSentTemp) / (1048576 * t.speed));
}


void FileReceiver::getFileDetails( string & name, uint8_t & type ) {
	lock_guard<mutex> l(transferDetailsMutex);
	name = transferName;
	type = transferType;
}

void FileReceiver::stop() {
	/*By storing this flag it is possible to stop all threads.*/
	success.store( false );

}
