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

class SharedMem {
private:
    std::unique_ptr<SharedMem_Interface> m_base;

public:
    SharedMem();
    bool createMem();
    bool openMem();
    void releaseMem();
    std::wstring getContent();
    void setContent(std::wstring);
};

#endif // USERLIST_H
