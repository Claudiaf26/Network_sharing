#include "FileTransfer.h"


FileTransfer::FileTransfer( string ipAddress, path Source ) : ip( ipAddress ), source( Source ) {
	if ( is_directory( source ) ) {
		recursive_directory_iterator rdi( source.generic_path() );
		this->rdi = rdi;
		recursive_directory_iterator end_rdi;
		this->end_rdi = end_rdi;
	}
	success.store( true );
	overallSize = 1; overallSent = 0;
}

FileTransfer::~FileTransfer() {
	for ( auto it = transferThreads.begin(); it != transferThreads.end(); it++ ) {
		if ( (*it)->joinable() ) {
			(*it)->join();
		}
	}
}

bool FileTransfer::transfer() {
	try {
		tradePort(); //throws
		prepareSockets(); //throws

		if ( is_directory( source ) ) {
			if ( !sendDir() )
				throw std::domain_error( "Not working. " );

			/*Create threads*/
			transferThreads.resize( ports.size() );
			for ( uint16_t i = 0; i < ports.size(); i++ )
				transferThreads[i].reset( new thread( &FileTransfer::sendFile, this, i ) );
		} else {
			transferThreads.resize( 1 );
			transferThreads[0].reset( new thread( &FileTransfer::sendFile, this, 0 ) );
		}

	} catch ( ... ) {
		success.store( false );
	}
	for ( auto it = transferThreads.begin(); it != transferThreads.end(); it++ ) {
		if ( (*it)->joinable() ) {
			(*it)->join();
		}
	}
	for ( auto it = fileSockets.begin(); it != fileSockets.end(); it++ )
		it->Close();
	fileSockets.clear();

	return success.load();

}

void FileTransfer::tradePort() {
	//Control connection
	TCPSocket s( ip, 50000 );

	//Asking to open nThreads data connection (nThreads ports are needed)
	string msg( "PORT" );
	vector<char> message( msg.begin(), msg.end() );
	uint16_t nrThreads;
	if ( is_directory( source ) )
		nrThreads = thread::hardware_concurrency() / 2;
	else
		nrThreads = 1;

	nrThreads = htons( nrThreads );

	message.resize( msg.size() + 2 );
	memcpy( message.data() + message.size() - 2, &nrThreads, sizeof( nrThreads ) );

	if ( s.Send( message ) == false )
		throw std::domain_error( "Can't establish connection. " );

	struct timeval t; t.tv_sec = 120; t.tv_usec = 500;
	if ( s.Receive( message, 6, t ) == false )
		throw std::domain_error( "Can't establish connection. " );

	//The response is PORT NrPortsAvailable Port1 Port2 ...
	//The host will reply with how many threads it wants to use.
	msg.assign( message.begin(), message.begin() + 4 );
	if ( msg.compare( "PORT" ) != 0 )
		throw std::domain_error( "Protocol not respected. " );

	memcpy( &nrThreads, message.data() + 4, sizeof( nrThreads ) );
	nrThreads = ntohs( nrThreads );
	message.clear();
	//Here all ports are received.
	if ( s.Receive( message, 2 * nrThreads, t ) == false )
		throw std::domain_error( "Can't establish connection. " );

	ports.resize( nrThreads );
	for ( int16_t i = 0; i < nrThreads; i++ ) {
		memcpy( &ports[i], message.data() + i * 2, sizeof( int16_t ) );
		ports[i] = ntohs( ports[i] );
		cout << "Port: " << ports[i] << endl;
	}

	s.Close();

}

void FileTransfer::prepareSockets() {
	int8_t tries = 0;
	for ( int16_t i = 0; i < ports.size();) {
		try {
			fileSockets.push_back( TCPSocket( ip, ports[i] ) );
			++i;
		} catch ( ... ) {
			if ( tries <= 1 )
				++tries;
			else
				throw std::domain_error( "Can't establish connection. " );
			success.store( "false" );
			this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
		}
	}
}

bool FileTransfer::sendDir() {
	/*Computing directories tree and overall dimension*/

	vector<wstring> tree;
	vector<char> message;
	auto future_tree = async( &FileTransfer::exploreDir, this );
	auto future_dirSize = async( &FileTransfer::dirSize, this );

	//Sending SIZE command and waiting for the ACK
	message.push_back( 'S' ); message.push_back( 'I' ); message.push_back( 'Z' ); message.push_back( 'E' );
	message.resize( message.size() + 8 );
	overallSize.store( future_dirSize.get() );
	uint64_t tempSize = boost::endian::native_to_big( overallSize.load() );
	memcpy( message.data() + message.size() - 8, &tempSize, sizeof( tempSize ) );
	if ( !fileSockets[0].Send( message ) )
		throw std::domain_error( "Connection closed. " );

	vector<char> msgV( 3 );
	string msgS;
	struct timeval timeout; timeout.tv_sec = 100; timeout.tv_usec = 0;
	if ( !fileSockets[0].Receive( msgV, 3, timeout ) ) {
		throw std::domain_error( "Connection closed before SIZE ACK. " );
	}
	msgS.assign( msgV.begin(), msgV.end() );
	if ( msgS.compare( "ACK" ) != 0 )
		throw std::domain_error( "Expected ACK for SIZE. " );

	/*Sending the directory tree to the other host and waiting for the ack.*/
	message.clear();
	tree = future_tree.get();
	message = createDirectoryPacket( tree );
	if ( !fileSockets[0].Send( message ) )
		throw std::domain_error( "Connection closed. " );

	if ( !fileSockets[0].Receive( msgV, 3, timeout ) ) {
		throw std::domain_error( "Connection closed before DIRT ACK. " );
	}
	msgS.assign( msgV.begin(), msgV.end() );
	if ( msgS.compare( "ACK" ) != 0 )
		throw std::domain_error( "Expected DIRT for SIZE. " );


	return true;
}

