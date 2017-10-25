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
    virtual bool createMem();
    virtual void releaseMem();
    virtual bool openMem();
    virtual std::wstring getContent();
    virtual void setContent(std::wstring);
};

#endif // SHAREDMEM_WINDOWS_H
#endif
