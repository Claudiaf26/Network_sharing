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

using namespace std;
using namespace boost::filesystem;

class FileReceiver {
private:
	TCPSocket controlS;

	path dest;
	vector<uint16_t> ports;

	atomic<bool> success;

	atomic<uint64_t> overallSize;
	atomic<uint64_t> overallSent;

	vector<TCPServerSocket> fileServerSockets;
	vector<TCPSocket> fileSockets;
	vector<thread> transferThreads;

	/*Based on the hardware concurrency, trades the number of ports to be used.
	*It receives a proposal with a certain number, 
	*and accepts it or if it can't hold them, it will send
	*a counter-proposal that will be accepted.*/
	void tradeport();
	void prepareSockets();

	/*The method that will actually evaluate each packet, if it is a directory, the overall size
	*or the actual file. */
	void threadReceive(uint16_t i);

public:
	/*TCPSocket is a connected Socket, obtained from a TCPServerSocket.accept().
	 *The TCPServerSocket.accept() will be always listening for incoming control 
	 *connection. Destination is a local folder in which files will be received. 
	 *It accepts a string or a wstring. Pass it a wstring. If the path does not
	 *exists, it will be created.*/
	FileReceiver(TCPSocket s, path destination);
	~FileReceiver();

	/*Stop the transfer from both the client and the server in a thread-safe fashion.*/
	void stop();

	/*Returns an int with the percentage of the total size sent.*/
	uint8_t getProgress();

	/*To be called to effectively start the transfer. It is important that after this call
	 *the object does not go out of scope.*/
	bool receive();
	

};

#endif
