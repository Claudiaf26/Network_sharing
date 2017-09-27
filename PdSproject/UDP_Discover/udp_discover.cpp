#include "udp_discover.h"

UDP_Discover::UDP_Discover(std::string userName) {
#ifdef _WIN32
    base = unique_ptr<UDP_Discover_Windows>(new UDP_Discover_Windows(userName));
#endif
#ifdef __linux__
    base = std::unique_ptr<UDP_Discover_Linux>(new UDP_Discover_Linux(userName));
#endif
}

UDP_Discover::~UDP_Discover() {
    base.release();
}

void UDP_Discover::run(int8_t mode) {
    base->run(mode);
}
void UDP_Discover::run(int8_t mode, string newUserName) {
    base->run(mode, newUserName);
}
void UDP_Discover::stop() {
    base->stop();
}
bool UDP_Discover::changeMode(int8_t newMode) {
    return base->changeMode(newMode);
}

std::vector<struct User> UDP_Discover::getActive() {
    return base->getActive();
}
