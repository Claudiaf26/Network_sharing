#ifdef _WIN32

#include "TCPServerSocket_Windows.h"

TCPServerSocket_Windows::TCPServerSocket_Windows(uint16_t port) : TCPServerSocket_Interface() {
	WSAStartup(0x0202, &wsaSocket);

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) 
		throw std::invalid_argument("Error during socket: " + WSAGetLastError());
	u_int reuse = 1;
	if ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof( reuse ) ) == SOCKET_ERROR )
		throw std::invalid_argument( "Error during socket: " + WSAGetLastError() );


	address.sin_family = AF_INET;
	address.sin_addr.s_addr=htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (::bind(s, (struct sockaddr*) &address, sizeof(address)) == SOCKET_ERROR)
		throw std::invalid_argument("Error during binding: " + WSAGetLastError());

	if(listen(s, SOMAXCONN) == SOCKET_ERROR)
		throw std::invalid_argument("Error during listen: " + WSAGetLastError());

	started = true;

}


TCPServerSocket_Windows::~TCPServerSocket_Windows() {
	if (started != FALSE) {
		started = FALSE;
		cout <<"Entra? ";
		Close();
	}
}

void TCPServerSocket_Windows::Close() {
	started = FALSE;
	closesocket(s);
	WSACleanup();
}

TCPSocket TCPServerSocket_Windows::Accept() {
	int16_t connectedSocket;
	sockaddr_in clientAddress;
	socklen_t clen= sizeof(clientAddress);
	connectedSocket = accept(s, (struct sockaddr*) &clientAddress, &clen);
	if ( connectedSocket == INVALID_SOCKET) {
		if ( WSAGetLastError() == 10004 )
			throw std::domain_error( "SOCKETCLOSED" );
		else
			throw std::invalid_argument( to_string(WSAGetLastError()) );
	}

	int32_t bufferSize = 65664;
	setsockopt( connectedSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof( bufferSize ) );
	return TCPSocket(connectedSocket);

}
#endif
