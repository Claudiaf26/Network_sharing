#include "udp_manager.h"
#include "define.h"
#include <iostream>

UDP_Manager::UDP_Manager(QObject *parent) :
    QObject(parent)
{
    running = false;
    udp = nullptr;
    newThread = nullptr;
    deletedThread = nullptr;
    timerThread = new QThread(this);
    timer = new QTimer();
    timer->setInterval(500);
    timer->moveToThread(timerThread);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkUser()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
}

//gestisci bene thread!
UDP_Manager::~UDP_Manager(){
    if (udp != nullptr)
        delete udp;
    if (newThread != nullptr)
        delete newThread;
    if (deletedThread != nullptr)
        delete deletedThread;
    timerThread->quit();
    timerThread->wait();
    delete timerThread;
}

void UDP_Manager::start(string user, bool mod, string pic){
    UDP_Discover_Status status = (mod != true) ? UDS_ACTIVE : UDS_HIDDEN; //mod == true significa che la modalità privata è attiva
    if (!running){
        udp = new UDPDiscover(user, pic);

        udp->run(status);
        running = !running;
        timerThread->start();
        newThread = new std::thread(&UDP_Manager::run, this, true);
        deletedThread = new std::thread(&UDP_Manager::run, this, false);
        newThread->detach();
        deletedThread->detach();
    }
    else{
        if (user != username){
            udp->changeUserName(user);
        }

        if (mod != mode){
            udp->changeMode(status);
        }

        if (pic != picture)
            udp->changePicture(pic);
    }

    username = user;
    mode = mod;
    picture = pic;
}

void UDP_Manager::run(bool newOrDeleted){
    while(true){
        if(newOrDeleted){
            struct User newUser;
            if(UserListSingleton::get_instance().popNew(newUser)){
                newQueue.push(newUser);
            }
        }
        else{
            struct User deletedUser;
            if(UserListSingleton::get_instance().popDeleted(deletedUser) )
                deletedQueue.push(deletedUser);
        }
    }
}


void UDP_Manager::checkUser(){    
    if(!newQueue.empty()){
        struct User newUser = newQueue.front();
        newQueue.pop();
        emit addUser(newUser);

        string message("L'utente\n\n" + newUser.name + "\n\nè connesso");
        QString imgStr(QString::fromStdString(iconString));
        imgStr.append(QString::fromStdString(newUser.picture));
        emit showSignal(QString::fromStdString(message), imgStr);
    }

    if(!deletedQueue.empty()){
        struct User deletedUser = deletedQueue.front();
        deletedQueue.pop();
        emit deleteUser(deletedUser);

        string message("L'utente\n\n" + deletedUser.name + "\n\nè disconnesso");
        QString imgStr(QString::fromStdString(iconString));
        imgStr.append(QString::fromStdString(deletedUser.picture));
        emit showSignal(QString::fromStdString(message), imgStr);
    }
}
