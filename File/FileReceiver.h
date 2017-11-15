#ifndef FILERECEIVER
#define FILERECEIVER

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/endian/arithmetic.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <future>
#include <mutex>
#include "TCPSocket/TCPSocket.h"
#include "TCPServerSocket/TCPServerSocket.h"
#include "define.h"
//#include "TCPSocket.h"
//#include "TCPServerSocket.h"
//#include "../../define.h"


using namespace std;

class FileReceiver {
private:
	TCPSocket controlS;

	boost::filesystem::path dest;

	vector<uint16_t> ports;

	atomic<bool> success;

	atomic<uint64_t> overallSize;
	atomic<uint64_t> overallSent;

	atomic<std::chrono::time_point<std::chrono::system_clock>> transferStart;

	vector<TCPServerSocket> fileServerSockets;
	vector<TCPSocket> fileSockets;
	vector<thread> transferThreads;

	mutex transferDetailsMutex;
    string transferName;
	uint8_t transferType;

	/*Receives the transfer request containing the file/folder name and type*/
	void receiveTransferRequest();

	/*Based on the hardware concurrency, trades the number of ports to be used.
	*It receives a proposal with a certain number, 
	*and accepts it or if it can't hold them, it will send
	*a counter-proposal that will be accepted.*/
	void tradeport();

	/*For each port, it opens a socket.*/
	void prepareSockets();

	/*The method that will actually reiceive the files*/
	void threadReceive(uint16_t i);

public:
	/*TCPSocket is a connected Socket, obtained from a TCPServerSocket.accept().
	 *The TCPServerSocket.accept() will be always listening for incoming control 
	 *connection. Destination is a local folder in which files will be received. 
	 *It accepts a string. If the path does not
	 *exists, it will be created.*/
	FileReceiver(TCPSocket s, boost::filesystem::path destination);
	~FileReceiver();

	/*Stop the transfer from both the client and the server in a thread-safe fashion.*/
	void stop();

	/*To be called to effectively start the transfer.*/
	bool receive();

	/*Returns the progress (0-99), the FT_COMPLETE or the FT_ERROR defined in define.h*/
	/*Returns the transfer speed in MBps and the time left in seconds*/
	void getStatus(transferStatus& t);

	/*Sets the name of the file/directory transferred and the type, either FT_FILE or FT_DIRECTORY defined in define.h*/
	void getFileDetails(string& name, uint8_t& type);

};

#endif
