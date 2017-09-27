#ifdef __linux__
#include "udp_discover_linux.h"
 UDP_Discover_Linux::UDP_Discover_Linux(string userN, string picture) : UDP_Discover_Interface(){
	mode = UDS_STOP;
	userName = userN;

	mutex_wait.tv_sec = 1;
	mutex_wait.tv_nsec = 0;

	char loopch = 0;
	//Initialize synch attributes
	vectorActiveUsersSynch = PTHREAD_MUTEX_INITIALIZER;
	modeSynch = PTHREAD_MUTEX_INITIALIZER;

	mode = UDS_STOP;
	defaultMessage.append("UDPDISCOVERY" + userName + "\r\n" + picture + "\r\n");

	//Initialize socket on which packets will be sent
	sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sendSocket < 0)
		throw std::runtime_error("Sending Socket Error. Can't start. ");
	if (setsockopt(sendSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
		throw std::runtime_error("Socket option error. Can't start. ");

	//Initialize socket on which packets will be received
	receiveSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (receiveSocket < 0)
		throw std::runtime_error("Receive Socket Error. Can't start. ");
	u_int reuse = 1;
	if (setsockopt(receiveSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("Socket option error. Can't start. ");
	if (setsockopt(receiveSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
		throw std::runtime_error("Socket option error. Can't start. ");
	sockaddr_in addr;
	struct ip_mreq mreq;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(DISCOVERY_PORT);
	if (bind(receiveSocket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		throw std::runtime_error("Socket bind error. Can't start. ");
	inet_pton(AF_INET, "239.0.0.100", &(mreq.imr_multiaddr.s_addr));
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); //Local interface

	if (setsockopt(receiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
		throw std::runtime_error("Socket bind error. Can't start. ");
	}

	//Prepare multicast address structure
	memset(&MulticasSockaddr, 0, sizeof(MulticasSockaddr));
	MulticasSockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "239.0.0.100", &(MulticasSockaddr.sin_addr));
	MulticasSockaddr.sin_port = htons(DISCOVERY_PORT);

}


 UDP_Discover_Linux::~UDP_Discover_Linux() {
	 uint16_t temp_mode=mode;
	 stop();
	 if(temp_mode == UDS_ACTIVE){
		 pthread_join(threadsID[0], NULL);
		 pthread_join(threadsID[1], NULL);
		 pthread_join(threadsID[2], NULL);
	 }else  if (temp_mode == UDS_HIDDEN){
		pthread_join(threadsID[0], NULL);
		pthread_join(threadsID[2], NULL);
	 }


 	close(sendSocket);
 	close(receiveSocket);

 	pthread_mutex_destroy(&vectorActiveUsersSynch);
 	pthread_mutex_destroy(&modeSynch);

 }

 void* UDP_Discover_Linux::advertise() {
 	string message;
 	int8_t temp_mode;


 	while (true) {
 		pthread_mutex_lock(&modeSynch);
 		temp_mode = mode;
 		pthread_mutex_unlock(&modeSynch);

 		if (temp_mode == UDS_STOP || temp_mode == UDS_HIDDEN) {
 			return 0;
 		}
 		sendto(sendSocket, defaultMessage.c_str(), defaultMessage.length(), 0, (struct sockaddr *) &MulticasSockaddr, sizeof(MulticasSockaddr));

 		sleep(ADVERTISE_SLEEP_TIME);
 	}
 	return 0;
 }


 void* UDP_Discover_Linux::discover() {
 	char* addr;
 	struct sockaddr_in senderAddr;
 	socklen_t addrlen = sizeof(senderAddr);

 	struct User  newUsr;
 	vector<char> messageBuffer(270);
 	string message;
 	uint8_t pos1, pos2;

 	int8_t temp_mode;


 	while (true) {
 		pthread_mutex_lock(&modeSynch);
 		temp_mode = mode;
 		pthread_mutex_unlock(&modeSynch);

 		if (temp_mode == UDS_STOP) {
 			return 0;
 		}
 		if(recvfrom(receiveSocket, messageBuffer.data(), messageBuffer.size(), 0, (struct sockaddr *) &senderAddr, &addrlen)<0)
 			continue;

 		message.assign(messageBuffer.cbegin(), messageBuffer.cend());
 		addr = inet_ntoa(senderAddr.sin_addr);

 		/*Packet translation. If the protocol is not respected, just ignore.*/
 		pos1 = message.find("UDPDISCOVERY")+strlen("UDPDISCOVERY");
 		pos2 = message.find("\r\n");
 		if (pos1 == string::npos || pos2 == string::npos)
 			continue;
 		newUsr.name.assign(message.substr(pos1, pos2-pos1));
		pos1 = pos2 + strlen( "\r\n" );
		pos2 = message.find( "\r\n", pos1 );
		if ( pos1 == string::npos || pos2 == string::npos )
			continue;
		newUsr.picture.assign( message.substr( pos1, pos2 - pos1 ) );

 		newUsr.ip.assign(addr);
 		newUsr.age = 3;
 		cout <<newUsr.name <<" " <<newUsr.picture <<endl;
 		pthread_mutex_lock(&vectorActiveUsersSynch);
 		auto ele = find(activeUsers.begin(), activeUsers.end(), newUsr);
 		if (ele != activeUsers.end()) {
 			//The host is still active, refresh it
 			(*ele).age = 3;

 		} else {
 			/*The host has just connected.
 			1. Add it to the list of users.
 			2. Reply with a discovery packet to reduce the delay for cache aligning.
 				ACHTUNG! Only if the host is visible!*/
			ConnectionSingleton::get_instance().pushNew( newUsr );
 			activeUsers.push_back(newUsr);
 			if (temp_mode == UDS_ACTIVE)
 				sendto(sendSocket, defaultMessage.c_str(), defaultMessage.length(), 0, (struct sockaddr *) &MulticasSockaddr, sizeof(MulticasSockaddr));
 		}
 		pthread_mutex_unlock(&vectorActiveUsersSynch);

 	}
 	return 0;
 }


 void* UDP_Discover_Linux::aging() {
 	int8_t temp_mode;
 	while (true) {
 		pthread_mutex_lock(&modeSynch);
 		temp_mode = mode;
 		pthread_mutex_unlock(&modeSynch);

 		if (temp_mode == UDS_STOP) {
 			return 0;
 		}
 		pthread_mutex_lock(&vectorActiveUsersSynch);
 		for (auto it = activeUsers.begin(); it != activeUsers.end();) {
 			if ((*it).age>=0) {
 				cout <<"Found: "<<(*it).name <<" " <<(*it).ip <<" age: " <<(*it).age<<endl;
 				(*it).age--;
 				++it;
 			}else{
				ConnectionSingleton::get_instance().pushDeleted( *it );
 				cout<<(*it).name  <<" disconnected"<<endl;
 				it = activeUsers.erase(it);

 			}
 		}
 		pthread_mutex_unlock(&vectorActiveUsersSynch);
 		sleep(AGING_SLEEP_TIME);
 	}
 	return 0;
 }

 void UDP_Discover_Linux::run(int8_t initialMode) {
 	if (initialMode == UDS_STOP || mode != UDS_STOP)
 		return;

 	mode = initialMode;

 	if(initialMode == UDS_ACTIVE){
 		pthread_create(&threadsID[0], NULL, &UDP_Discover_Linux::Discover, this);
 		pthread_create(&threadsID[1], NULL, &UDP_Discover_Linux::Advertise, this);
 		pthread_create(&threadsID[2], NULL, &UDP_Discover_Linux::Aging, this);
 	} else if (initialMode == UDS_HIDDEN) {
 		pthread_create(&threadsID[0], NULL, &UDP_Discover_Linux::Discover, this);
 		pthread_create(&threadsID[2], NULL, &UDP_Discover_Linux::Aging, this);
 	}
 }


 void UDP_Discover_Linux::run(int8_t initialMode, string newUserName, string picture) {
 	if (initialMode == UDS_STOP || mode != UDS_STOP)
 		return;

 	mode = initialMode;
	userName.assign(newUserName);
	picture.assign( picture );

 	if(initialMode == UDS_ACTIVE){
 		pthread_create(&threadsID[0], NULL, &UDP_Discover_Linux::Discover, this);
 		pthread_create(&threadsID[1], NULL, &UDP_Discover_Linux::Advertise, this);
 		pthread_create(&threadsID[2], NULL, &UDP_Discover_Linux::Aging, this);
 	} else if (initialMode == UDS_HIDDEN) {
 		pthread_create(&threadsID[0], NULL, &UDP_Discover_Linux::Discover, this);
 		pthread_create(&threadsID[2], NULL, &UDP_Discover_Linux::Aging, this);
 	}
 }

 void UDP_Discover_Linux::stop() {
 	pthread_mutex_lock(&modeSynch);
 	mode = UDS_STOP;
 	pthread_mutex_unlock(&modeSynch);

 	pthread_mutex_lock(&vectorActiveUsersSynch);
 	activeUsers.clear();
 	pthread_mutex_unlock(&vectorActiveUsersSynch);

 }

 bool UDP_Discover_Linux::changeMode(int8_t newMode) {
 	if (newMode == UDS_STOP)
 		return false;

 	int8_t temp_mode;
 	pthread_mutex_lock(&modeSynch);
 	temp_mode = mode;
 	pthread_mutex_unlock(&modeSynch);
 	if (temp_mode == UDS_STOP)
 		return false;

 	pthread_mutex_lock(&modeSynch);
 	mode = newMode;
 	pthread_mutex_unlock(&modeSynch);

 	if (temp_mode == newMode)
 		return false;


 	if (temp_mode == UDS_ACTIVE && newMode == UDS_HIDDEN) {
 		//Do nothing. The advertise() will automatically close.
 		return true;
 	} else if (temp_mode == UDS_HIDDEN && newMode == UDS_ACTIVE) {
 		//start advertising
 		pthread_create(&threadsID[1], NULL, &UDP_Discover_Linux::Advertise, this);
 		return true;
 	} else {
 		return false;
 	}


 	return true;
 }

 vector<struct User> UDP_Discover_Linux::getActive() {
 	pthread_mutex_lock(&vectorActiveUsersSynch);
 	vector<struct User> temp(activeUsers);
 	pthread_mutex_unlock(&vectorActiveUsersSynch);
 	return temp;
 }

 #endif
