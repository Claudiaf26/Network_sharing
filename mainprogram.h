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
    bool m_running;

    std::fstream m_settingsFile;
    std::vector<User> m_userVector;
    curSettings m_currentUser;

    ContextMenu* m_contextMenuHandler;
    StartUI* m_startUI;
    UserSelection* m_userUI;
    ShowUsers* m_showUI;
    NotificationManager* m_notifications;
    UDP_Manager* m_udpDiscover;
    ReceiverManager* m_fileReceiving;

    void clearMembers();
    void clearCurrentUser();
    void initializeMembers();
    void connectSlots();

    void loadCurrentUserFromFile();
    void saveCurrentUserToFile();
    void checkModeFlag(uint8_t);

    void startMainRoutine();
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
