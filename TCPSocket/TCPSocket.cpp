#include "TCPSocket.h"

TCPSocket::TCPSocket(string ip, uint16_t port) {
#ifdef _WIN32
	base = unique_ptr<TCPSocket_Windows>(new TCPSocket_Windows(ip, port));
#endif
#ifdef __linux__
	base = std::unique_ptr<TCPSocket_Linux>(new TCPSocket_Linux(ip, port));
#endif
}

TCPSocket::TCPSocket(int16_t connectedSocket) {
#ifdef _WIN32
	base = unique_ptr<TCPSocket_Windows>(new TCPSocket_Windows(connectedSocket));
#endif
#ifdef __linux__
	base = std::unique_ptr<TCPSocket_Linux>(new TCPSocket_Linux(connectedSocket));
#endif
}



TCPSocket::~TCPSocket() {
	base.reset( nullptr );
}

void TCPSocket::Close() {
	base->Close();
}

bool TCPSocket::Send(vector<char> source) {
	return base->Send(source);
}

bool TCPSocket::Receive(vector<char> &dest, uint32_t size, struct timeval timeout) {
	return base->Receive(dest, size, timeout);
}

string TCPSocket::getPeerIp() {
	return base->getPeerIp();
}

