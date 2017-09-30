#ifdef _WIN32	
#ifndef UDP_DISCOVER_WINDOWS

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <vector>
#include <string>
#include <ws2tcpip.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <process.h>
#include <memory>
#include "udp_discover_interface.h"
#include "../UDPSocket/UDPSocketMulticast.h"

using namespace std;

class UDP_Discover_Windows : public UDP_Discover_Interface {
private:
	static const uint16_t AGING_SLEEP_TIME = 6000;
	static const uint16_t ADVERTISE_SLEEP_TIME = 4000;
	static const uint16_t DISCOVERY_PORT = 5000;

	HANDLE threadsHandle[3]; //Discover, Advertise, Aging

	CRITICAL_SECTION vectorActiveUsersSynch;
	vector<struct User> activeUsers;

	CRITICAL_SECTION modeSynch;
	int8_t mode;

	unique_ptr<UDPSocketMulticast> socket;

	string userName;
	string picture;
	string defaultMessage;

	void advertise();
	void discover();
	void aging();

	//Wrapper for _beginthreadex.
	static unsigned int __stdcall Advertise(void* p) {
		UDP_Discover_Windows* udw = static_cast<UDP_Discover_Windows*>(p);
		udw->advertise();
		return 0;
	}
	static unsigned int __stdcall Discover(void* p) {
		UDP_Discover_Windows* udw = static_cast<UDP_Discover_Windows*>(p);
		udw->discover();
		return 0;
	}
	static unsigned int __stdcall Aging(void* p) {
		UDP_Discover_Windows* udw = static_cast<UDP_Discover_Windows*>(p);
		udw->aging();
		return 0;
	}


	//Can't be copied, can't be assigned
	UDP_Discover_Windows(const UDP_Discover_Windows& s);
	UDP_Discover_Windows& operator=(const UDP_Discover_Windows& s);

public:
	void run(int8_t mode);
	void run(int8_t mode, string newUserName, string picture);

	void stop();
	bool changeMode(int8_t newMode);

	vector<struct User> getActive();

	UDP_Discover_Windows(string userName, string picture);
	~UDP_Discover_Windows();

};
#endif // !UDP_DISCOVER_WINDOWS

#endif
