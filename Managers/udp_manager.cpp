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
    if (m_udpDiscovery != nullptr)
        delete m_udpDiscovery;
    if (m_newThread != nullptr)
        delete m_newThread;
    if (m_deletedThread != nullptr)
        delete m_deletedThread;
    m_timerThread->quit();
    m_timerThread->wait();
    delete m_timerThread;
}

void UDP_Manager::start(string user, bool mod, string pic){
    UDP_Discover_Status status = (mod != true) ? UDS_ACTIVE : UDS_HIDDEN; //mod == true significa che la modalità privata è attiva
    if (!m_running){
        m_udpDiscovery = new UDPDiscover(user, pic);

        m_udpDiscovery->run(status);
        m_running = !m_running;
        m_timerThread->start();
        m_newThread = new std::thread(&UDP_Manager::run, this, true);
        m_deletedThread = new std::thread(&UDP_Manager::run, this, false);
        m_newThread->detach();
        m_deletedThread->detach();
    }
    else{
        if (user != m_username){
            m_udpDiscovery->changeUserName(user);
        }

        if (mod != m_privateMode){
            m_udpDiscovery->changeMode(status);
        }

        if (pic != m_picture)
            m_udpDiscovery->changePicture(pic);
    }

    m_username = user;
    m_privateMode = mod;
    m_picture = pic;
}

void UDP_Manager::run(bool newOrDeleted){
    while(true){
        if(newOrDeleted){
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
