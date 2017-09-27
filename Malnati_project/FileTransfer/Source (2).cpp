#include "FileReceiver.h"
#include <iostream>
#include <string>
using namespace std;


int main() {
	string s("F:/Pippo");
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	TCPServerSocket ts(50000);
	TCPSocket so = ts.Accept();
	FileReceiver fr(std::move(so), s);
	
	cout << "Success? " << fr.receive() << endl;
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t( end );

	std::cout << "finished computation at " << std::ctime( &end_time )
		<< "elapsed time: " << elapsed_seconds.count() << "s\n";
	getchar();
	return 0;
}
