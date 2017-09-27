#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include <iostream>

//#define SERVER
#define CLIENT
using namespace std;

int main() {
#ifdef CLIENT
	/*Test the client*/
	TCPSocket s(string("127.0.0.1"), 50001);

	vector<char> v;
	for (uint32_t i = 0; i < 50000; i++) {
		v.push_back(i % 120);
	}

	cout << "Vector complete." << endl;

	cout <<"Send successfull? " << s.Send(v) <<endl;

	TCPSocket s1=move(s);

	cout << "Movement done. " << endl;

	struct timeval t; t.tv_sec = 1; t.tv_usec = 0;

	vector<char> v2;
	cout << "Receive successful? " <<s1.Receive(v2, v.size(), t)<<endl;
	
	cout << "Are they equal? " << (v == v2) << endl;

	/*End client testing*/
#endif

#ifdef SERVER
	/*Test the server*/
	TCPServerSocket tsw(50001);

	vector<char> v1;
	for (uint32_t i = 0; i <50000; i++) {
		v1.push_back(i % 120);
	}
	cout << "Vector complete." << endl;

	TCPSocket cs = tsw.Accept();
	cout << "Connection established.";



	vector<char> v;
	struct timeval t; t.tv_sec = 1; t.tv_usec = 0;
	cout << "Receive successful? " << cs.Receive(v, v1.size(), t) << endl;

	cout << "Are they equal? " << (v == v1)<<endl;


	cout << "Send successfull? " << cs.Send(v) << endl;

	/*End server testing*/

#endif	

	getchar();
	return 0;
}
