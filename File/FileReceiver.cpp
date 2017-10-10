#include "FileReceiver.h"

FileReceiver::FileReceiver(TCPSocket s, wstring p) : controlS(std::move(s)) {
	std::replace( p.begin(), p.end(), '\\', '/' );
	dest = p;
	dest=dest.generic_path();
	overallSent = 0;
	overallSize = 0;
	success = true;
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
		tradeport();
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

wstring FileReceiver::getFileName() {
	lock_guard<mutex> l( fileNameMutex );
	return fileName;
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
	controlS.Close();

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

                wstring prova(msgV.begin(), msgV.end());
                path filePath(prova);

				fileNameMutex.lock();
				if(fileName.empty() )
                    fileName = filePath.wstring();
				fileNameMutex.unlock();

				uint8_t duplicate=1;
				if ( exists( dest.generic() / filePath ) ) {
					while ( exists( dest.generic() / (filePath.stem().wstring() + L"(" + to_wstring( duplicate ) + L")" + filePath.extension().wstring()) ) ) {
						++duplicate;
					}
					filePath = filePath.stem().wstring() + L"(" + to_wstring( duplicate ) + L")" + filePath.extension().wstring();
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
			wstring wmsgS;
			wmsgS.assign(msgV.begin(), msgV.end());
			uint32_t pos1=0, pos2 = 0;
			path tempPath;
			/*The first path received is the main folder. It is checked if it is a duplicated
			 * and a number is put next to the name. Than all other directories are computed
			 * inside it.*/
			pos2 = wmsgS.find( '^', pos1 );
			tempPath = wmsgS.substr( pos1, pos2 - pos1 );
			
			fileNameMutex.lock();
            fileName = tempPath.wstring();
			fileNameMutex.unlock();

			uint8_t duplicate = 1;
			if ( exists( dest / tempPath.generic() ) ) {
				while ( exists( dest / (tempPath.wstring() + L"(" + to_wstring( duplicate ) + L")") ) ) {
					++duplicate;
				}
				tempPath = tempPath.wstring() + L"(" + to_wstring( duplicate ) + L")";
			}
			dest = dest / tempPath;
			pos1 = pos2 + 1;

			while ( pos1 < wmsgS.size() ) {
				pos2=wmsgS.find( '^' , pos1 );
				tempPath = wmsgS.substr( pos1, pos2 - pos1 );
				create_directories( dest / tempPath );
				pos1 = pos2 + 1;
			}

			vector<char> ackMessage;
			ackMessage.push_back( 'A' ); ackMessage.push_back( 'C' ); ackMessage.push_back( 'K' );
			if ( !fileSockets[0].Send( ackMessage ) )
				throw std::domain_error( "Connection closed before the SIZE ACK. " );
			
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

void FileReceiver::stop() {
	success.store( false );

}
