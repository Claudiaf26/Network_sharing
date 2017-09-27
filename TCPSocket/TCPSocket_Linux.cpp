#ifdef __linux__

#include "TCPSocket_Linux.h"

TCPSocket_Linux::TCPSocket_Linux(string ip, uint16_t port) : TCPSocket_Interface(){
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0) {
		throw std::invalid_argument("Error during socket: " + errno);
	}
	u_int reuse = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("Socket option error. " + errno);

	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr));
	serverAddress.sin_port = htons(port);

	if(connect(s, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr)) < 0)
		throw std::invalid_argument("Error during connect: " + errno);

	int32_t bufferSize = 65664;
	setsockopt( s, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof( bufferSize ) );

		started = true;

}

TCPSocket_Linux::TCPSocket_Linux(int16_t connectedS) {
	s = connectedS;
	socklen_t len = sizeof(serverAddress);
	if( getpeername(s, (sockaddr*)&serverAddress, &len) < 0)
		throw std::invalid_argument("Socket not valid: " + errno);

	started = true;

}

TCPSocket_Linux::~TCPSocket_Linux() {
	if (started != false) {
		started = false;
		Close();
	}
}

void TCPSocket_Linux::Close() {
	started = false;
	close(s);
}


bool TCPSocket_Linux::Receive(vector<char> &dest, uint32_t size, struct timeval timeout) {
	/*Receive exactly size bytes. For each read, a timeout time is waited before assuming the client is disconnected.
	If the client disconnects or is assumed disconnected, false is returned. */
	if (started == false || size > INT32_MAX)
		return false;
	fd_set readset;
	uint32_t received=0;
	int32_t i=0;
	struct timeval this_timeout = timeout;

	FD_ZERO(&readset); FD_SET(s, &readset);

	dest.resize(size);

	while (received < size && select(FD_SETSIZE, &readset, NULL, NULL, &this_timeout) > 0 ) {
		i = recv(s, dest.data()+received, size-received, 0);
		if (i <= 0) {
			return false;
		} else {
			received = received + i;
			//la select resetta!
			FD_ZERO(&readset); FD_SET(s, &readset);
			this_timeout = timeout;
		}
	}

	if (received != size)
		return false;
	else {
		return true;
	}


}

bool TCPSocket_Linux::Send(vector<char> source) {
	/*Sends the entire source array.*/
	if (started == false || source.size()>INT32_MAX)
		return false;
	uint32_t sent = 0;
	int32_t i = 1;

	while (i > 0 && sent < source.size()) {
		i = send(s, source.data() + sent, source.size() - sent, MSG_NOSIGNAL );
		sent = sent + i;
	}
	if (sent < source.size())
		return false;
	else
		return true;
}

#endif
