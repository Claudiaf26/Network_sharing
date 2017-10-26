#ifdef __linux__
#ifndef SHAREDMEM_LINUX_H
#define SHAREDMEM_LINUX_H

#include "SharedMemory/sharedmem_interface.h"
#include <semaphore.h>
#include <time.h>

class SharedMem_Linux : public SharedMem_Interface {
private:
    int m_sharedMemoryID;
    key_t m_uniqueKey;
    wchar_t* m_sharedMemoryWString;
    sem_t* m_semaphore;
    struct timespec m_waitTime;
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
