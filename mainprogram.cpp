#include "mainprogram.h"
#include "systemswrapper.h"
#include "DataStruct/usersingleton.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <exception>

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
    startUI = nullptr;
    userUI = nullptr;
    showUI = nullptr;
    notification = nullptr;
    udpDiscover = nullptr;
    receiver = nullptr;

    if(!UserSingleton::get_instance().initialize())
        exit(EXIT_FAILURE);

    /*QFile f(":/style/darkorange/Style/darkorange.stylesheet");
    if (!f.exists()) {
        printf("Unable to set stylesheet, file not found\n");
    }
    else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }*/

    running = false;

    curr.username.clear();
    curr.directory.clear();
    curr.picture.clear();
    curr.privateMode = false;
    curr.automaticMode = false;
    curr.notificationNoShowMode = false;

    context = new ContextMenu(L"FileSender");
    startUI = new StartUI();
    userUI = new UserSelection();
    showUI = new ShowUsers(true, "");
    notification = new NotificationManager(this);
    udpDiscover = new UDP_Manager(this);
    receiver = new ReceiverManager(this);

    QObject::connect(startUI, SIGNAL(startProgram(uint8_t, std::string, std::string, std::string)), this, SLOT(startProgram(uint8_t, std::string, std::string, std::string)) );
    QObject::connect(userUI, SIGNAL(changeUser(QString, uint8_t)), startUI, SLOT(setUser(QString, uint8_t)));
    QObject::connect(startUI, SIGNAL(showUserList()), showUI, SLOT(show()));
    QObject::connect(startUI, SIGNAL(userChoice()), userUI, SLOT(showOnTop()) );
    QObject::connect(udpDiscover, SIGNAL(showSignal(QString, QString)), notification, SLOT(showNotification(QString, QString)) );
    QObject::connect(udpDiscover, SIGNAL(addUser(User)), this, SLOT(addUser(User)) );
    QObject::connect(udpDiscover, SIGNAL(deleteUser(User)), this, SLOT(deleteUser(User)) );
    QObject::connect(udpDiscover, SIGNAL(addUser(User)), showUI, SLOT(addUser(User)) );
    QObject::connect(udpDiscover, SIGNAL(deleteUser(User)), showUI, SLOT(deleteUser(User)) );
    QObject::connect(this, SIGNAL(changeSettings(uint8_t, std::string, std::string, std::string)), startUI, SLOT(changeSettings(uint8_t, std::string, std::string, std::string)) );
    QObject::connect(this, SIGNAL(changeSettings(uint8_t, std::string, std::string, std::string)), userUI, SLOT(changeSettings(uint8_t, std::string, std::string, std::string)) );
    QObject::connect(receiver, SIGNAL(error(QString)), this, SLOT(showError(QString)), Qt::DirectConnection);
    QObject::connect(userUI, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    QObject::connect(receiver, SIGNAL(searchUser(std::string&,std::string)), this, SLOT(searchUser(std::string&,std::string)), Qt::DirectConnection);
}

MainProgram::~MainProgram(){
    UserSingleton::get_instance().close();
    context->removeFromContextMenu();
    if (context != nullptr)
        delete context;
    if (notification != nullptr)
        delete notification;
    if (udpDiscover != nullptr)
        delete udpDiscover;
    if (receiver != nullptr)
        delete receiver;
    if (startUI != nullptr)
        delete startUI;
    if (userUI != nullptr)
        delete userUI;
    if (showUI != nullptr)
        delete showUI;
}

//cose da cambiare (potenzialmente): rendere const & oggetti passati a funzione, cambiare nomi in m_, aggiungere sottofunzioni per aumentare leggibilità
//(lo scrivo qui per non scriverlo altrove, usa static cast ovunque)

