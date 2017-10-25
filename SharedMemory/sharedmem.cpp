#include "sharedmem.h"

using namespace std;

SharedMem::SharedMem(){
#ifdef _WIN32
    m_base = make_unique<SharedMem_Windows>();
#endif
/*#ifdef __linux__
    base = unique_ptr<SharedMem_Linux>(new SharedMem_Linux());
#endif*/
}

bool SharedMem::createMem(){
    return m_base->createMem();
}

bool SharedMem::openMem(){
    return m_base->openMem();
}

void SharedMem::releaseMem(){
    m_base->releaseMem();
}

void SharedMem::setContent(wstring newContent){
    if(openMem()){
        m_base->setContent(newContent);
    }
}

wstring SharedMem::getContent(){
    if(openMem()){
       return m_base->getContent();
    }
    else
        return L"ERROR";
}
