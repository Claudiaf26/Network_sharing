#ifdef _WIN32
#include "UDPSocketMulticast_Windows.h"
#pragma comment(lib, "Iphlpapi.lib")

UDPSocketMulticast_Windows::UDPSocketMulticast_Windows( string multicastIp, uint16_t multicastPort ) {
	this->multicastIp = multicastIp;
	this->multicastPort = multicastPort;
	closed = false;

	char loopch = 0;
	//Send socket initialization
	WSAStartup( 0x0202, &wsaDataSend );
	sendSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( sendSocket == INVALID_SOCKET )
		throw std::runtime_error( "Sending Socket Error. Can't start. " );

	//Avoid multicast to be forwarder on multicast
	if ( setsockopt( sendSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof( loopch ) ) == SOCKET_ERROR )
		throw std::runtime_error( "Socket option (multicast) error. Can't start. " );

	//Initialize socket on which packets will be received
	WSAStartup( 0x0202, &wsaDataReceive );
	receiveSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( receiveSocket == INVALID_SOCKET )
		throw std::runtime_error( "Receive Socket Error. Can't start. " );

	//Prevent errors if the socket is closed and re-opened
	u_int reuse = 1;
	if ( setsockopt( receiveSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof( reuse ) ) == SOCKET_ERROR )
		throw std::runtime_error( "Socket option error. Can't start. " );
	if ( setsockopt( receiveSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof( loopch ) ) == SOCKET_ERROR )
		throw std::runtime_error( "Socket option error. Can't start. " );

	//Binding the socket
	sockaddr_in addr;
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( multicastPort );
	if ( ::bind( receiveSocket, (struct sockaddr *) &addr, sizeof( addr ) ) == SOCKET_ERROR )
		throw std::runtime_error( "Socket bind error. Can't start. " );

	//IGMP Subscription
	joinMulticast();

	//Prepare multicast address structure
	memset( &multicastSockaddr, 0, sizeof( multicastSockaddr ) );
	multicastSockaddr.sin_family = AF_INET;
	inet_pton( AF_INET, multicastIp.c_str(), &(multicastSockaddr.sin_addr) );
	multicastSockaddr.sin_port = htons( multicastPort );

}

UDPSocketMulticast_Windows::~UDPSocketMulticast_Windows() {
	if ( !closed ) {
		closeSocket();
	}
}

void UDPSocketMulticast_Windows::joinMulticast() {
	/*
	*This method is needed in Windows due to the kernel
	*choosing the wrong local interface in some cases.
	*Here each interface is checked but only the one connected
	*to a gateway is accepted.
	*THIS METHOD IS NOT PORTABLE!
	*/
	struct ip_mreq mreq;
	inet_pton( AF_INET, multicastIp.c_str(), &(mreq.imr_multiaddr.s_addr) );

	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	PIP_ADAPTER_UNICAST_ADDRESS selectedInterface = NULL;
	ULONG  size;
	if ( GetAdaptersAddresses( AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, NULL, &size ) != ERROR_BUFFER_OVERFLOW )
		throw std::runtime_error( "Socket multicast binding error. Can't start. " );

	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc( size );
	if ( GetAdaptersAddresses( AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, adapter_addresses, &size ) != ERROR_SUCCESS )
		throw std::runtime_error( "Socket multicast binding error. Can't start. " );

	for ( aa = adapter_addresses; aa != NULL; aa = aa->Next ) {
		if ( aa->FirstGatewayAddress != NULL ) {
			selectedInterface = aa->FirstUnicastAddress;
			break;
		}
	}
	if ( selectedInterface == NULL )
		throw std::runtime_error( "Socket multicast binding error. Can't start. " );
	else {
		mreq.imr_interface = ((struct sockaddr_in*)selectedInterface->Address.lpSockaddr)->sin_addr;
	}

	if ( setsockopt( receiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof( mreq ) ) == SOCKET_ERROR ) {
		throw std::runtime_error( "Socket multicast binding error. Can't start. " );
	}
	free( adapter_addresses );

}

void UDPSocketMulticast_Windows::closeSocket() {
	shutdown( receiveSocket, SD_BOTH );
	closesocket( sendSocket );
	closesocket( receiveSocket );
	WSACleanup();
	closed = true;
}

void UDPSocketMulticast_Windows::sendPacket( string message ) {
	sendto( sendSocket, message.c_str(), message.length(), 0, (struct sockaddr *) &multicastSockaddr, sizeof( multicastSockaddr ) );
}

int8_t UDPSocketMulticast_Windows::receivePacket( string& message, string& senderIp ) {

	wchar_t waddr_t[INET_ADDRSTRLEN];
	char addr[INET_ADDRSTRLEN];
	size_t wcstombs_sReturnValue;
	DWORD addrBufferLen = INET_ADDRSTRLEN;
	vector<char> messageBuffer( 371 );

	struct sockaddr_in senderAddr;
	socklen_t addrlen = sizeof( senderAddr );

	int8_t recvfromCode = recvfrom( receiveSocket, messageBuffer.data(), messageBuffer.size(), 0, (struct sockaddr *) &senderAddr, &addrlen );
	if ( recvfromCode < 0 )
		return recvfromCode;

	message.assign( messageBuffer.cbegin(), messageBuffer.cend() );
	WSAAddressToStringW( (struct sockaddr*) &senderAddr, addrlen, NULL, waddr_t, &addrBufferLen );
	/*wchar_t is needed by WSAAddressToStringW but it is not needed to store the IP and it has
	compatibility issues with Linux.*/
	if ( wcstombs_s( &wcstombs_sReturnValue, addr, INET_ADDRSTRLEN, waddr_t, INET_ADDRSTRLEN - 1 ) != 0 )
		return -1;

	senderIp.assign( addr );
	return 0;
}

#endif

