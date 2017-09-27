#define _CRT_SECURE_NO_WARNINGS
#include "FileTransfer.h"
#include <iostream>
#include <string>
using namespace std;



int main() {
	wstring s (L"C:/Users/Massimo/Desktop/root");
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	FileTransfer ft("192.168.1.198",s);
	cout << "Starting " << endl;
	cout<< "Success? "<<ft.transfer()<<endl;
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
	getchar();
	return 0;
}

