#ifdef _WIN32

#include "TCPSocket_Windows.h"

TCPSocket_Windows::TCPSocket_Windows(string ip, uint16_t port) : TCPSocket_Interface(), constructor_type(1){
	WSAStartup(0x0202, &wsaSocket);
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		throw std::invalid_argument("Error during socket: " + WSAGetLastError());
	}
	u_int reuse = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) == SOCKET_ERROR)
		throw std::runtime_error("Socket option error. " + WSAGetLastError());

	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(serverAddress.sin_addr));
	serverAddress.sin_port = htons(port);

    if(connect(s, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr)) == SOCKET_ERROR){
        throw std::invalid_argument("Error during connect: " + WSAGetLastError());
    }
	
	int32_t bufferSize = 65664;
	setsockopt( s, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof( bufferSize ) );

	started = TRUE;

}

TCPSocket_Windows::TCPSocket_Windows(int16_t connectedS): constructor_type( 2 ) {
	WSAStartup(0x0202, &wsaSocket);
	s = connectedS;
	socklen_t len = sizeof(serverAddress);
	if( getpeername(s, (sockaddr*)&serverAddress, &len) == SOCKET_ERROR)
		throw std::invalid_argument("Socket not valid: " + WSAGetLastError());

	started = true;

}




TCPSocket_Windows::~TCPSocket_Windows() {
	if (started != FALSE) {
		started = FALSE;
		Close();
	}
}

void TCPSocket_Windows::Close() {
	started = FALSE;
	closesocket(s);
	if(constructor_type != 2 )
		WSACleanup();
}


bool TCPSocket_Windows::Receive(vector<char> &dest, uint32_t size, struct timeval timeout) {
	/*Receive exactly size bytes. For each read, a timeout time is waited before assuming the client is disconnected.
	If the client disconnects or is assumed disconnected, false is returned. */
	if (started == false || size > INT32_MAX)
		return false;
	fd_set readset;
	uint32_t received=0;
	int32_t i =0;
	struct timeval this_timeout = timeout;

	FD_ZERO(&readset); FD_SET(s, &readset);
	
	dest.resize(size);

	while (received < size && select(FD_SETSIZE, &readset, NULL, NULL, &this_timeout) > 0 ) {
		i = recv(s, dest.data()+received, size-received, 0);
		if (i == 0 || i == SOCKET_ERROR) {
			cout << "Prova " <<WSAGetLastError() << endl;
			return false;
		} else {
			received = received + i;
			//la select resetta!
			FD_ZERO(&readset); FD_SET(s, &readset);
			this_timeout = timeout;
		}
	}

	if ( received != size ) {
		cout << "received != size" << endl;
		return false;
	} else {
		return true;
	}


}

string TCPSocket_Windows::getPeerIp() {
	char ipAddress[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &(serverAddress.sin_addr), ipAddress, INET_ADDRSTRLEN );
	return string(ipAddress);
}

bool TCPSocket_Windows::Send(vector<char> source) {
	/*Sends the entire source array.*/
	if (started == false || source.size() > INT32_MAX)
		return false;
	uint32_t sent = 0;
	int32_t i = 0;

	while (i != SOCKET_ERROR && sent < source.size()) {
		i = send(s, source.data() + sent, source.size() - sent, 0);
		sent = sent + i;
	}
	if (sent < source.size() || i== SOCKET_ERROR)
		return false;
	else
		return true;
}
#endif
