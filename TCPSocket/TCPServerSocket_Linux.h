#ifdef __linux__

#ifndef TCPSERVERSOCKET_LINUX
#define TCPSERVERSOCKET_LINUX

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include "TCPSocket.h"
#include "TCPServerSocket_Interface.h"

using namespace std;


class TCPServerSocket_Linux : public TCPServerSocket_Interface {
private:
	bool started;

	int16_t s;
	sockaddr_in address;

	//Can't be copied, can't be assigned
	TCPServerSocket_Linux(const TCPServerSocket_Linux& s);
	TCPServerSocket_Linux& operator=(const TCPServerSocket_Linux& s);

public:
	TCPServerSocket_Linux(uint16_t port);
	~TCPServerSocket_Linux();
	TCPSocket Accept();
	void Close();


};

#endif
#endif
