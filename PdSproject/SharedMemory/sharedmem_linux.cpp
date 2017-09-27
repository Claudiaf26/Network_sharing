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

inline int MIN(int a, int b){
    return (a < b) ? a : b;
}

SharedMem_Linux::SharedMem_Linux(){
    //prende l'exe path
    char szTmp[32];
    char pBuf[1024];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = MIN(readlink(szTmp, pBuf, 1024), 1024 - 1);
    if(bytes >= 0)
        pBuf[bytes] = '\0';

    key = ftok(pBuf, PROJ_ID);
}

bool SharedMem_Linux::createMem(){
    sem = sem_open(to_string(key).c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (sem == SEM_FAILED)
        return false;

    sem_wait(sem);

    sharedMemID = shmget(key, MEM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (sharedMemID < 0){
        sem_post(sem);
        return false;
    }

    sem_post(sem);

    return true;
}

bool SharedMem_Linux::openMem(){
    sem = sem_open(to_string(key).c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (sem == SEM_FAILED)
        return false;

    sem_wait(sem);

    sharedMemID = shmget(key, MEM_SIZE, IPC_CREAT | 0666);
    if (sharedMemID < 0){
        sem_post(sem);
        return false;
    }

    sem_post(sem);

    return true;
}

void SharedMem_Linux::releaseMem(){
    struct shmid_ds shmid_ds;

    sem_close(sem);
    shmctl(sharedMemID, IPC_RMID, &shmid_ds);
}

wstring SharedMem_Linux::getContent(){
    sem_wait(sem);
    pMem = (wchar_t*)shmat(sharedMemID, NULL, 0);
    if (pMem == (wchar_t*)(-1) ){
        sem_post(sem);
        return L"";
    }

    wstring content = pMem;

    shmdt(pMem);
    sem_post(sem);

    return content;
}

void SharedMem_Linux::setContent(wstring newContent){
    sem_wait(sem);
    pMem = (wchar_t*)shmat(sharedMemID, NULL, 0);
    if (pMem == (wchar_t*)(-1) ){
        char* error = strerror(errno);
        cout << error << endl;
        sem_post(sem);
        return;
    }

    wcscpy(pMem, newContent.c_str() );

    shmdt(pMem);
    sem_post(sem);
}

#endif
