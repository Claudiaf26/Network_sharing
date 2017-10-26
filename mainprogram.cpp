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
//----------- funzioni inline --------------------------------------------
inline string toEnd(const bool& value){
    return value ? "1\n" : "0\n";
}

inline wstring StringToWString(const string& s){
 wstring temp(s.length(),' ');
 copy(s.begin(), s.end(), temp.begin());
 return temp;
 }
//------------------------------------------------------------------------

MainProgram::MainProgram(): m_running(false){
    clearMembers();
    if(!UserSingleton::get_instance().initialize()){
        showError("ATTENZIONE: Un altra sessione del programma è già in corso!");
        exit(EXIT_FAILURE);
    }
    /*QFile f(":/style/darkorange/Style/darkorange.stylesheet");
    if (!f.exists()) {
        printf("Unable to set stylesheet, file not found\n");
    }
    else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }*/      
    clearCurrentUser();
    initializeMembers();
    connectSlots();
}

MainProgram::~MainProgram(){
    UserSingleton::get_instance().close();
    if (m_contextMenuHandler->isAddedToContextMenu())
        if (!m_contextMenuHandler->removeFromContextMenu())
            showError("Non è stato possibile chiudere il menù contestuale,\n\
                       il programma rimarrà nel menù accessibile tramite click destro su qualunque file\n\
                       o cartella! Riavviare per risolvere il problema");
    safeDelete(m_contextMenuHandler);
    safeDelete(m_notifications);
    safeDelete(m_udpDiscover);
    safeDelete(m_fileReceiving);
    safeDelete(m_startUI);
    safeDelete(m_userUI);
    safeDelete(m_showUI);
}

void MainProgram::start(){
    uint8_t flags = 0;
    m_settingsFile.open("settings.txt", ios_base::in );
    bool firstRegistration = !m_settingsFile.good();
    if(!firstRegistration){
        try{
            loadCurrentUserFromFile();
            checkModeFlag(flags);
            emit changeSettings(flags, m_currentUser.username,
                                m_currentUser.picture, m_currentUser.directory);
            m_startUI->show();

        } catch(...){
            showError("Si è verificato un errore nella lettura dei dati utente,\
                       saranno ripristinate le informazioni iniziali");
            firstRegistration = true;
        }
    }

    if (firstRegistration){
        m_currentUser.username = SystemsWrapper::getSystemUsername();
        m_currentUser.directory = SystemsWrapper::getSystemDownloadFolder();
        emit changeSettings(flags, m_currentUser.username,
                            m_currentUser.picture, m_currentUser.directory);
        m_userUI->show();
    }
}

void MainProgram::startProgram(uint8_t t_flag, string t_username, string t_picture, string t_directory){
    m_currentUser.username = t_username;
    m_currentUser.directory = t_directory;
    m_currentUser.picture = t_picture;
    m_currentUser.automaticMode = ((t_flag & AUTOMATIC_FLAG) != 0) ? true : false;
    m_currentUser.notificationNoShowMode = ((t_flag & NOTIFICATION_FLAG) != 0) ? true : false;
    m_currentUser.privateMode = ((t_flag & PRIVATE_FLAG) != 0) ? true : false;

    m_settingsFile.open("settings.txt", ios_base::out | ios_base::trunc);
    if (m_settingsFile.good())
        saveCurrentUserToFile();
    m_settingsFile.close();
    startMainRoutine();
}
//--------------------------------------------------------------------------

//------------------------- slot -------------------------------------------
void MainProgram::addUser(User t_newUser){
    m_userVector.push_back(t_newUser);
    try{
        UserSingleton::get_instance().setList(m_userVector);
    }catch(runtime_error error){
        showError("Errore nel salvataggio dell'utente connesso\nsulla memoria condivisa, il programma verrà chiuso");
        QApplication::exit(-1);
    }
}

void MainProgram::deleteUser(User t_deletedUser){
    m_userVector.erase(find(m_userVector.begin(), m_userVector.end(), t_deletedUser) );
    try{
        UserSingleton::get_instance().setList(m_userVector);
    }catch(runtime_error error){
        showError("Errore nel salvataggio dell'utente disconnesso\nsulla memoria condivisa, il programma verrà chiuso");
        QApplication::exit(-1);
    }
}

void MainProgram::showError(QString t_errorText){
    QMessageBox errorBox;
    errorBox.setText(t_errorText);
    errorBox.exec();
}

void MainProgram::searchUser(std::string& t_name, std::string t_searchedIP){
    for (auto it = m_userVector.begin(); it != m_userVector.end(); ++it){
        if (it->ip == t_searchedIP)
            t_name = it->name;
    }
}
//-------------------------------------------------------------------------

//----------------------------- funzioni ausiliarie -----------------------
void MainProgram::clearMembers(){
    m_contextMenuHandler = nullptr;
    m_startUI = nullptr;
    m_userUI = nullptr;
    m_showUI = nullptr;
    m_notifications = nullptr;
    m_udpDiscover = nullptr;
    m_fileReceiving = nullptr;
}
void MainProgram::clearCurrentUser(){
    m_currentUser.username.clear();
    m_currentUser.directory.clear();
    m_currentUser.picture.clear();
    m_currentUser.privateMode = false;
    m_currentUser.automaticMode = false;
    m_currentUser.notificationNoShowMode = false;
}

void MainProgram::initializeMembers(){
    m_contextMenuHandler = new ContextMenu(L"FileSender");
    m_startUI = new StartUI();
    m_userUI = new UserSelection();
    m_showUI = new ShowUsers(true, "");
    m_notifications = new NotificationManager(this);
    m_udpDiscover = new UDP_Manager(this);
    m_fileReceiving = new ReceiverManager(this);
}

