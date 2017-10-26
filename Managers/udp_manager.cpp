#include "udp_manager.h"
#include "define.h"
#include <iostream>

UDP_Manager::UDP_Manager(QObject *parent) :
    QObject(parent)
{
    m_running = false;
    m_udpDiscovery = nullptr;
    m_newThread = nullptr;
    m_deletedThread = nullptr;
    m_timerThread = new QThread(this);
    m_timer = new QTimer();
    m_timer->setInterval(500);
    m_timer->moveToThread(m_timerThread);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(checkUser()));
    QObject::connect(m_timerThread, SIGNAL(started()), m_timer, SLOT(start()));
}

//gestisci bene thread!
UDP_Manager::~UDP_Manager(){
    safeDelete(m_udpDiscovery);
    safeDelete(m_newThread);
    safeDelete(m_deletedThread);
    m_timerThread->quit();
    m_timerThread->wait();
    delete m_timerThread;
    delete m_timer;
}

void UDP_Manager::start(string t_username, bool t_privateMode, string t_picture){
    UDP_Discover_Status status = (t_privateMode != true) ? UDS_ACTIVE : UDS_HIDDEN; //mod == true significa che la modalità privata è attiva
    if (!m_running){
        m_udpDiscovery = new UDPDiscover(t_username, t_picture);

        m_udpDiscovery->run(status);
        m_running = !m_running;
        m_timerThread->start();
        m_newThread = new std::thread(&UDP_Manager::run, this, true);
        m_deletedThread = new std::thread(&UDP_Manager::run, this, false);
        m_newThread->detach();
        m_deletedThread->detach();
    }
    else{
        if (t_username != m_username){
            m_udpDiscovery->changeUserName(t_username);
        }

        if (t_privateMode != m_privateMode){
            m_udpDiscovery->changeMode(status);
        }

        if (t_picture != m_picture)
            m_udpDiscovery->changePicture(t_picture);
    }

    m_username = t_username;
    m_privateMode = t_privateMode;
    m_picture = t_picture;
}

void UDP_Manager::run(bool t_newOrDeleted){
    while(true){
        if(t_newOrDeleted){
            struct User newUser;
            if(UserListSingleton::get_instance().popNew(newUser)){
                m_newUsersQueue.push(newUser);
            }
        }
        else{
            struct User deletedUser;
            if(UserListSingleton::get_instance().popDeleted(deletedUser) )
                m_deletedUsersQueue.push(deletedUser);
        }
    }
}


void UDP_Manager::checkUser(){    
    if(!m_newUsersQueue.empty()){
        struct User newUser = m_newUsersQueue.front();
        m_newUsersQueue.pop();
        emit addUser(newUser);

        string message("L'utente\n\n" + newUser.name + "\n\nè connesso");
        QString imgStr(QString::fromStdString(iconString));
        imgStr.append(QString::fromStdString(newUser.picture));
        emit showSignal(QString::fromStdString(message), imgStr);
    }

    if(!m_deletedUsersQueue.empty()){
        struct User deletedUser = m_deletedUsersQueue.front();
        m_deletedUsersQueue.pop();
        emit deleteUser(deletedUser);

        string message("L'utente\n\n" + deletedUser.name + "\n\nè disconnesso");
        QString imgStr(QString::fromStdString(iconString));
        imgStr.append(QString::fromStdString(deletedUser.picture));
        emit showSignal(QString::fromStdString(message), imgStr);
    }
}
