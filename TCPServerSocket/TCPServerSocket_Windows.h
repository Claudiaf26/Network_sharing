#ifdef _WIN32

#ifndef TCPSERVERSOCKET_WINDOWS
#define TCPSERVERSOCKET_WINDOWS
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include "../TCPSocket/TCPSocket.h"
#include "TCPServerSocket_Interface.h"
#pragma comment(lib, "Ws2_32.lib")


using namespace std;


class TCPServerSocket_Windows : public TCPServerSocket_Interface {
private:
	bool started;

	WSADATA wsaSocket;
	int16_t s;
	sockaddr_in address;

	//Can't be copied, can't be assigned
	TCPServerSocket_Windows(const TCPServerSocket_Windows& s);
	TCPServerSocket_Windows& operator=(const TCPServerSocket_Windows& s);

public:
	TCPServerSocket_Windows(uint16_t port);
	~TCPServerSocket_Windows();
	TCPSocket Accept();
	void Close();


};

#endif 
#endif
