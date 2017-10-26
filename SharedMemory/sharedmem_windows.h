#ifdef _WIN32
#ifndef SHAREDMEM_WINDOWS_H
#define SHAREDMEM_WINDOWS_H

#include "SharedMemory/sharedmem_interface.h"
#include <Windows.h>

//questa versione non gestisce il caso in cui si sfori la dimensione
class SharedMem_Windows : public SharedMem_Interface {
private:
    HANDLE m_sharedHandle;
    HANDLE m_mutexHandle;
    LPTSTR m_sharedMemoryWString;

    const std::wstring m_mutexName = L"Global\\FileSharingMutex";
    const std::wstring m_sharedName = L"Global\\FileSharingProject";

public:
    SharedMem_Windows();
    virtual bool createMem();                   //crea un'area di memoria condivisa
    virtual void releaseMem();                  //distrugge l'area di memoria condivisa
    virtual bool openMem();                     //apre l'area di memoria condivisa
    virtual std::wstring getContent();          //ottiene la lista degli utenti serializzata dall'area di memoria condivisa
    virtual void setContent(std::wstring);      //scrive la lista degli utenti serializzata sull'area di memoria condivisa
};

#endif // SHAREDMEM_WINDOWS_H
#endif
