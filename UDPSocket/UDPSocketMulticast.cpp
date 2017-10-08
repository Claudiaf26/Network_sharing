#include "UDPSocketMulticast.h"

UDPSocketMulticast::UDPSocketMulticast( string ip, uint16_t multicastPort ) {
#ifdef _WIN32
	base = unique_ptr<UDPSocketMulticast_Interface>( new UDPSocketMulticast_Windows( ip, multicastPort ) );
#endif
#ifdef __linux__
	base = std::unique_ptr<UDPSocketMulticast_Interface>( new UDPSocketMulticast_Linux( ip, multicastPort ) );
#endif
}

UDPSocketMulticast::~UDPSocketMulticast() {}

void UDPSocketMulticast::sendPacket( std::string message ) {
	base->sendPacket( message );
}

int8_t UDPSocketMulticast::receivePacket( std::string& message, std::string& senderIp ) {
	return base->receivePacket( message, senderIp );
}

void UDPSocketMulticast::closeSocket() {
	base->closeSocket();
}
