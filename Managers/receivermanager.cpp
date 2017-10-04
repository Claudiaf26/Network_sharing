#include "receivermanager.h"
#include <QApplication>

using namespace std;

ReceiverManager::ReceiverManager(): active(true) {
    serverSock = unique_ptr<TCPServerSocket>(new TCPServerSocket(50000));
    timerThread = new QThread(this);
    timer = new QTimer();
    timer->setInterval(1000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkProgress()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
    timer->moveToThread(timerThread);
}

ReceiverManager::~ReceiverManager(){
    active = false;
    serverSock->Close();
    timerThread->quit();
    timerThread->wait();
    delete timerThread;
    delete timer;
}

void ReceiverManager::loop(){
    timerThread->start();
    while(active){
        TCPSocket socket = serverSock->Accept();
        ReceivingObject newReceiver(path, move(socket));
        newReceiver.receivingThread = unique_ptr<std::thread>(new std::thread(&FileReceiver::receive, newReceiver.receiver));
        newReceiver.progressUI->show();
        receivingList.push_back(move(newReceiver));
    }
}

void ReceiverManager::checkProgress(){
    for (auto it = receivingList.begin(); it != receivingList.end(); it++){
        uint8_t progress = it->receiver->getProgress();
        it->progressUI->setProgress(progress);
        if (it->progressUI->isClosed()){
            if (progress < 100)
                it->receiver->stop();
            it->receivingThread->join();
            it->progressUI->close();
            receivingList.erase(it);
        }
    }
}

