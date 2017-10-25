#ifdef _WIN32
#include "sharedmem_windows.h"
#include <cstdlib>

#define MEM_SIZE 1024
#define WAIT_MS 10000

using namespace std;

SharedMem_Windows::SharedMem_Windows() {
    m_size = MEM_SIZE;
}

bool SharedMem_Windows::createMem(){
    m_mutexHandle = CreateMutexW(NULL, FALSE, m_mutexName.c_str());
    if (m_mutexHandle == NULL)
        return false;

    DWORD waitResult = WaitForSingleObject(m_mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        CloseHandle(m_mutexHandle);
        return false;
    }

    m_sharedHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_size, m_sharedName.c_str());
    if((m_sharedHandle == NULL) || (GetLastError() == ERROR_ALREADY_EXISTS)){
        ReleaseMutex(m_mutexHandle);
        CloseHandle(m_mutexHandle);
        return false;
    }

    ReleaseMutex(m_mutexHandle);
    return true;
}

bool SharedMem_Windows::openMem(){
    m_mutexHandle = CreateMutexW(NULL, FALSE, m_mutexName.c_str());
    if (m_mutexHandle == NULL)
        return false;

    DWORD waitResult = WaitForSingleObject(m_mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        return false;
    }

    m_sharedHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, TRUE, m_sharedName.c_str());
    if(m_sharedHandle == NULL){
        ReleaseMutex(m_mutexHandle);
        return false;
    }

    ReleaseMutex(m_mutexHandle);

    return true;
}

void SharedMem_Windows::releaseMem(){
    CloseHandle(m_sharedHandle);
    CloseHandle(m_mutexHandle);
}

wstring SharedMem_Windows::getContent(){
    DWORD waitResult = WaitForSingleObject(m_mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        return L"";
    }

    wstring content;

    m_sharedMemoryWString = (LPTSTR) MapViewOfFile(m_sharedHandle, FILE_MAP_ALL_ACCESS, 0, 0, m_size);
    if (m_sharedMemoryWString == NULL){
        ReleaseMutex(m_mutexHandle);
        return L"";
    }

    content = m_sharedMemoryWString;

    UnmapViewOfFile(m_sharedMemoryWString);
    ReleaseMutex(m_mutexHandle);

    return content;
}

void SharedMem_Windows::setContent(wstring newContent){
    DWORD waitResult = WaitForSingleObject(m_mutexHandle, WAIT_MS);
    if (waitResult != WAIT_OBJECT_0){
        return;
    }

    m_sharedMemoryWString = (LPTSTR) MapViewOfFile(m_sharedHandle, FILE_MAP_ALL_ACCESS, 0, 0, m_size);
    if (m_sharedMemoryWString == NULL){
        ReleaseMutex(m_mutexHandle);
        return;
    }

    CopyMemory((PVOID)m_sharedMemoryWString, newContent.c_str(), m_size);

    UnmapViewOfFile(m_sharedMemoryWString);
    ReleaseMutex(m_mutexHandle);
}


#endif