void MainProgram::start(){
    //forse c'è da invertire l'ordine di queste due, va fatto meglio il controllo (se c'è un errore di formattazione?)
    uint8_t flags = 0;
    settingsFile.open("settings.txt", ios_base::in );
    bool firstRegistration = !settingsFile.good();
    if(!firstRegistration){
        try{
            string userLine, directLine, iconLine, automaticLine, notificationLine, privateLine;
            getline(settingsFile, userLine);
            getline(settingsFile, directLine);
            getline(settingsFile, iconLine);
            getline(settingsFile, automaticLine);
            getline(settingsFile, notificationLine);
            getline(settingsFile, privateLine);
            settingsFile.close();

            curr.username = userLine.substr(userLine.find("=")+1);
            curr.directory = directLine.substr(directLine.find("=")+1);
            curr.picture = iconLine.substr(iconLine.find("=")+1);
            char automaticChar = automaticLine[automaticLine.find("=")+1];
            char notificationChar = notificationLine[notificationLine.find("=")+1];
            char privateChar = privateLine[privateLine.find("=")+1];
            curr.automaticMode = (automaticChar == '1') ? true : false;
            curr.notificationNoShowMode = (notificationChar == '1') ? true : false;
            curr.privateMode = (privateChar == '1') ? true : false;

            if (curr.automaticMode)
                flags |= AUTOMATIC_FLAG;
            if (curr.notificationNoShowMode)
                flags |= NOTIFICATION_FLAG;
            if (curr.privateMode)
                flags |= PRIVATE_FLAG;
            emit changeSettings(flags, curr.username, curr.picture, curr.directory);

            startUI->show();

        } catch(...){
            showError("Si è verificato un errore nella lettura dei dati utente, saranno ripristinate le informazioni iniziali");
            firstRegistration = true;
        }
    }

    if (firstRegistration){
        curr.username = SystemsWrapper::getSystemUsername();
        curr.directory = SystemsWrapper::getSystemDownloadFolder();
        emit changeSettings(flags, curr.username, curr.picture, curr.directory);

        userUI->show();
    }
}

void MainProgram::startProgram(uint8_t flg, string user, string icon, string direct){
    curr.username = user;
    curr.directory = direct;
    curr.picture = icon;
    curr.automaticMode = ((flg & AUTOMATIC_FLAG) != 0) ? true : false;
    curr.notificationNoShowMode = ((flg & NOTIFICATION_FLAG) != 0) ? true : false;
    curr.privateMode = ((flg & PRIVATE_FLAG) != 0) ? true : false;

    settingsFile.open("settings.txt", ios_base::out | ios_base::trunc);
    if (settingsFile.good()){
        string userLine("username=" +curr.username+'\n'),
               directLine("directory=" +curr.directory+'\n'),
               iconLine("icon="+curr.picture+'\n'),
               automaticLine("automaticreception="+toEnd(curr.automaticMode)),
               notificationLine("notshownotification="+toEnd(curr.notificationNoShowMode)),
               privateLine("privatemode="+toEnd(curr.privateMode));

        settingsFile << userLine << directLine << iconLine << automaticLine << notificationLine << privateLine;
    }
    settingsFile.close();

    context->addToContextMenu();
    notification->setMode(curr.notificationNoShowMode);
    udpDiscover->start(curr.username, curr.privateMode, curr.picture);
    receiver->setPath(StringToWString(direct));
    receiver->setMode(curr.automaticMode);
    if(receiver->isActive())
        receiver->start();
}

void MainProgram::addUser(User newUser){
    userVect.push_back(newUser);
    UserSingleton::get_instance().setList(userVect);
}

void MainProgram::deleteUser(User deletedUser){
    userVect.erase(find(userVect.begin(), userVect.end(), deletedUser) );
    UserSingleton::get_instance().setList(userVect);
}

void MainProgram::showError(QString errorText){
    QMessageBox errorBox;
    errorBox.setText(errorText);
    errorBox.exec();
}

void MainProgram::searchUser(std::string& name, std::string searchedIP){
    for (auto it = userVect.begin(); it != userVect.end(); it++){
        if (it->ip == searchedIP)
            name = it->name;
    }
}
