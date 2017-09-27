#include "TCPServerSocket.h"

TCPServerSocket::TCPServerSocket(uint16_t port) {
#ifdef _WIN32
	base = unique_ptr<TCPServerSocket_Windows>(new TCPServerSocket_Windows(port));
#endif
#ifdef __linux__
	base = std::unique_ptr<TCPServerSocket_Linux>(new TCPServerSocket_Linux(port));
#endif
}

TCPServerSocket::~TCPServerSocket() {
	base.release();
}

TCPSocket TCPServerSocket::Accept() {
	return base->Accept();
}
void TCPServerSocket::Close() {
	base->Close();
}