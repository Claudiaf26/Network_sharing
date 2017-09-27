#ifndef SHAREDMEM_INTERFACE_H
#define SHAREDMEM_INTERFACE_H

#include <string>

class SharedMem_Interface {
protected:
    uint32_t size;
public:
    virtual bool createMem() = 0;
    virtual bool openMem() = 0;
    virtual void releaseMem() = 0;
    virtual std::wstring getContent() = 0;
    virtual void setContent(std::wstring) = 0;
};

#endif // SHAREDMEM_INTERFACE_H
