#ifdef _WIN32
#include "udp_discover_windows.h"
#pragma comment(lib, "Iphlpapi.lib")

UDP_Discover_Windows::UDP_Discover_Windows(string userN, string pic) : UDP_Discover_Interface(){
	mode = UDS_STOP;
	userName = userN;
	picture = pic;
	char loopch = 0;
	//Initialize synch attributes
	InitializeCriticalSection(&vectorActiveUsersSynch);
	InitializeCriticalSection(&modeSynch);
	mode = UDS_STOP;

	defaultMessage.append("UDPDISCOVERY" + userName + "\r\n" + picture + "\r\n");
	cout << "Default message: " << defaultMessage;
	//Initialize socket on which packets will be sent
	WSAStartup(0x0202, &wsaDataSend);
	sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sendSocket == INVALID_SOCKET)
		throw std::runtime_error("Sending Socket Error. Can't start. ");
	if (setsockopt(sendSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) == SOCKET_ERROR)
		throw std::runtime_error("Socket option error. Can't start. ");

	//Initialize socket on which packets will be received
	WSAStartup(0x0202, &wsaDataReceive);
	receiveSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (receiveSocket == INVALID_SOCKET)
		throw std::runtime_error("Receive Socket Error. Can't start. ");
	u_int reuse = 1;
	if (setsockopt(receiveSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) == SOCKET_ERROR)
		throw std::runtime_error("Socket option error. Can't start. ");
	if (setsockopt(receiveSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) == SOCKET_ERROR)
		throw std::runtime_error("Socket option error. Can't start. ");
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	addr.sin_port = htons(DISCOVERY_PORT); 

	if (::bind(receiveSocket, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR)
		throw std::runtime_error("Socket bind error. Can't start. ");

	joinMulticast();

	//Prepare multicast address structure
	memset(&MulticasSockaddr, 0, sizeof(MulticasSockaddr));
	MulticasSockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "239.0.0.100", &(MulticasSockaddr.sin_addr));
	MulticasSockaddr.sin_port = htons(DISCOVERY_PORT);

}

UDP_Discover_Windows::~UDP_Discover_Windows() {
	stop();
	WaitForMultipleObjects(3, threadsHandle, TRUE, INFINITE); //Threads will naturally terminate.
	
	CloseHandle(threadsHandle[0]);
	CloseHandle(threadsHandle[1]);
	CloseHandle(threadsHandle[2]);

	closesocket(sendSocket);
	closesocket(receiveSocket);
	WSACleanup();

	DeleteCriticalSection(&vectorActiveUsersSynch);
	DeleteCriticalSection(&modeSynch);
	
}

void UDP_Discover_Windows::joinMulticast() {
	/*
	*This method is needed in Windows due to the kernel
	*choosing the wrong local interface in some cases.
	*Here each interface is checked but only the one connected
	*to a gateway is accepted.
	*THIS METHOD IS NOT PORTABLE!
	*/
	struct ip_mreq mreq;
	inet_pton(AF_INET, "239.0.0.100", &(mreq.imr_multiaddr.s_addr));

	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	PIP_ADAPTER_UNICAST_ADDRESS selectedInterface = NULL;
	ULONG  size;
	if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, NULL, &size) != ERROR_BUFFER_OVERFLOW)
		throw std::runtime_error("Socket bind error. Can't start. ");

	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
	if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, adapter_addresses, &size) != ERROR_SUCCESS)
		throw std::runtime_error("Socket bind error. Can't start. ");

	for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
		if (aa->FirstGatewayAddress != NULL) {
			selectedInterface = aa->FirstUnicastAddress;
			break;
		}
	}
	if (selectedInterface == NULL)
		throw std::runtime_error("Socket bind error. Can't start. ");
	else {
		mreq.imr_interface = ((struct sockaddr_in*)selectedInterface->Address.lpSockaddr)->sin_addr;
	}
	char contenitore[22];
	inet_ntop(AF_INET, &mreq.imr_interface, contenitore, 22);
	cout << "Local interface chosen: " << contenitore << endl;

	if (setsockopt(receiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == SOCKET_ERROR) {
		throw std::runtime_error("Socket bind error. Can't start. ");
	}
	free(adapter_addresses);

}


