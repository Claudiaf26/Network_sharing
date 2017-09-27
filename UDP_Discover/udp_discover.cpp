#include "udp_discover.h"

UDP_Discover::UDP_Discover(std::string userName, string picture) {
#ifdef _WIN32
	base = unique_ptr<UDP_Discover_Windows>(new UDP_Discover_Windows(userName, picture));
#endif
#ifdef __linux__
	base = std::unique_ptr<UDP_Discover_Linux>(new UDP_Discover_Linux(userName, picture));
#endif
}

UDP_Discover::~UDP_Discover() {
	base.release();
}

void UDP_Discover::run(int8_t mode) {
	base->run(mode);
}
void UDP_Discover::run(int8_t mode, std::string newUserName, std::string picture) {
	base->run(mode, newUserName, picture);
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
