#ifndef UDPSOCKETMULTICAST_INTERFACE
#define UDPSOCKETMULTICAST_INTERFACE

#include <iostream>
#include <string>
class UDPSocketMulticast_Interface {
public:
	UDPSocketMulticast_Interface() {
	};

	virtual ~UDPSocketMulticast_Interface() {
	};

	virtual void sendPacket( std::string message ) = 0;
	virtual int8_t receivePacket( std::string& message, std::string& senderIp ) = 0;

};


#endif