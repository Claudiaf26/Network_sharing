#ifdef __linux__
#ifndef SHAREDMEM_LINUX_H
#define SHAREDMEM_LINUX_H

#include "SharedMemory/sharedmem_interface.h"
#include <semaphore.h>

class SharedMem_Linux : public SharedMem_Interface {
private:
    int sharedMemID;
    key_t key;
    wchar_t* pMem;
    sem_t* sem;
public:
    SharedMem_Linux();
    virtual bool createMem();
    virtual void releaseMem();
    virtual bool openMem();
    virtual std::wstring getContent();
    virtual void setContent(std::wstring);
};

#endif // SHAREDMEM_LINUX_H
#endif
