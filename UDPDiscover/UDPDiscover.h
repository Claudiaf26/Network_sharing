#ifdef _WIN32
#ifndef UDP_DISCOVER_WINDOWS
#define UDP_DISCOVER_WINDOWS
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <process.h>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include "../UDPSocket/UDPSocketMulticast.h"
#include "../DataStruct/sharedsingleton.h"
#include "../define.h"

using namespace std;

class UDPDiscover{
private:
	static const uint16_t AGING_SLEEP_TIME = 6000;
	static const uint16_t ADVERTISE_SLEEP_TIME = 4000;
	static const uint16_t DISCOVERY_PORT = 5000;

	thread threads[3]; //Discover, Advertise, Aging

	mutex vectorActiveUsersSynch;
	vector<struct User> activeUsers;

	atomic<int8_t> mode;

	unique_ptr<UDPSocketMulticast> socket;

	string userName;
	string picture;
	string defaultMessage;

	void advertise();
	void discover();
	void aging();

	//Can't be copied, can't be assigned
	UDPDiscover(const UDPDiscover& s);
	UDPDiscover& operator=(const UDPDiscover& s);

public:
	void run(int8_t mode);
	void run(int8_t mode, string newUserName, string picture);

	void stop();
	bool changeMode(int8_t newMode);

	vector<struct User> getActive();

	UDPDiscover(string userName, string picture);
	~UDPDiscover();

};
#endif // !UDP_DISCOVER_WINDOWS

#endif
