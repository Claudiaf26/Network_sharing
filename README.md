# File sharing
This C++ software lets people in the same network see their presence and share files and folders between them. 
It was developed as a project for the course "Programmazione di Sistema" (for Prof. Malnati). [Here](progetto2017.pdf) are the specifics in Italian.

## Implementation
The software uses QT to handle the GUI and boost::filesystem to handle file and directories, both were chosen as they are not OS dependant. The networking part is based on the C socket library, and it is wrapped using the Factory pattern to hide differences between Linux (sys/socket.h) and Windows (winsock.h).