void UDP_Discover_Windows::advertise() {
	string message;
	int8_t temp_mode;


	/*PROTOCOL:
	1. char[12] = "UDPDISCOVERY";
	2. string name; null terminated string, at most 255 characters.
	3. \r\n
	4. picture name, at most 255 characters
	. \r\n
	*/

	while (true) {
		EnterCriticalSection(&modeSynch);
		temp_mode = mode;
		LeaveCriticalSection(&modeSynch);

		if (temp_mode == UDS_STOP || temp_mode == UDS_HIDDEN) {
			return;
		}
		sendto(sendSocket, defaultMessage.c_str(), defaultMessage.length(), 0, (struct sockaddr *) &MulticasSockaddr, sizeof(MulticasSockaddr));
		Sleep(ADVERTISE_SLEEP_TIME);
	}


}

void UDP_Discover_Windows::discover() {
	wchar_t waddr_t[INET_ADDRSTRLEN];
	char addr[INET_ADDRSTRLEN];
	size_t wcstombs_sReturnValue;
	DWORD addrBufferLen = INET_ADDRSTRLEN;

	struct sockaddr_in senderAddr;
	socklen_t addrlen = sizeof(senderAddr); //PLATFORM DEPENDENT!!!

	struct User  newUsr;
	vector<char> messageBuffer(371);
	string message;
	uint8_t pos1, pos2;

	int8_t temp_mode;

	/*PROTOCOL:
	1. char[12] = "UDPDISCOVERY";
	2. string name; null terminated string, at most 255 characters.
	3. \r\n
	4. picture name, at most 100 characters.
	5. \r\n
	*/

	while (true) {	
		EnterCriticalSection(&modeSynch);
		temp_mode = mode;
		LeaveCriticalSection(&modeSynch);

		if (temp_mode == UDS_STOP) {
			return;
		}

		if(recvfrom(receiveSocket, messageBuffer.data(), messageBuffer.size(), 0, (struct sockaddr *) &senderAddr, &addrlen)<0)
			continue;
		message.assign(messageBuffer.cbegin(), messageBuffer.cend());
		WSAAddressToStringW((struct sockaddr*) &senderAddr, addrlen, NULL, waddr_t, &addrBufferLen);
		/*wchar_t is needed by WSAAddressToStringW but it is not needed to store the IP and it has
		compatibility issues with Linux.*/
		if (wcstombs_s(&wcstombs_sReturnValue, addr, INET_ADDRSTRLEN, waddr_t, INET_ADDRSTRLEN - 1) != 0)
			continue;
		

		/*Packet translation. If the protocol is not respected, just ignore.*/
		pos1 = message.find("UDPDISCOVERY")+strlen("UDPDISCOVERY");
		pos2 = message.find("\r\n");
		if (pos1 == string::npos || pos2 == string::npos)
			continue;
		newUsr.name.assign(message.substr(pos1, pos2-pos1));
		pos1 = pos2 + strlen( "\r\n" );
		pos2 = message.find( "\r\n" , pos1);
		if ( pos1 == string::npos || pos2 == string::npos )
			continue;
		newUsr.picture.assign( message.substr( pos1, pos2 - pos1 ) );
		newUsr.ip.assign(addr);
		pos2 = newUsr.ip.find(":");
		newUsr.ip.assign(newUsr.ip.substr(0, pos2));
		newUsr.age = 3;


		EnterCriticalSection(&vectorActiveUsersSynch);
		auto ele = find(activeUsers.begin(), activeUsers.end(), newUsr);
		if (ele != activeUsers.end()) {
			//The host is still active, refresh it
			(*ele).age = 3;
		} else {
			/*The host has just connected.
			1. Add it to the list of users
			2. Reply with a discovery packet to reduce the delay for cache aligning.
				ACHTUNG! Only if the host is visible!*/
			activeUsers.push_back(newUsr);
			ConnectionSingleton::get_instance().pushNew( newUsr );
			if (temp_mode == UDS_ACTIVE) 
				sendto(sendSocket, defaultMessage.c_str(), defaultMessage.length(), 0, (struct sockaddr *) &MulticasSockaddr, sizeof(MulticasSockaddr));
			
		}
		LeaveCriticalSection(&vectorActiveUsersSynch);

	}

}

