#ifndef TCPSERVERSOCKET_INTERFACE
#define TCPSERVERSOCKET_INTERFACE
#include <iostream>
#include <string>
#include <vector>
#include "TCPSocket.h"

using namespace std;

class TCPServerSocket_Interface {

public:
public:
	TCPServerSocket_Interface() {}
	virtual ~TCPServerSocket_Interface() {}
	virtual TCPSocket Accept() = 0;
	virtual void Close() = 0;
};


#endif