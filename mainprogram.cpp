#include "mainprogram.h"
#include "systemswrapper.h"
#include "DataStruct/usersingleton.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

inline string toEnd(bool value){
    return value ? "1\n" : "0\n";
}
inline wstring StringToWString(const string& s){
 wstring temp(s.length(),' ');
 copy(s.begin(), s.end(), temp.begin());
 return temp;
 }

MainProgram::MainProgram(){
    context = nullptr;
    settingsUI = nullptr;
    notification = nullptr;
    udpDiscover = nullptr;
    receiver = nullptr;
    receiverThread = new QThread(this);

    if(!UserSingleton::get_instance().initialize())
        exit(EXIT_FAILURE);

    running = false;

    curr.username.clear();
    curr.directory.clear();
    curr.privateMode = false;
    curr.automaticMode = false;
    curr.notificationNoShowMode = false;

    context = new ContextMenu(L"FileSender");
    settingsUI = new Settings();
    notification = new NotificationManager(this);
    udpDiscover = new UDP_Manager();
    receiver = new ReceiverManager();
    receiver->moveToThread(receiverThread);

    QObject::connect(settingsUI, SIGNAL(startProgram(uint8_t,string,string)), this, SLOT(startProgram(uint8_t,string,string)) );
    QObject::connect(udpDiscover, SIGNAL(showSignal(QString)), notification, SLOT(showNotification(QString)) );
    QObject::connect(udpDiscover, SIGNAL(addUser(User)), this, SLOT(addUser(User)) );
    QObject::connect(udpDiscover, SIGNAL(deleteUser(User)), this, SLOT(deleteUser(User)) );
    QObject::connect(udpDiscover, SIGNAL(addUser(User)), settingsUI, SLOT(addUser(User)) );
    QObject::connect(udpDiscover, SIGNAL(deleteUser(User)), settingsUI, SLOT(deleteUser(User)) );
    QObject::connect(this, SIGNAL(changeSettings(uint8_t, string, string)), settingsUI, SLOT(changeSettings(uint8_t, string, string)) );
    QObject::connect(receiverThread, SIGNAL(started()), receiver, SLOT(loop()));
}

MainProgram::~MainProgram(){
    context->removeFromContextMenu();
    receiverThread->quit();
    receiverThread->wait();
    delete receiverThread;
    if (context != nullptr)
        delete context;
    if (settingsUI != nullptr)
        delete settingsUI;
    if (notification != nullptr)
        delete notification;
    if (udpDiscover != nullptr)
        delete udpDiscover;
    if (receiver != nullptr)
        delete receiver;
}

//cose da cambiare (potenzialmente): rendere const & oggetti passati a funzione, cambiare nomi, aggiungere sottofunzioni per aumentare leggibilità

void MainProgram::start(){
    //forse c'è da invertire l'ordine di queste due, va fatto meglio il controllo (se c'è un errore di formattazione?)
    settingsFile.open("settings.txt", ios_base::in );
    if(settingsFile.fail()){
        curr.username = SystemsWrapper::getSystemUsername();
        curr.directory = SystemsWrapper::getSystemDownloadFolder();
    }
    else{
        string userLine, directLine, automaticLine, notificationLine, privateLine;
        getline(settingsFile, userLine);
        getline(settingsFile, directLine);
        getline(settingsFile, automaticLine);
        getline(settingsFile, notificationLine);
        getline(settingsFile, privateLine);
        settingsFile.close();

        curr.username = userLine.substr(userLine.find("=")+1);
        curr.directory = directLine.substr(userLine.find("=")+2); //perché?
        char automaticChar = automaticLine[automaticLine.find("=")+1];
        char notificationChar = notificationLine[notificationLine.find("=")+1];
        char privateChar = privateLine[privateLine.find("=")+1];
        curr.automaticMode = (automaticChar == '1') ? true : false;
        curr.notificationNoShowMode = (notificationChar == '1') ? true : false;
        curr.privateMode = (privateChar == '1') ? true : false;
    }
    uint8_t flags = 0;
    if (curr.automaticMode)
        flags |= AUTOMATIC_FLAG;
    if (curr.notificationNoShowMode)
        flags |= NOTIFICATION_FLAG;
    if (curr.privateMode)
        flags |= PRIVATE_FLAG;
    changeSettings(flags, curr.username, curr.directory);

    settingsUI->show();
}

void MainProgram::startProgram(uint8_t flg, string user, string direct){
    curr.username = user;
    curr.directory = direct;
    curr.automaticMode = ((flg & AUTOMATIC_FLAG) != 0) ? true : false;
    curr.notificationNoShowMode = ((flg & NOTIFICATION_FLAG) != 0) ? true : false;
    curr.privateMode = ((flg & PRIVATE_FLAG) != 0) ? true : false;

    settingsFile.open("settings.txt", ios_base::out | ios_base::trunc);
    if (settingsFile.good()){
        string userLine("username=" +curr.username+'\n'),
               directLine("directory=" +curr.directory+'\n'),
               automaticLine("automaticreception="+toEnd(curr.automaticMode)),
               notificationLine("notshownotification="+toEnd(curr.notificationNoShowMode)),
               privateLine("privatemode="+toEnd(curr.privateMode));

        settingsFile << userLine << directLine << automaticLine << notificationLine << privateLine;
    }
    settingsFile.close();

    context->addToContextMenu();
    notification->setMode(curr.notificationNoShowMode);
    udpDiscover->start(curr.username, curr.privateMode);
    receiver->setPath(StringToWString(direct));
    if(!receiverThread->isRunning())
        receiverThread->start();
}

void MainProgram::addUser(User newUser){
    userVect.push_back(newUser);
    UserSingleton::get_instance().setList(userVect);
}

void MainProgram::deleteUser(User deletedUser){
    userVect.erase(find(userVect.begin(), userVect.end(), deletedUser) );
    UserSingleton::get_instance().setList(userVect);
}
