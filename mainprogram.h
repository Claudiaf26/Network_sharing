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


//indica le caratteristiche dell'utente corrente
struct curSettings{
    std::string username;
    std::string directory;
    std::string picture;

    //modalità di utilizzo del programma
    bool automaticMode;                 //accetta automaticamente i file inviati
    bool notificationNoShowMode;        //non mostra le notifiche quando un utente si connette
    bool privateMode;                   //rende invisibili agli altri utenti, permettendo comunque di inviare i file in modalità anonima
};

//questa classe gestisce le operazioni principali che il programma deve compiere all'avvio
class MainProgram:public QObject{
    Q_OBJECT
private:
    bool m_running;                     //flag che indica se il programma è in ascolto sulla rete

    std::fstream m_settingsFile;        //usato per salvare e scrivere le impostazioni dell'utente su file
    std::vector<User> m_userVector;     //vettore degli utenti connessi
    curSettings m_currentUser;          //contiene le informazioni sulla sessione in corso

    ContextMenu* m_contextMenuHandler;  //gestisce l'aggiunta del programma al menù a tendina del SO

    //interfacce grafiche
    StartUI* m_startUI;
    UserSelection* m_userUI;
    ShowUsers* m_showUI;

    NotificationManager* m_notifications;//gestisce le notifiche mostrate quando un utente si (dis)connette
    UDP_Manager* m_udpDiscover;         //gestisce l'individuazione di nuovi utenti connessi sulla rete
    ReceiverManager* m_fileReceiving;   //gestisce la ricezione di file

    //funzioni ausiliarie
    void clearMembers();
    void clearCurrentUser();
    void initializeMembers();
    void connectSlots();

    void loadCurrentUserFromFile();
    void saveCurrentUserToFile();
    void checkModeFlag(uint8_t&);

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
