#ifndef USERLIST_H
#define USERLIST_H

#ifdef _WIN32
#include "SharedMemory/sharedmem_windows.h"
#endif

#ifdef __linux__
#include "SharedMemory/sharedmem_linux.h"
#endif

#include <memory>
#include <string>

//gestisce la creazione di memoria condivisa su windows e linux mediante polimorfismo
class SharedMem {
private:
    std::unique_ptr<SharedMem_Interface> m_base;

public:
    SharedMem();
    bool createMem();               //crea un'area di memoria condivisa
    bool openMem();                 //apre l'area di memoria condivisa
    void releaseMem();              //distrugge l'area di memoria condivisa
    std::wstring getContent();      //ottiene la lista degli utenti serializzata dall'area di memoria condivisa
    void setContent(std::wstring);  //scrive la lista degli utenti serializzata sull'area di memoria condivisa
};

#endif // USERLIST_H  
