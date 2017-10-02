#ifdef __linux__
#include "UDPSocketMulticast_Linux.h"
const uint8_t UDPSocketMulticast_Linux::REFRESH_MULTICAST_SECONDS;

UDPSocketMulticast_Linux::UDPSocketMulticast_Linux( string ip, uint16_t multicastPort ) {
	this->multicastIp = multicastIp;
	this->multicastPort = multicastPort;

	closed = false;

	char loopch = 0;
	sendSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( sendSocket < 0 )
		throw std::runtime_error( "Sending Socket Error. Can't start. " );
	if ( setsockopt( sendSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof( loopch ) ) < 0 )
		throw std::runtime_error( "Socket option error. Can't start. " );

	//Initialize socket on which packets will be received
	receiveSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( receiveSocket < 0 )
		throw std::runtime_error( "Receive Socket Error. Can't start. " );
	u_int reuse = 1;
	if ( setsockopt( receiveSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof( reuse ) ) < 0 )
		throw std::runtime_error( "Socket SOL_SOCKET error. Can't start. " );
	if ( setsockopt( receiveSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof( loopch ) ) < 0 )
		throw std::runtime_error( "Socket IP_MULTICAST_LOOP error. Can't start. " );

	//Binding the socket
	sockaddr_in addr;
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( multicastPort );

	if ( bind( receiveSocket, (struct sockaddr *) &addr, sizeof( addr ) ) < 0 )
		throw std::runtime_error( "Socket bind error. Can't start. " );

	//multicastIp should be used instead, but it does not work
	inet_pton( AF_INET, "239.0.0.100", &(mreq.imr_multiaddr.s_addr) );
	mreq.imr_interface.s_addr = htonl( INADDR_ANY ); //Local interface

	if ( setsockopt( receiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof( mreq ) ) < 0 ) {
		throw std::runtime_error( "Socket IGMP error. Can't start. " );
	}

	//Prepare multicast address structure
	memset( &multicastSockaddr, 0, sizeof( multicastSockaddr ) );
	multicastSockaddr.sin_family = AF_INET;
	inet_pton( AF_INET, "239.0.0.100", &(multicastSockaddr.sin_addr) );
	multicastSockaddr.sin_port = htons( multicastPort );
	
	multicastRefresher = thread( &UDPSocketMulticast_Linux::refreshMulticast, this );
}

UDPSocketMulticast_Linux::~UDPSocketMulticast_Linux() {
	if ( !closed )
		closeSocket();
}

void UDPSocketMulticast_Linux::refreshMulticast() {
 	while ( !closed ) {
 		if ( setsockopt( receiveSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof( mreq ) ) < 0 ) {
 			return;
		}
 		if ( setsockopt( receiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof( mreq ) ) < 0 ) {
 			return;
 		}
 		unique_lock<mutex> ul( mutex_wait_for_refresh );
 		cv_wait_for_refresh.wait_for( ul, std::chrono::seconds( REFRESH_MULTICAST_SECONDS ));
 	}
 }


void UDPSocketMulticast_Linux::sendPacket( string message ) {
	sendto( sendSocket, message.c_str(), message.length(), 0, (struct sockaddr *) &multicastSockaddr, sizeof( multicastSockaddr ) );
}

int8_t UDPSocketMulticast_Linux::receivePacket( std::string& message, std::string& senderIp ) {
	char* addr;
	struct sockaddr_in senderAddr;
	socklen_t addrlen = sizeof( senderAddr );

	vector<char> messageBuffer( 270 );

	int8_t recvfromCode = recvfrom( receiveSocket, messageBuffer.data(), messageBuffer.size(), 0, (struct sockaddr *) &senderAddr, &addrlen );
	if ( recvfromCode < 0 )
		return recvfromCode;

	message.assign( messageBuffer.cbegin(), messageBuffer.cend() );
	addr = inet_ntoa( senderAddr.sin_addr );

	senderIp.assign( addr );
	return 0;
}

void UDPSocketMulticast_Linux::closeSocket() {
	closed = true;
	cv_wait_for_refresh.notify_one();
	if ( multicastRefresher.joinable() )
		multicastRefresher.join();

	shutdown( receiveSocket, SHUT_RDWR );
	close( sendSocket );
	close( receiveSocket );
}

#endif
