#ifndef TCPSERVERSOCKET
#define TCPSERVERSOCKET
#include <iostream>
#include <string>
#include <vector>
#include "TCPServerSocket_Interface.h"
#include "TCP_Socket/TCPSocket.h"

#ifdef _WIN32
#include "TCPServerSocket_Windows.h"
#endif
#ifdef __linux__
#include "TCPServerSocket_Linux.h"
#endif

using namespace std;

class TCPServerSocket{
private:
	unique_ptr<TCPServerSocket_Interface> base;

	TCPServerSocket(const TCPServerSocket&);
	TCPServerSocket& operator=(const TCPServerSocket& source);

public:
	TCPServerSocket(uint16_t port);
	~TCPServerSocket();
	TCPSocket Accept();
	void Close();

	TCPServerSocket(TCPServerSocket&& source) {
		//Movement constructor. Use with move(TCPSocket)
		this->base = unique_ptr<TCPServerSocket_Interface>(source.base.release());
	}
};


#endif
