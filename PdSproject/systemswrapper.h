#ifndef SYSTEMSWRAPPER_H
#define SYSTEMSWRAPPER_H

#define ULEN 10

#ifdef _WIN32
#pragma comment(lib,"Advapi32.lib")
#include <windows.h>
#include <shlobj.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <pwd.h>
#endif

#include <string>
#include <iostream>

class SystemsWrapper{
public:
    static string getSystemUsername(){
        std::string curUser;

#ifdef _WIN32
        DWORD len = UNLEN+1;
        WCHAR winUser[UNLEN+1];
        if(GetUserNameW(winUser, &len)){
            int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, winUser, -1, NULL, 0, NULL, NULL);
            char* encodedStr = new char[sizeNeeded];
            WideCharToMultiByte(CP_UTF8, 0, winUser, -1, encodedStr, sizeNeeded, NULL, NULL);
            curUser = encodedStr;
            delete encodedStr;
        }
        else
            curUser = "Default";
#endif

#ifdef __linux__
        char username[UNLEN+1];
        size_t username_len = UNLEN+1;
        getlogin_r(username, username_len);
        curUser = username;
#endif
        return curUser;
    }

    static std::string getSystemDownloadFolder(){
        std::string directory;

#ifdef _WIN32
        WCHAR direct[UNLEN+1];
        PWSTR winDirect = direct;
        if(!SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DONT_UNEXPAND, NULL, &winDirect)){
            int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, winDirect, -1, NULL, 0, NULL, NULL);
            char* encodedStr = new char[sizeNeeded];
            WideCharToMultiByte(CP_UTF8, 0, winDirect, -1, encodedStr, sizeNeeded, NULL, NULL);
            directory = encodedStr;
        }
        else
            directory = "C:";
#endif

#ifdef __linux__
        const char* linuxDirect;

        if ((linuxDirect = getenv("HOME")) == NULL) {
            linuxDirect = getpwuid(getuid())->pw_dir;
        }

        directory = linuxDirect;
        //prova a raggiungere la cartella di download
#endif
        return directory;
    }

};

#endif // SYSTEMSWRAPPER_H

