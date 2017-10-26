#ifdef __linux__
#include "contextmenu_linux.h"
#include <iostream>
#include <cstdio>
#include <exception>
#include <unistd.h>
#include <fstream>
#include <locale>
#include <codecvt>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>

inline int MIN(const int& a, const int& b){
    return (a < b) ? a : b;
}

ContextMenu_Linux::ContextMenu_Linux(wstring arg){
    m_contextString = arg;
}

bool ContextMenu_Linux::addToContextMenu(){
    //prende il percorso dell'applicazione
    char szTmp[32];
    char pBuf[1024];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = MIN(readlink(szTmp, pBuf, 1024), 1024 - 1);
    if(bytes >= 0)
        pBuf[bytes] = '\0';

    string contextPath = pBuf;
    contextPath.append(" %f");
    wstring_convert<codecvt_utf8_utf16<wchar_t>> convert;
    string contextStringA = convert.to_bytes(m_contextString);

    //prende l'home path
    const char* cHomeDirect;
    if ((cHomeDirect = getenv("HOME")) == NULL) {
        cHomeDirect = getpwuid(getuid())->pw_dir;
            }

    string homeDirect = cHomeDirect;
    m_completeRegistrationPath = homeDirect + m_registrationPath;

    //controlla se esiste il path di Nautilus
    DIR* dir = opendir(m_completeRegistrationPath.c_str());
        if (!dir)
            return false;
        else
            closedir(dir);
        m_completeRegistrationPath.append("custom.desktop");

    ifstream  src("custom.desktop");
    if(!src.good())
        return false;
    ofstream  dst(m_completeRegistrationPath, ios_base::out | ios_base::trunc);
    if(!dst.good())
        return false;

    string buffer;
    getline(src, buffer); dst << buffer << endl;
    getline(src, buffer); dst << buffer << endl;
    getline(src, buffer); buffer.append(contextStringA); dst << buffer << endl;
    getline(src, buffer); buffer.append(contextStringA); dst << buffer << endl;
    getline(src, buffer); dst << buffer << endl;
    getline(src, buffer); dst << buffer << endl;
    getline(src, buffer); dst << buffer << endl;
    getline(src, buffer); buffer.append(contextStringA); dst << buffer << endl;
    getline(src, buffer); buffer.append(contextPath); dst << buffer << endl;
    dst.flush();

    return true;
}

bool ContextMenu_Linux::removeFromContextMenu(){
    if (0 == remove(m_completeRegistrationPath.c_str()) )
        return true;
    else
        return false;
}

#endif
