#ifndef TCPSOCKET_INTERFACE
#define TCPSOCKET_INTERFACE
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class TCPSocket_Interface{
public:
	TCPSocket_Interface() {};
	virtual ~TCPSocket_Interface() {};
	virtual void Close() = 0;
	virtual bool Send(vector<char> source) = 0;
	virtual bool Receive(vector<char> &dest, uint32_t size, struct timeval timeout) = 0;
	//TCPSocket_Interface(TCPSocket_Interface&& rval) { }

};


#endif
