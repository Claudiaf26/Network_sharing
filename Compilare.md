# Istruzioni per la compilazione del codice di networking

## Su Windows 32,64 bit:
Scaricare da qui i binari direttamente, sia 32 che 64 bit, la versione è quella di VS2015, mettili dove vuoi
https://sourceforge.net/projects/boost/files/boost-binaries/1.64.0/

Su visual studio, proprietà del progetto, per tutte le modalità (Debug e Release, 64 e 32 bit in base a per cosa compili linki quella giusta) C/C++, Additional include directories $PATH$boost_1_64_0\
Linker: Additional library directories $PATH$\boost_1_64_0\lib64-mscv... (o lib32)

## Su Linux 32, 64 bit:
https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz
Scarica, spacchetta, non cancellare lo zip.

Vai nella cartella spacchettata, fai
``` ./bootstrap.sh
   ./b2 install 
```
e copia tutta la cartella boost in /usr/lib
spacchetta di nuovo(cancella quella vecchia che tanto hai già copiato), compila con 
```
./bootstrap.sh 
./b2  cflags=-m32   cxxflags=-m32     address-model=32     threading=multi architecture=x86    instruction-set=i686   stage
```

copia la cartella per intero in usr/lib32
Compila la 64 bit con questo comando, elencando tutti i file (c'è un modo di scriptarlo automaticamente, ma non lo conosco) 
```
g++ -std=c++11 -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive FileTransfer.cpp Source.cpp TCPServerSocket.cpp TCPServerSocket_Linux.cpp TCPServerSocket_Windows.cpp TCPSocket.cpp TCPSocket_Linux.cpp TCPSocket_Windows.cpp -static -lboost_filesystem -lboost_system -o FileTransfer
```

Compila la 32 con questo comando, elencando tutti i file
```
g++ -m32 -std=c++11 -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive FileTransfer.cpp Source.cpp TCPServerSocket.cpp TCPServerSocket_Linux.cpp TCPServerSocket_Windows.cpp TCPSocket.cpp TCPSocket_Linux.cpp TCPSocket_Windows.cpp -static /usr/lib32/boost_1_64_0/bin.v2/libs/filesystem/build/gcc-5.3.1/release/address-model-32/architecture-x86/instruction-set-i686/link-static/threading-multi/libboost_filesystem.a /usr/lib32/boost_1_64_0/bin.v2/libs/system/build/gcc-5.3.1/release/address-model-32/architecture-x86/instruction-set-i686/link-static/threading-multi/libboost_system.a -o FileTransfer32
```
