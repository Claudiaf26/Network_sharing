#ifndef UDP_DISCOVER_INTERFACE
#define UDP_DISCOVER_INTERFACE

#include <vector>
#include <iostream>
#include <string>
#include "../DataStruct/sharedsingleton.h"



class UDP_Discover_Interface {
protected:
	std::vector<struct User> activeUsers;
	int8_t mode;

public:
	/*run starts the proper threads based on the mode specified.*/
	virtual void run(int8_t mode) = 0;
	/*This second version of run overwrites the username specified in the constructor.*/
	virtual void run(int8_t mode, std::string newUserName, std::string picture)=0;

	/*stops the discovery and blocks all threads. Flushes the users buffers.*/
	virtual void stop() = 0;
	/*Switches between the active and hidden modes. Can't be used for starting or stopping the program.*/
	virtual bool changeMode(int8_t newMode) = 0;

	/*Returns users connected at the moment.*/
	virtual std::vector<struct User> getActive() = 0;

	UDP_Discover_Interface() {}; //The constructor will be overloaded in the derived class, it needs the username!!
	virtual ~UDP_Discover_Interface(){ }

};
#endif