void UDP_Discover_Windows::aging() {
	int8_t temp_mode;
	while (true) {
		EnterCriticalSection(&modeSynch);
		temp_mode = mode;
		LeaveCriticalSection(&modeSynch);

		if (temp_mode == UDS_STOP) {
			return;
		}
		EnterCriticalSection(&vectorActiveUsersSynch);
		for (auto it = activeUsers.begin(); it != activeUsers.end();) {
			if ((*it).age>=0) {
				(*it).age--;
				++it;
			}else{
				ConnectionSingleton::get_instance().pushDeleted( *it );
				it = activeUsers.erase(it);
			}
		}
		LeaveCriticalSection(&vectorActiveUsersSynch);
		Sleep(AGING_SLEEP_TIME);
	}
}


void UDP_Discover_Windows::run(int8_t  initialMode) {
	if (initialMode == UDS_STOP || mode != UDS_STOP) 
		return;
	
	mode = initialMode;

	if(initialMode == UDS_ACTIVE){
		threadsHandle[0] =	(HANDLE) _beginthreadex(NULL, 0, &UDP_Discover_Windows::Discover, this, 0, NULL);
		threadsHandle[1] =	(HANDLE) _beginthreadex(NULL, 0, &UDP_Discover_Windows::Advertise, this, 0, NULL);
		threadsHandle[2] =	(HANDLE) _beginthreadex(NULL, 0, &UDP_Discover_Windows::Aging, this, 0, NULL);
	} else if (initialMode == UDS_HIDDEN) {
		threadsHandle[0] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Discover, this, 0, NULL);
		threadsHandle[2] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Aging, this, 0, NULL);

	}

}

void UDP_Discover_Windows::run(int8_t  initialMode, string newUserName, string picture) {
	if (initialMode == UDS_STOP || mode != UDS_STOP)
		return;

	mode = initialMode;
	userName.assign(newUserName);
	picture.assign( picture );

	if (initialMode == UDS_ACTIVE) {
		threadsHandle[0] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Discover, this, 0, NULL);
		threadsHandle[1] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Advertise, this, 0, NULL);
		threadsHandle[2] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Aging, this, 0, NULL);
	} else if (initialMode == UDS_HIDDEN) {
		threadsHandle[0] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Discover, this, 0, NULL);
		threadsHandle[2] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Aging, this, 0, NULL);

	}
}



void UDP_Discover_Windows::stop() {
	EnterCriticalSection(&modeSynch);
	mode = UDS_STOP;
	LeaveCriticalSection(&modeSynch);

	EnterCriticalSection(&vectorActiveUsersSynch);
	activeUsers.clear();
	LeaveCriticalSection(&vectorActiveUsersSynch);

}
bool UDP_Discover_Windows::changeMode(int8_t newMode) {
	if (newMode == UDS_STOP)
		return false;

	int8_t temp_mode;
	EnterCriticalSection(&modeSynch);
	temp_mode = mode;
	LeaveCriticalSection(&modeSynch);
	if (temp_mode == UDS_STOP)
		return false;

	EnterCriticalSection(&modeSynch);
	mode = newMode;
	LeaveCriticalSection(&modeSynch);
	
	if (temp_mode == newMode)
		return false;


	if (temp_mode == UDS_ACTIVE && newMode == UDS_HIDDEN) {
		//Do nothing. The advertise() will automatically close.
		return true;
	} else if (temp_mode == UDS_HIDDEN && newMode == UDS_ACTIVE) {
		//start advertising
		threadsHandle[2] = (HANDLE)_beginthreadex(NULL, 0, &UDP_Discover_Windows::Advertise, this, 0, NULL);
		return true;
	} else {
		return false;
	}


	return true;
}



vector<struct User> UDP_Discover_Windows::getActive() {
	EnterCriticalSection(&vectorActiveUsersSynch);
	vector<struct User> temp(activeUsers);
	LeaveCriticalSection(&vectorActiveUsersSynch);
	return temp;
}

#endif
