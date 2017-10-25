#include "FileReceiver.h"

FileReceiver::FileReceiver(TCPSocket s, boost::filesystem::path p) : controlS(std::move(s)), transferStart( std::chrono::system_clock::now() ) {
	dest = p;
	dest=dest.generic_path();
	overallSent = 0;
	overallSize = 0;
	success = true;

	receiveTransferRequest();

}

FileReceiver::~FileReceiver() {
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
		tradeport();
		controlS.Close();

		prepareSockets();

		for ( uint16_t  i = 0; i < fileSockets.size(); ++i ) {
			transferThreads.push_back( thread( &FileReceiver::threadReceive, this, i ) );
		}
		for ( uint16_t  i = 0; i < transferThreads.size(); ++i )
			if ( transferThreads[i].joinable() )
				transferThreads[i].join();
		transferThreads.clear();

	}catch(std::exception de ){
		cout <<"Eccezione: " << de.what() << endl;
		success.store( false );
	}

	for ( auto it = fileSockets.begin(); it != fileSockets.end(); ++it )
		it->Close();
	fileSockets.clear();

	for ( auto it = fileServerSockets.begin(); it != fileServerSockets.end(); it++ )
		it->Close();
	fileServerSockets.clear();

	return success.load();
}

void FileReceiver::receiveTransferRequest() {
	lock_guard<mutex> l( transferDetailsMutex );
	vector<char> reqV( 9 );
	struct timeval timeout; timeout.tv_sec = 120; timeout.tv_usec = 0;
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
	timeout.tv_sec = 120; timeout.tv_usec = 0;
	if ( !controlS.Receive( reqV, transferNameSize, timeout ) )
		throw std::domain_error( "Can't receive the name. " );

	transferName.assign( reqV.begin(), reqV.end());
}

void FileReceiver::tradeport() {
	vector<char> msg(6);
    struct timeval timeout; timeout.tv_sec = 120; timeout.tv_usec = 0;
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
			cout << "Port: " << testPort << endl;
			testPort = htons(testPort);
			memcpy(msg.data() + i*2, &testPort, sizeof(testPort));
			++i;
		}catch (...) {}
	}
	controlS.Send(msg);

}

void FileReceiver::prepareSockets() {
	for (uint16_t i = 0; i < fileServerSockets.size(); i++)
		fileSockets.push_back(TCPSocket(fileServerSockets[i].Accept()));

}

void FileReceiver::threadReceive(uint16_t i){
    vector<char> msgV(4);
    string msgS;
    struct timeval timeout1; timeout1.tv_sec = 120; timeout1.tv_usec = 0;
    struct timeval timeout2; timeout2.tv_sec = 120; timeout2.tv_usec = 0;
	boost::filesystem::fstream file;
	while ( success.load() ) {
		if ( !fileSockets[i].Receive( msgV, 4, timeout1 ) ) {
			success.store( false );
			return;
		}
		msgS.assign( msgV.begin(), msgV.end() );

		if ( msgS.compare( "FILE" ) == 0 ) {
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

				uint8_t duplicate=1;
				if ( boost::filesystem::exists( dest.generic() / filePath ) ) {
					while ( boost::filesystem::exists( dest.generic() / (filePath.stem().string() + "(" + to_string( duplicate ) + ")" + filePath.extension().string()) ) ) {
						++duplicate;
					}
					filePath = filePath.stem().string() + "(" + to_string( duplicate ) + ")" + filePath.extension().string();
				}
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
					//This means that it is a file being sent, not a directory, so the overallSize is given by the file size
					overallSize.store( fileSize );
				}

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
				success.store( false );
				if ( file.is_open() )
					file.close();
				cout << this_thread::get_id <<de.what() << endl;
				return;
			}
			file.close();
	
		} else if ( msgS.compare( "QUIT" ) == 0 ) {
			fileSockets[i].Close();
			return;

		} else if ( msgS.compare( "SIZE" ) == 0 ) {
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

uint8_t FileReceiver::getProgress(){
	uint8_t progress;
	if ( overallSize.load()  == 0)
		return 0;
	progress = (overallSent.load() * 100) / overallSize.load();
	return progress;
}


uint8_t FileReceiver::getStatus() {
	if ( !success.load() )
		return FT_ERROR;
	uint8_t progress = getProgress();
	if ( progress < 100 )
		return progress;

	return FT_COMPLETE;
}

void FileReceiver::getStatistics( double& speed, double& timeLeft ) {
	int overallSizeTemp = overallSize.load();
	int overallSentTemp = overallSent.load();
	std::chrono::seconds timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>( std::chrono::system_clock::now() - transferStart.load() ));
	speed = static_cast<double>(overallSentTemp) / (1048576 * timeElapsed.count());
	timeLeft = static_cast<double>(overallSizeTemp - overallSentTemp) / (1048576 * speed);
}

void FileReceiver::getFileDetails( string & name, uint8_t & type ) {
	lock_guard<mutex> l(transferDetailsMutex);
	name = transferName;
	type = transferType;
}

void FileReceiver::stop() {
	success.store( false );

}
