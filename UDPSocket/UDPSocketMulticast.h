#ifndef UDPSOCKETMULTICAST
#define UDPSOCKETMULTICAST

#include <memory>
#include <string>

#ifdef _WIN32
#include "UDPSocketMulticast_Windows.h"
#endif
#ifdef __linux__
#include "UDPSocketMulticast_Linux.h"
#endif

class UDPSocketMulticast {
private:
	unique_ptr<UDPSocketMulticast_Interface> base;

	//Can't be copied, can't be assigned
	UDPSocketMulticast( const UDPSocketMulticast& );
	UDPSocketMulticast& operator=( const UDPSocketMulticast& s );

public:
	UDPSocketMulticast( string ip, uint16_t multicastPort );
	~UDPSocketMulticast();
	virtual void sendPacket( std::string message );
	virtual int8_t receivePacket( std::string& message, std::string& senderIp );
	virtual void closeSocket();

};

#endif
