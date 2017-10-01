#ifndef FILETRANSFER
#define FILETRANSFER

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/endian/arithmetic.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <string.h>
#include "TCPSocket.h"


using namespace std;
using namespace boost::filesystem;


class FileTransfer {
private:
	path source;
	string ip;
	vector<uint16_t> ports;

	atomic<bool> success;

	atomic<uint64_t> overallSize; 
	atomic<uint64_t> overallSent;
	vector<TCPSocket> fileSockets;

	mutex rdi_protection;
	recursive_directory_iterator rdi;
	recursive_directory_iterator end_rdi;

	vector<unique_ptr<thread>> transferThreads;

	/*Based on results from dirSize and exploreDir, calls the createDirectoryPacket to send the dir tree. */
	bool sendDir();
	/*To be called in a separate thread, sends a single file.
	 *If the transmission is one file only, it will send it. If it is a directory, 
	 *it will search for each file in the directory tree. */
	void sendFile(uint16_t i);

	/*Explore the dir to find the overall size, given by the sum of the file size.*/
	uint64_t dirSize();
	/*Explore the dir and all subdirectories and saves the path in a proper packet.*/
	vector<wstring> exploreDir();

	/*Creates the packet with all the directories according to the protocol. */
	vector<char> createDirectoryPacket(vector<wstring> tree);
	
	/*Based on the hardware concurrency, trades the number of ports to be used.
	 *It asks for a certain number, if the receiver can't hold them, it will send
	 *a counter-proposal that will be accepted.*/
	void tradePort();
	/*Both sockets will open a data connection for each traded port.*/
	void prepareSockets();

public: 
	/*ipAddress of an active host. If not active, an exception is thrown.
	 *source path of the file or of the directory. If it does not exist,
	 *an exception is thrown.*/
	FileTransfer( string ipAddress, path source );
	~FileTransfer();

	/*Returns the percentage sent.*/
	uint8_t getProgress();
	/*Start the transfer. Returns 0 if any error happens or the stop() method is invoked.*/
	bool transfer();
	/*Clear and thread-safe way to stop the transfer.*/
	void stop(); 



};


#endif