void MainProgram::connectSlots(){
    QObject::connect(m_startUI, SIGNAL(startProgram(uint8_t, std::string, std::string, std::string)), this, SLOT(startProgram(uint8_t, std::string, std::string, std::string)) );
    QObject::connect(m_userUI, SIGNAL(changeUser(QString, QString)), m_startUI, SLOT(setUser(QString, QString)));
    QObject::connect(m_startUI, SIGNAL(showUserList()), m_showUI, SLOT(show()));
    QObject::connect(m_startUI, SIGNAL(userChoice()), m_userUI, SLOT(showOnTop()) );
    QObject::connect(m_udpDiscover, SIGNAL(showSignal(QString, QString)), m_notifications, SLOT(showNotification(QString, QString)) );
    QObject::connect(m_udpDiscover, SIGNAL(addUser(User)), this, SLOT(addUser(User)) );
    QObject::connect(m_udpDiscover, SIGNAL(deleteUser(User)), this, SLOT(deleteUser(User)) );
    QObject::connect(m_udpDiscover, SIGNAL(addUser(User)), m_showUI, SLOT(addUser(User)) );
    QObject::connect(m_udpDiscover, SIGNAL(deleteUser(User)), m_showUI, SLOT(deleteUser(User)) );
    QObject::connect(this, SIGNAL(changeSettings(uint8_t, std::string, std::string, std::string)), m_startUI, SLOT(changeSettings(uint8_t, std::string, std::string, std::string)) );
    QObject::connect(this, SIGNAL(changeSettings(uint8_t, std::string, std::string, std::string)), m_userUI, SLOT(changeSettings(uint8_t, std::string, std::string, std::string)) );
    QObject::connect(m_fileReceiving, SIGNAL(error(QString)), this, SLOT(showError(QString)), Qt::DirectConnection);
    QObject::connect(m_userUI, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    QObject::connect(m_fileReceiving, SIGNAL(searchUser(std::string&,std::string)), this, SLOT(searchUser(std::string&,std::string)), Qt::DirectConnection);
}

void MainProgram::loadCurrentUserFromFile(){
    string userLine, directLine, iconLine, automaticLine, notificationLine, privateLine;
    getline(m_settingsFile, userLine);
    getline(m_settingsFile, directLine);
    getline(m_settingsFile, iconLine);
    getline(m_settingsFile, automaticLine);
    getline(m_settingsFile, notificationLine);
    getline(m_settingsFile, privateLine);
    m_settingsFile.close();
    if (userLine.empty() || directLine.empty() || iconLine.empty() || automaticLine.empty() || notificationLine.empty() || privateLine.empty())
        throw runtime_error("Wrong formatting of user settings, empty lines!");

    if((userLine.find("username=") == string::npos) || (directLine.find("directory=") == string::npos) ||
       (iconLine.find("icon=") == string::npos) || (automaticLine.find("automaticreception=") == string::npos) ||
       (notificationLine.find("notshownotification=") == string::npos) || (privateLine.find("privatemode=") == string::npos) )
        throw runtime_error("Wrong formatting of user settings, wrong line start!");

    m_currentUser.username = userLine.substr(userLine.find("=")+1);
    m_currentUser.directory = directLine.substr(directLine.find("=")+1);
    m_currentUser.picture = iconLine.substr(iconLine.find("=")+1);
    char automaticChar = automaticLine[automaticLine.find("=")+1];
    char notificationChar = notificationLine[notificationLine.find("=")+1];
    char privateChar = privateLine[privateLine.find("=")+1];
    m_currentUser.automaticMode = (automaticChar == '1') ? true : false;
    m_currentUser.notificationNoShowMode = (notificationChar == '1') ? true : false;
    m_currentUser.privateMode = (privateChar == '1') ? true : false;

}

void MainProgram::checkModeFlag(uint8_t& t_flags){
    if (m_currentUser.automaticMode)
        t_flags |= AUTOMATIC_FLAG;
    if (m_currentUser.notificationNoShowMode)
        t_flags |= NOTIFICATION_FLAG;
    if (m_currentUser.privateMode)
        t_flags |= PRIVATE_FLAG;
}

void MainProgram::saveCurrentUserToFile(){
    string userLine("username=" +m_currentUser.username+'\n'),
           directLine("directory=" +m_currentUser.directory+'\n'),
           iconLine("icon="+m_currentUser.picture+'\n'),
           automaticLine("automaticreception="+toEnd(m_currentUser.automaticMode)),
           notificationLine("notshownotification="+toEnd(m_currentUser.notificationNoShowMode)),
           privateLine("privatemode="+toEnd(m_currentUser.privateMode));

    m_settingsFile << userLine << directLine << iconLine << automaticLine << notificationLine << privateLine;
}

void MainProgram::startMainRoutine(){
    m_running = true;
    if(!m_contextMenuHandler->addToContextMenu()){
        showError("Non è stato possibile aggiungere l'applicazione nel menù a tendina!\n\
                   L'applicazione verrà chiusa");
        m_running = false;
        QApplication::exit(-1);
    }
    if (m_running)
        m_notifications->setMode(m_currentUser.notificationNoShowMode);

    try{
        if (m_running)
            m_udpDiscover->start(m_currentUser.username, m_currentUser.privateMode, m_currentUser.picture);
    }catch(...){
        showError("Non è stato possibile stabilire una connessione UDP, l'applicazione verrà chiusa");
        m_running = false;
        QApplication::exit(-1);
    }

    m_fileReceiving->setPath(StringToWString(m_currentUser.directory));
    m_fileReceiving->setMode(m_currentUser.automaticMode);
    if(m_fileReceiving->isActive())
        m_fileReceiving->start();
}
