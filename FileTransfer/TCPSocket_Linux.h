#ifdef __linux__
#ifndef TCPSOCKET_LINUX
#define TCPSOCKET_LINUX
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
#include "TCPSocket_Interface.h"

using namespace std;


class TCPSocket_Linux : public TCPSocket_Interface{
private:
	bool started;

	int16_t  s;
	struct sockaddr_in serverAddress;

public:
	TCPSocket_Linux(string ip, uint16_t port);
	TCPSocket_Linux(int16_t connectedS);
	~TCPSocket_Linux();
	void Close();
	bool Send(vector<char> source);
	bool Receive(vector<char> &dest, uint32_t size, struct timeval timeout );

};


#endif
#endif
