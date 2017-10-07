#ifdef _WIN32 
#ifndef TCPSOCKET_WINDOWS
#define TCPSOCKET_WINDOWS
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include "TCPSocket_Interface.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace std;


class TCPSocket_Windows : public TCPSocket_Interface{
private:
	bool started;

	WSADATA wsaSocket;
	int16_t  s;
	struct sockaddr_in serverAddress;

public:
	TCPSocket_Windows(string ip, uint16_t port);
	TCPSocket_Windows(int16_t connectedS);
	~TCPSocket_Windows();
	void Close();
	bool Send(vector<char> source);
	bool Receive(vector<char> &dest, uint32_t size, struct timeval timeout );
	string getPeerIp();


};


#endif 
#endif
