#ifndef MAINPROGRAM_H
#define MAINPROGRAM_H

#include "define.h"
#include "UserInterface/startui.h"
#include "UserInterface/userselection.h"
#include "UserInterface/showusers.h"
#include "Managers/notificationmanager.h"
#include "UDPDiscover/UDPDiscover.h"
#include "Managers/udp_manager.h"
#include "ContextMenu/contextmenu.h"
#include "SharedMemory/sharedmem.h"
#include "Managers/receivermanager.h"
#include <QApplication>
#include <QObject>
#include <QThread>
#include <vector>
#include <string>
#include <memory>
#include <fstream>



struct curSettings{
    std::string username;
    std::string directory;
    std::string picture;

    bool automaticMode;
    bool notificationNoShowMode;
    bool privateMode;
};

class MainProgram:public QObject{
    Q_OBJECT
private:
    bool running;

    std::fstream settingsFile;
    std::vector<User> userVect;
    curSettings curr;

    ContextMenu* context;
    StartUI* startUI;
    UserSelection* userUI;
    ShowUsers* showUI;
    NotificationManager* notification;
    UDP_Manager* udpDiscover;
    ReceiverManager* receiver;


public:
    MainProgram();
    ~MainProgram();

public slots:
    void start();
    void startProgram(uint8_t, std::string, std::string, std::string);
    void addUser(User);
    void deleteUser(User);
    void showError(QString);
    void searchUser(std::string&, std::string);

signals:
    void changeSettings(uint8_t, std::string, std::string, std::string);
};

#endif // MAINPROGRAM_H
