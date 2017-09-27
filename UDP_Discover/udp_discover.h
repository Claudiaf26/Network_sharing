#ifndef UDP_DISCOVER
#define UDP_DISCOVER
#include <memory>
#include <string>
#ifdef _WIN32
#include "udp_discover_windows.h"
#endif
#ifdef __linux__
#include "udp_discover_linux.h"
#endif

class UDP_Discover {
private:
	unique_ptr<UDP_Discover_Interface> base;

	//Can't be copied, can't be assigned
	UDP_Discover(const UDP_Discover& s);
	UDP_Discover& operator=(const UDP_Discover& s);

public:
	UDP_Discover(std::string userName, string picture);
	~UDP_Discover();

	virtual void run(int8_t mode);
	virtual void run(int8_t mode, std::string newUserName, std::string picture);
	virtual void stop();
	virtual bool changeMode(int8_t newMode);
	virtual std::vector<struct User> getActive();


};


#endif
