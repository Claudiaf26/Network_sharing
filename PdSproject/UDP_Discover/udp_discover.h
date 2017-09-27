#ifndef UDP_DISCOVER_H
#define UDP_DISCOVER_H

#include <memory>
#include <string>
#ifdef _WIN32
#include "UDP_Discover/udp_discover_windows.h"
#endif
#ifdef __linux__
#include "UDP_Discover/udp_discover_linux.h"
#endif

class UDP_Discover {
private:
    unique_ptr<UDP_Discover_Interface> base;

    //Can't be copied, can't be assigned
    UDP_Discover(const UDP_Discover& s);
    UDP_Discover& operator=(const UDP_Discover& s);

public:
    UDP_Discover(std::string userName);
    ~UDP_Discover();

    virtual void run(int8_t mode);
    virtual void run(int8_t mode, string newUserName);
    virtual void stop();
    virtual bool changeMode(int8_t newMode);
    virtual std::vector<struct User> getActive();


};


#endif // UDP_DISCOVER_H
