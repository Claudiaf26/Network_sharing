#ifndef TCPSOCKET
#define TCPSOCKET
#include <memory>
#include <string>
#include "TCPSocket_Interface.h"

#ifdef _WIN32
#include "TCPSocket_Windows.h"
#endif
#ifdef __linux__
#include "TCPSocket_Linux.h"
#endif

class TCPSocket {
private:
	unique_ptr<TCPSocket_Interface> base;

	TCPSocket(const TCPSocket&);
	TCPSocket& operator=(const TCPSocket& source);

public:
	TCPSocket(string ip, uint16_t port);
	TCPSocket(int16_t connectedSocket);

	~TCPSocket();
	void Close();
	bool Send(vector<char> source);
	bool Receive(vector<char> &dest, uint32_t size, struct timeval timeout);
	string getPeerIp();
	TCPSocket(TCPSocket&& source) {
		//Movement constructor. Use with move(TCPSocket)
		this->base = unique_ptr<TCPSocket_Interface>(source.base.release());
	}


};



#endif
