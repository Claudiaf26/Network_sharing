#ifdef __linux__
#include "sharedmem_linux.h"
#include <cwchar>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <string.h>
#include <iostream>

#define PROJ_ID 5891
#define MEM_SIZE 1024

using namespace std;

inline int MIN(const int& a, const int& b){
    return (a < b) ? a : b;
}

SharedMem_Linux::SharedMem_Linux(){
    m_waitTime->tv_sec = 2;
    m_waitTime.tv_nsec = 1000;

    //prende il percorso del programma in esecuzione per creare una chiave unica
    char szTmp[32];
    char pBuf[1024];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = MIN(readlink(szTmp, pBuf, 1024), 1024 - 1);
    if(bytes >= 0)
        pBuf[bytes] = '\0';

    m_uniqueKey = ftok(pBuf, PROJ_ID);
}

bool SharedMem_Linux::createMem(){
    m_semaphore = sem_open(to_string(m_uniqueKey).c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (m_semaphore == SEM_FAILED)
        return false;

    if (-1 == sem_timedwait(m_semaphore, &m_waitTime))
        return false;

    m_sharedMemoryID = shmget(m_uniqueKey, MEM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (m_sharedMemoryID < 0){
        sem_post(m_semaphore);
        return false;
    }

    sem_post(m_semaphore);

    return true;
}

bool SharedMem_Linux::openMem(){
    m_semaphore = sem_open(to_string(m_uniqueKey).c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (m_semaphore == SEM_FAILED)
        return false;

    if (-1 == sem_timedwait(m_semaphore, &m_waitTime))
        return false;

    m_sharedMemoryID = shmget(m_uniqueKey, MEM_SIZE, IPC_CREAT | 0666);
    if (m_sharedMemoryID < 0){
        sem_post(m_semaphore);
        return false;
    }

    sem_post(m_semaphore);

    return true;
}

void SharedMem_Linux::releaseMem(){
    struct shmid_ds shmid_ds;

    sem_close(m_semaphore);
    shmctl(m_sharedMemoryID, IPC_RMID, &shmid_ds);
}

wstring SharedMem_Linux::getContent(){
    if (-1 == sem_timedwait(m_semaphore, &m_waitTime))
        throw runtime_error("Semaphore opening was impossible");

    m_sharedMemoryWString = (wchar_t*)shmat(m_sharedMemoryID, NULL, 0);
    if (m_sharedMemoryWString == reinterpret_cast<wchar_t*>(-1) ){
        sem_post(m_semaphore);
        throw runtime_error("File mapping was impossible");
    }

    wstring content = m_sharedMemoryWString;

    shmdt(m_sharedMemoryWString);
    sem_post(m_semaphore);

    return content;
}

void SharedMem_Linux::setContent(wstring t_newContent){
    if (-1 == sem_timedwait(m_semaphore, &m_waitTime))
        throw runtime_error("Semaphore opening was impossible");

    m_sharedMemoryWString = (wchar_t*)shmat(m_sharedMemoryID, NULL, 0);
    if (m_sharedMemoryWString == reinterpret_cast<wchar_t*>(-1) ){
        sem_post(m_semaphore);
        throw runtime_error("File mapping was impossible");
    }

    wcscpy(m_sharedMemoryWString, t_newContent.c_str() );

    shmdt(m_sharedMemoryWString);
    sem_post(m_semaphore);
}

#endif
