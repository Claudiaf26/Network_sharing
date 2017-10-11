#include "FileReceiver.h"
#include <iostream>
#include <string>
using namespace std;


int main() {

		wstring s( L"E:/Pippo" );
		try {
		TCPServerSocket ts( 50000 );
		while ( true ) {
			cout << "Inizio... " << endl;

			TCPSocket so = ts.Accept();
			std::chrono::time_point<std::chrono::system_clock> start, end;
			start = std::chrono::system_clock::now();
			cout << "getPeerIp(): " << so.getPeerIp() << endl;
			FileReceiver fr( std::move( so ), s );
			thread t( &FileReceiver::receive, &fr );

			uint8_t status;
			do {
				status = fr.getStatus();
				cout << "Progress: " << unsigned( fr.getStatus() ) << endl;
				this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
			} while ( status != FT_COMPLETE && status != FT_ERROR );

			string name;
			uint8_t type;
			fr.getFileDetails( name, type );
			cout << "getFileDetails(): " << name <<" " <<unsigned(type) <<" Success? " <<unsigned(fr.getStatus()) << endl;
			t.join();
			
			end = std::chrono::system_clock::now();

			std::chrono::duration<double> elapsed_seconds = end - start;
			std::time_t end_time = std::chrono::system_clock::to_time_t( end );

			std::cout << "elapsed time: " << elapsed_seconds.count() << "s" << endl;
		}
		} catch ( std::exception e ) {
			cout << "Eccezione nel main " <<e.what() <<endl;
		}
		cout << "\n\n";
		getchar();
	
	return 0;
}
