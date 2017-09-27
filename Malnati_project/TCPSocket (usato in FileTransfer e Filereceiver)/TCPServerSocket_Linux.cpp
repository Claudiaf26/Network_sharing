#ifdef __linux__

#include "TCPServerSocket_Linux.h"

TCPServerSocket_Linux::TCPServerSocket_Linux(uint16_t port) : TCPServerSocket_Interface() {

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0)
		throw std::invalid_argument("Error during socket: " + errno);


	address.sin_family = AF_INET;
	address.sin_addr.s_addr=htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (bind(s, (struct sockaddr*) &address, sizeof(address)) < 0)
		throw std::invalid_argument("Error during binding: " + errno);

	if(listen(s, SOMAXCONN) < 0)
		throw std::invalid_argument("Error during listen: " + errno);

	started = true;

}


TCPServerSocket_Linux::~TCPServerSocket_Linux() {
	if (started != false) {
		started = false;
		Close();
	}
}

void TCPServerSocket_Linux::Close() {
	started = false;
	close(s);
}

TCPSocket TCPServerSocket_Linux::Accept() {
	int16_t connectedSocket;
	sockaddr_in clientAddress;
	socklen_t clen= sizeof(clientAddress);
	connectedSocket = accept(s, (struct sockaddr*) &clientAddress, &clen);
	if (connectedSocket < 0 )
		throw std::invalid_argument("The client has problems: " + errno);

	return TCPSocket(connectedSocket);

}
#endif