#ifdef _WIN32
#include "UDPDiscover.h"
#pragma comment(lib, "Iphlpapi.lib")

UDPDiscover::UDPDiscover( string userN, string pic ){
	mode = UDS_STOP;
	userName = userN;
	picture = pic;

	//Initialize synch attributes
	defaultMessage.append( "UDPDISCOVERY" + userName + "\r\n" + picture + "\r\n" );
	cout << "Default message: " << defaultMessage;

	socket = unique_ptr<UDPSocketMulticast>( new UDPSocketMulticast( "239.0.0.100", DISCOVERY_PORT ) );

}

UDPDiscover::~UDPDiscover() {
	stop();

	//Threads will naturally terminate.
	for ( int i = 0; i < 3; ++i )
		if ( threads[i].joinable() )
			threads[i].join();
}

void UDPDiscover::advertise() {
	int8_t temp_mode;

	/*PROTOCOL:
	1. char[12] = "UDPDISCOVERY";
	2. string name; null terminated string, at most 255 characters.
	3. \r\n
	4. picture name, at most 255 characters
	. \r\n
	*/

	while ( true ) {
		temp_mode = mode.load();

		if ( temp_mode == UDS_STOP || temp_mode == UDS_HIDDEN ) {
			return;
		}
		socket->sendPacket( defaultMessage );
		Sleep( ADVERTISE_SLEEP_TIME );
	}

}

void UDPDiscover::discover() {
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

	while ( true ) {
		temp_mode = mode.load();

		if ( temp_mode == UDS_STOP ) {
			return;
		}

		if ( socket->receivePacket( message, senderIp ) < 0 )
			continue;

		/*Packet translation. If the protocol is not respected, just ignore.*/
		pos1 = message.find( "UDPDISCOVERY" ) + strlen( "UDPDISCOVERY" );
		pos2 = message.find( "\r\n" );
		if ( pos1 == string::npos || pos2 == string::npos )
			continue;
		newUsr.name.assign( message.substr( pos1, pos2 - pos1 ) );
		pos1 = pos2 + strlen( "\r\n" );
		pos2 = message.find( "\r\n", pos1 );
		if ( pos1 == string::npos || pos2 == string::npos )
			continue;
		newUsr.picture.assign( message.substr( pos1, pos2 - pos1 ) );
		newUsr.ip.assign( senderIp );
		pos2 = newUsr.ip.find( ":" );
		newUsr.ip.assign( newUsr.ip.substr( 0, pos2 ) );
		newUsr.age = 3;

		vectorActiveUsersSynch.lock();
		auto ele = find( activeUsers.begin(), activeUsers.end(), newUsr );
		if ( ele != activeUsers.end() ) {
			//The host is still active, refresh it
			(*ele).age = 3;
		} else {
			/*The host has just connected.
			1. Add it to the list of users
			2. Reply with a discovery packet to reduce the delay for cache aligning.
				ACHTUNG! Only if the host is visible!*/
			activeUsers.push_back( newUsr );
			UserListSingleton::get_instance().pushNew( newUsr );
			if ( temp_mode == UDS_ACTIVE )
				socket->sendPacket( defaultMessage );
			cout << "Found: " << newUsr.name << " " << newUsr.ip << " " << newUsr.age << endl;
		}
		vectorActiveUsersSynch.unlock();

	}

}

void UDPDiscover::aging() {
	int8_t temp_mode;
	while ( true ) {
		temp_mode = mode.load();

		if ( temp_mode == UDS_STOP ) {
			return;
		}

		vectorActiveUsersSynch.lock();
		for ( auto it = activeUsers.begin(); it != activeUsers.end();) {
			if ( (*it).age >= 0 ) {
				(*it).age--;
				cout << "Aging: " << it->name << " " << it->ip << " " << it->age << endl;
				++it;
			} else {
				UserListSingleton::get_instance().pushDeleted( *it );
				it = activeUsers.erase( it );
			}
		}
		vectorActiveUsersSynch.unlock();
		Sleep( AGING_SLEEP_TIME );
	}
}


void UDPDiscover::run( int8_t  initialMode ) {
	if ( initialMode == UDS_STOP || mode != UDS_STOP )
		return;

	mode = initialMode;

	if ( initialMode == UDS_ACTIVE ) {
		threads[0] = thread( &UDPDiscover::discover, this );
		threads[1] = thread( &UDPDiscover::advertise, this );
		threads[2] = thread( &UDPDiscover::aging, this );

	} else if ( initialMode == UDS_HIDDEN ) {
		threads[0] = thread( &UDPDiscover::discover, this );
		threads[2] = thread( &UDPDiscover::aging, this );
	}

}

void UDPDiscover::run( int8_t  initialMode, string newUserName, string picture ) {
	if ( initialMode == UDS_STOP || mode != UDS_STOP )
		return;

	mode = initialMode;
	userName.assign( newUserName );
	picture.assign( picture );

	if ( initialMode == UDS_ACTIVE ) {
		threads[0] = thread( &UDPDiscover::discover, this );
		threads[1] = thread( &UDPDiscover::advertise, this );
		threads[2] = thread( &UDPDiscover::aging, this );
	} else if ( initialMode == UDS_HIDDEN ) {
		threads[0] = thread( &UDPDiscover::discover, this );
		threads[2] = thread( &UDPDiscover::aging, this );

	}
}



void UDPDiscover::stop() {
	mode.store( UDS_STOP );

	vectorActiveUsersSynch.lock();
	activeUsers.clear();
	vectorActiveUsersSynch.unlock();

	socket->closeSocket();

	for ( int i = 0; i < 3; ++i ) {
		//Each thread will naturally die, don't care of the result.
		threads[i].join();
	}

}
bool UDPDiscover::changeMode( int8_t newMode ) {
	if ( newMode == UDS_STOP )
		return false;

	int8_t temp_mode;
	temp_mode = mode.load();

	if ( temp_mode == UDS_STOP )
		return false;

	mode.store( newMode );

	if ( temp_mode == newMode )
		return false;


	if ( temp_mode == UDS_ACTIVE && newMode == UDS_HIDDEN ) {
		//The advertise() will automatically close.
		if ( threads[1].joinable() )
			threads[1].join();
		return true;
	} else if ( temp_mode == UDS_HIDDEN && newMode == UDS_ACTIVE ) {
		//start advertising
		threads[1] = thread( &UDPDiscover::advertise, this );
		return true;
	} else {
		return false;
	}

	return true;
}



vector<struct User> UDPDiscover::getActive() {
	vectorActiveUsersSynch.lock();
	vector<struct User> temp( activeUsers );
	vectorActiveUsersSynch.unlock();
	return temp;
}

#endif
