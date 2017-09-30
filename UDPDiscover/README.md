# UDP Discover
This class can be used both on Linux and on Windows to advertise the host's presence and find other hosts.


## Interface

### UDPDiscover(string userName, string picture)
Constructor.

userName is a string containing the name that the host will communicate to the others.

picture is a string containing the name of one of the possible pictures. Only the name will be sent, not the picture itself.

This method will initialize the connections, but won't start advertising or receiving until the run() method is called.
For any problem related to the connection, it will throw a std::runtime_error exception.

### 	~UDPDiscover();
Closes all the connections, and waits for the three private threads to gracefully terminate.

### void run(int8_t mode);
modes are defined in [define.h](../define.h).

UDS_STOP is not allowed in this method.

UDS_HIDDEN starts the service so that it will see other users, but won't advertise its presence.

UDS_ACTIVE stars the service so that it will both advertise its presence and see other users.

This method effectively starts the service by launching three private threads, one for advertising, one for listening, one for the aging.

### void run(int8_t mode, string newUserName, string picture);
This method works like run(int8_t mode), but it can be used if the user changes the name or the picture.

### void stop();
This method stops the service and waits for the threads to gracefully terminate. It will be called in the destructor.

### bool changeMode(int8_t newMode);
This method works like run, but can be used to change the mode in which the service is working between UDS_HIDDEN and UDS_ACTIVE.
If the argument is not valid, or an error happens, it will return false.

### vector<struct User> getActive();
Returns the list of active users.

## Singleton
Every user that connects or disconnects will be pushed in the singleton.
