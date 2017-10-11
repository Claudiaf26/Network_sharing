#define _CRT_SECURE_NO_WARNINGS
#include "FileTransfer.h"
#include <iostream>
#include <string>
using namespace std;



int main() {
	for ( int i = 0; i < 1; ++i ) {
		string s;
		if(i%2 == 0 )
			s = "E:/testtest" ;
		else
			s = "E:TeamSpeak";
		std::chrono::time_point<std::chrono::system_clock> start, end;
		start = std::chrono::system_clock::now();
		FileTransfer ft( "192.168.1.198", s );
		cout << "Starting " << endl;
		cout << "Success? " << ft.transfer() << endl;
		end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout <<i  <<"elapsed time: " << elapsed_seconds.count() << "s\n";
		this_thread::sleep_for( std::chrono::seconds( 2 ) );
	}
	getchar();
	return 0;
}

