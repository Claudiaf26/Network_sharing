#ifndef UDPDISCOVER
#define UDPDISCOVER
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include "../UDPSocket/UDPSocketMulticast.h"
#include "../DataStruct/userlistsingleton.h"
#include "../define.h"

using namespace std;

class UDPDiscover {
private:
    static const uint16_t AGING_SLEEP_TIME = 2000;
    static const uint16_t ADVERTISE_SLEEP_TIME = 2000;
	static const uint16_t DISCOVERY_PORT = 5000;

	thread threads[3]; //Discover, Advertise, Aging

	mutex vectorActiveUsersSynch;
	vector<struct User> activeUsers;

	atomic<int8_t> mode;

	UDPSocketMulticast socket;

	string userName;
	string picture;
	string defaultMessage;

	void advertise();
	void discover();
	void aging();

	//Can't be copied, can't be assigned
	UDPDiscover( const UDPDiscover& s );
	UDPDiscover& operator=( const UDPDiscover& s );

public:
	void run( int8_t mode );

	void stop();
	bool changeMode( int8_t newMode );
	void changeUserName( string newUserName );
	void changePicture( string newPicture );

	vector<struct User> getActive();

	UDPDiscover( string userName, string picture );
	~UDPDiscover();

};
#endif // !UDP_DISCOVER
