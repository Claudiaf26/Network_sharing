#ifdef __linux__
#ifndef UDP_DISCOVER_LINUX
#define UDP_DISCOVER_LINUX
#include <vector>
#include <string>
#include <string.h> //memset
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "udp_discover_interface.h"

using namespace std;

class UDP_Discover_Linux : public UDP_Discover_Interface {
private:
	//ACHTUNG! Windows uses ms, Linux uses s
	static const uint16_t AGING_SLEEP_TIME = 6;
	static const uint16_t ADVERTISE_SLEEP_TIME = 4;
	static const uint16_t DISCOVERY_PORT = 5000;

	/*If more than one shared resource is required, a timer is set so that if the second
	 * resource is not acquired within a certain time all resources are released and
	 * acquiring procedure starts again.*/
	struct timespec mutex_wait;


	pthread_t threadsID[3]; //Discover, Advertise, Aging

	pthread_mutex_t vectorActiveUsersSynch;
	vector<struct User> activeUsers;

	pthread_mutex_t modeSynch;
	int8_t mode;

	int16_t sendSocket;
	int16_t receiveSocket;
	sockaddr_in MulticasSockaddr;

	string userName;
	string defaultMessage;


	void* advertise();
	void* discover();
	void* aging();

	//Wrapper for pthread_create
	static void* Advertise(void* p) {
		UDP_Discover_Linux* udw = static_cast<UDP_Discover_Linux*>(p);
		udw->advertise();
		return 0;
	}
	static void* Discover(void* p) {
		UDP_Discover_Linux* udw = static_cast<UDP_Discover_Linux*>(p);
		udw->discover();
		return 0;
	}
	static void* Aging(void* p) {
		UDP_Discover_Linux* udw = static_cast<UDP_Discover_Linux*>(p);
		udw->aging();
		return 0;
	}


	//Can't be copied, can't be assigned
	UDP_Discover_Linux(const UDP_Discover_Linux& s);
	UDP_Discover_Linux& operator=(const UDP_Discover_Linux& s);

public:
	void run(int8_t mode);
	void run(int8_t mode, string newUserName, string picture);

	void stop();
	bool changeMode(int8_t newMode);

	vector<struct User> getActive();

	UDP_Discover_Linux(string userName, string picture);
	~UDP_Discover_Linux();

};

#endif // !UDP_DISCOVER_LINUX

#endif
