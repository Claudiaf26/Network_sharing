#ifdef _WIN32
#include "sharedmem_windows.h"
#include <cstdlib>

#define MEM_SIZE 1024
#define WAIT_MS 10000

using namespace std;

SharedMem_Windows::SharedMem_Windows() {
    size = MEM_SIZE;
}

bool SharedMem_Windows::createMem(){
    mutexHandle = CreateMutexW(NULL, FALSE, mutexName.c_str());
    if (mutexHandle == NULL)
        return false;

    DWORD waitResult = WaitForSingleObject(mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        CloseHandle(mutexHandle);
        return false;
    }

    sharedHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name.c_str());
    if((sharedHandle == NULL) || (GetLastError() == ERROR_ALREADY_EXISTS)){
        ReleaseMutex(mutexHandle);
        CloseHandle(mutexHandle);
        return false;
    }

    ReleaseMutex(mutexHandle);
    return true;
}

bool SharedMem_Windows::openMem(){
    mutexHandle = CreateMutexW(NULL, FALSE, mutexName.c_str());
    if (mutexHandle == NULL)
        return false;

    DWORD waitResult = WaitForSingleObject(mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        return false;
    }

    sharedHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, TRUE, name.c_str());
    if(sharedHandle == NULL){
        ReleaseMutex(mutexHandle);
        return false;
    }

    ReleaseMutex(mutexHandle);

    return true;
}

void SharedMem_Windows::releaseMem(){
    CloseHandle(sharedHandle);
    CloseHandle(mutexHandle);
}

wstring SharedMem_Windows::getContent(){
    DWORD waitResult = WaitForSingleObject(mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        return L"";
    }

    wstring content;

    pMem = (LPTSTR) MapViewOfFile(sharedHandle, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (pMem == NULL){
        ReleaseMutex(mutexHandle);
        return L"";
    }

    content = pMem;

    UnmapViewOfFile(pMem);
    ReleaseMutex(mutexHandle);

    return content;
}

void SharedMem_Windows::setContent(wstring newContent){
    DWORD waitResult = WaitForSingleObject(mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        return;
    }

    pMem = (LPTSTR) MapViewOfFile(sharedHandle, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (pMem == NULL){
        ReleaseMutex(mutexHandle);
        return;
    }

    CopyMemory((PVOID)pMem, newContent.c_str(), size);

    UnmapViewOfFile(pMem);
    ReleaseMutex(mutexHandle);
}


#endif
