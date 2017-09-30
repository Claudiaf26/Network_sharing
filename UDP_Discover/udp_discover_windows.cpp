#ifdef _WIN32
#include "udp_discover_windows.h"
#pragma comment(lib, "Iphlpapi.lib")

UDP_Discover_Windows::UDP_Discover_Windows(string userN, string pic) : UDP_Discover_Interface(){
	mode = UDS_STOP;
	userName = userN;
	picture = pic;

	//Initialize synch attributes
	InitializeCriticalSection( &vectorActiveUsersSynch );
	InitializeCriticalSection( &modeSynch );
	mode = UDS_STOP;

	defaultMessage.append( "UDPDISCOVERY" + userName + "\r\n" + picture + "\r\n" );
	cout << "Default message: " << defaultMessage;

	socket = unique_ptr<UDPSocketMulticast>( new UDPSocketMulticast( "239.0.0.100", DISCOVERY_PORT ) );

}

UDP_Discover_Windows::~UDP_Discover_Windows() {
	stop();
	WaitForMultipleObjects(3, threadsHandle, TRUE, INFINITE); //Threads will naturally terminate.
	
	CloseHandle(threadsHandle[0]);
	CloseHandle(threadsHandle[1]);
	CloseHandle(threadsHandle[2]);

	DeleteCriticalSection(&vectorActiveUsersSynch);
	DeleteCriticalSection(&modeSynch);
	
}

void UDP_Discover_Windows::advertise() {
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
		socket->sendPacket( defaultMessage );
		Sleep(ADVERTISE_SLEEP_TIME);
	}


}

void UDP_Discover_Windows::discover() {
	struct User  newUsr;
	string message;
	string senderIp;
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

		if(socket->receivePacket( message, senderIp ) <0)
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
		newUsr.ip.assign(senderIp);
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
				socket->sendPacket( defaultMessage );
			cout << "Found: " << newUsr.name << " " << newUsr.ip << " " << newUsr.age << endl;
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
				cout << "Aging: " << it->name << " " << it->ip << " " << it->age << endl;
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