void FileTransfer::sendFile( uint16_t i ) {
	boost::filesystem::ifstream file;
	bool flag = true;
	bool isDirectory = is_directory( source );
	wstring relativePath;
	path p;
	try {
		while ( success.load() ) {
			if ( isDirectory ) {
				rdi_protection.lock();
				while ( flag ) {
					if ( rdi == end_rdi ) {
						flag = false;
					} else {
						p = rdi->path();
						++rdi;
						if ( !is_directory( p ) ) {
							break;
						}
					}
				}
				rdi_protection.unlock();


				if ( flag == false )
					break;
				relativePath = (relative( p.generic(), source.generic() )).generic().wstring();
			} else {
				p = source;
				relativePath = source.filename().wstring();
			}
			uint64_t fileSize = file_size( p );
			if ( !isDirectory )
				overallSize.store( fileSize );

			{
				vector<char> message;
				string temp = "FILE";
				message.assign( temp.begin(), temp.end() );
				uint32_t pathlength = relativePath.length();
				message.resize( 8 );
				pathlength = htonl( pathlength );
				memcpy( message.data() + message.size() - 4, &pathlength, sizeof( pathlength ) );
				message.insert( message.end(), relativePath.begin(), relativePath.end() );
				fileSize = boost::endian::native_to_big( fileSize );
				message.resize( message.size() + 8 );
				memcpy( message.data() + message.size() - 8, &fileSize, sizeof( fileSize ) );

				fileSize = boost::endian::big_to_native( fileSize );

				if ( !fileSockets[i].Send( message ) )
					throw std::domain_error( "Not working. " );

			}//Clear the stack from some variable

			file.open( p, ios::in | ios::binary );
			if ( !file.is_open() )
				throw std::domain_error( "Not working. " );

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
				file.read( fileChunk.data(), chunkSize );
				if ( !fileSockets[i].Send( fileChunk ) )
					throw std::domain_error( "Not working. " );
				progress = progress - chunkSize;
				overallSent.fetch_add( chunkSize );
			}


			if ( padding != 0 ) {
				fileChunk.resize( padding );
				file.read( fileChunk.data(), padding );
				if ( !fileSockets[i].Send( fileChunk ) )
					throw std::domain_error( "Not working. " );
				overallSent.fetch_add( padding );
			}

			file.close();
			if ( !isDirectory )
				break;
		}
		if ( success.load() ) {
			vector<char> quit; quit.push_back( 'Q' ); quit.push_back( 'U' ); quit.push_back( 'I' ); quit.push_back( 'T' );
			fileSockets[i].Send( quit );
		}

	} catch ( ... ) {
		if ( file.is_open() )
			file.close();
		success.store( false );
	}

}



vector<wstring> FileTransfer::exploreDir() {
	vector<wstring> tree;
	if ( !exists( source ) )
		throw std::domain_error( "The source does not exists. " ); //The directory does not exists.

	recursive_directory_iterator rdi( source );
	recursive_directory_iterator end_rdi;

	tree.push_back( relative( source.generic(), source.parent_path() ).generic().wstring() );
	for ( ; rdi != end_rdi; rdi++ ) {
		if ( is_directory( (*rdi).path() ) )
			tree.push_back( relative( (*rdi).path(), source ).generic().wstring() );
	}
	return tree;
}

uint64_t FileTransfer::dirSize() {
	uint64_t size = 0;
	if ( !exists( source ) )
		throw std::domain_error( "The source does not exists. " ); //The directory does not exists.

	recursive_directory_iterator rdi( source );
	recursive_directory_iterator end_rdi;

	for ( ; rdi != end_rdi; rdi++ ) {
		if ( !is_directory( (*rdi).path() ) )
			size += file_size( (*rdi).path() );
	}
	return size;
}

vector<char> FileTransfer::createDirectoryPacket( vector<wstring> tree ) {
	string temp( "DIRT" );
	vector<char> message;
	message.insert( message.begin(), temp.begin(), temp.end() );
	message.insert( message.end(), 4, 0 );
	//message.push_back('^');
	for ( auto it = tree.begin(); it != tree.end(); it++ ) {
		message.insert( message.end(), (*it).begin(), (*it).end() );
		message.push_back( '^' );
	}
	uint32_t payloadSize = message.size() - temp.length() - 4;

	payloadSize = htonl( payloadSize );
	memcpy( message.data() + 4, &payloadSize, sizeof( payloadSize ) );

	string c( message.begin() + 4, message.begin() + 8 );

	return message;
}

uint8_t FileTransfer::getProgress() {
	uint8_t progress;
	progress = (overallSent.load() * 100) / overallSize.load();
	return progress;
}

void FileTransfer::stop() {
	success.store( false );
}