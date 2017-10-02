#ifdef _WIN32
#ifndef UDPSOCKETMULTICAST_WINDOWS
#define UDPSOCKETMULTICAST_WINDOWS
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <Iphlpapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "UDPSocketMulticast_Interface.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


class UDPSocketMulticast_Windows : public UDPSocketMulticast_Interface {
private:
	static const uint8_t REFRESH_MULTICAST_SECONDS=120;

	WSADATA wsaDataSend;
	int16_t sendSocket;
	WSADATA wsaDataReceive;
	int16_t receiveSocket;

	sockaddr_in multicastSockaddr;
	string multicastIp;
	uint16_t multicastPort;
	struct ip_mreq mreq;

	void joinMulticast();

	mutex mutex_wait_for_refresh;
	condition_variable cv_wait_for_refresh;
	void refreshMulticast();
	thread multicastRefresher;

	boolean closed;

public:
	UDPSocketMulticast_Windows( string ip, uint16_t multicastPort );
	~UDPSocketMulticast_Windows();

	void sendPacket( string message );
	int8_t receivePacket( string& message, string& senderIp );

	void closeSocket();
};

#endif
#endif