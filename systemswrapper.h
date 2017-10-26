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
#include <sys/types.h>
#include <dirent.h>
#endif

#include <string>
#include <iostream>

//classe con funzioni statiche che gestisce un paio di funzionalità di sistema diverse in base all'SO
class SystemsWrapper{
public:
    //ottiene l'username dell'utente
    static string getSystemUsername(){
        std::string curUser;

#ifdef _WIN32
        DWORD len = ULEN+1;
        WCHAR winUser[ULEN+1];
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
        char username[ULEN+1];
        size_t username_len = ULEN+1;
        if (0 == getlogin_r(username, username_len))
                curUser = username;
        else
            curUser = "Default";
#endif
        return curUser;
    }

    //ottiene la cartella di download di default dell'utente
    static std::string getSystemDownloadFolder(){
        std::string directory;

#ifdef _WIN32
        WCHAR direct[ULEN+1];
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

        //controlla se esiste la cartella download, se no imposta come cartella di default la home
        std::string directoryDownload = directory;
        directoryDownload.append("/Download");
        DIR* dir = opendir(directoryDownload.c_str());
        if (dir){
            closedir(dir);
            directory = directoryDownload;
        }
#endif
        return directory;
    }
};

#endif // SYSTEMSWRAPPER_H

