#include "udp_manager.h"
#include <iostream>

UDP_Manager::UDP_Manager(QObject *parent) :
    QObject(parent)
{
    //creazione oggetti di prova
    User user1, user2, user3, user4;
    user1.name = "Lamberti";
    user1.ip = "192.178.3.3";
    user2.name = "Pagiogi";
    user2.ip = "192.178.2.1";
    user3.name = "Raggibusco";
    user3.ip = "192.178.10.0";
    user4.name = "Lamberti";
    user4.ip = "192.178.10.0";
    newQueue.push(user1);
    newQueue.push(user2);
    newQueue.push(user3);
    newQueue.push(user4);
    //fine prova


    running = false;
    udp = nullptr;
    newThread = nullptr;
    deletedThread = nullptr;
    timerThread = new QThread(this);
    timer = new QTimer(this);
    timer->setInterval(500);
    timer->moveToThread(timerThread);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkUser()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
}

//gestisci bene thread!
UDP_Manager::~UDP_Manager(){
    delete udp;
    delete newThread;
    delete deletedThread;
    timerThread->quit();
    timerThread->wait();
}

void UDP_Manager::start(string user, bool mod){
    mode = mod;
    UDP_Discover_Status status = (mode != true) ? UDS_ACTIVE : UDS_HIDDEN; //ricordati di modificare in UDS_STOP
    if (!running){
        udp = new UDP_Discover(user);

        udp->run(status);
        running = !running;
        timerThread->start();
        newThread = new std::thread(&UDP_Manager::run, this, true);
        deletedThread = new std::thread(&UDP_Manager::run, this, false);
        newThread->detach();
        deletedThread->detach();
    }
    else{
        if (user != username)
            udp->run(status, user);
    }

    username = user;
}

void UDP_Manager::run(bool newOrDeleted){
    while(true){
        if(newOrDeleted){
            struct User newUser;
            if(ConnectionSingleton::get_instance().popNew(newUser)){
                newQueue.push(newUser);
            }
        }
        else{
            struct User deletedUser;
            if(ConnectionSingleton::get_instance().popDeleted(deletedUser) )
                deletedQueue.push(deletedUser);
        }
    }
}


void UDP_Manager::checkUser(){    
    if(!newQueue.empty()){
        struct User newUser = newQueue.front();
        newQueue.pop();
        emit addUser(newUser);

        string message("L'utente " + newUser.name + " è connesso");
        emit showSignal(QString::fromStdString(message));
    }

    if(!deletedQueue.empty()){
        struct User deletedUser = deletedQueue.front();
        deletedQueue.pop();
        emit deleteUser(deletedUser);

        string message("L'utente " + deletedUser.name + " è disconnesso");
        emit showSignal(QString::fromStdString(message));
    }
}
