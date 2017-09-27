#include "sharedmem.h"

using namespace std;

SharedMem::SharedMem(){
#ifdef _WIN32
    base = make_unique<SharedMem_Windows>();
#endif
#ifdef __linux__
    base = unique_ptr<SharedMem_Linux>(new SharedMem_Linux());
#endif
}

bool SharedMem::createMem(){
    return base->createMem();
}

bool SharedMem::openMem(){
    return base->openMem();
}

void SharedMem::releaseMem(){
    base->releaseMem();
}

void SharedMem::setContent(wstring newContent){
    if(openMem()){
        base->setContent(newContent);
    }
}

wstring SharedMem::getContent(){
    if(openMem()){
       return base->getContent();
    }
    else
        return L"ERROR";
}
