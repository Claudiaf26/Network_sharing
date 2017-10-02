#ifdef __linux__

#ifndef UDP_DISCOVER_LINUX
#define UDP_DISCOVER_LINUX
#include <vector>
#include <string>
#include <string.h> //memset
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "UDPSocketMulticast_Interface.h"
using namespace std;


class UDPSocketMulticast_Linux : public UDPSocketMulticast_Interface {
private:
	static const uint8_t REFRESH_MULTICAST_SECONDS=120;
	
	int16_t sendSocket;
	int16_t receiveSocket;

	sockaddr_in multicastSockaddr;
	string multicastIp;
	uint16_t multicastPort;
	
	struct ip_mreq mreq;
	
	mutex mutex_wait_for_refresh;
 	condition_variable cv_wait_for_refresh;
 	void refreshMulticast();
 	thread multicastRefresher;
	

	bool closed;

public:
	UDPSocketMulticast_Linux( string ip, uint16_t multicastPort );
	~UDPSocketMulticast_Linux();

	void sendPacket( string message );
	int8_t receivePacket( std::string& message, std::string& senderIp );
	void closeSocket();
};

#endif
#endif
