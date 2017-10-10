#include "sendermanager.h"
#include <QApplication>

using namespace std;

SenderManager::SenderManager(wstring thisPath):path(thisPath){
    timerThread = new QThread(this);
    timer = new QTimer();
    timer->setInterval(1000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkProgress()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
    timer->moveToThread(timerThread);
}

SenderManager::~SenderManager(){
    timerThread->quit();
    timerThread->wait();
    delete timerThread;
    delete timer;
}

void SenderManager::sendToUsers(const vector<User>& selectedUsers){
    for (auto it = selectedUsers.begin(); it != selectedUsers.end(); it++){
        SendingObject newTransfer(path, it->ip);
        newTransfer.sendingThread = unique_ptr<std::thread>(new std::thread(&FileTransfer::transfer, newTransfer.transfer));
        newTransfer.progressUI->show();
        transferList.push_back(move(newTransfer));
    }

    timerThread->start();
}

void SenderManager::checkProgress(){
    if (transferList.empty())
        QApplication::quit();
    for (auto it = transferList.begin(); it != transferList.end();){
        if (!it->progressUI->isClosed()){
            it->progressUI->setProgress(it->transfer->getProgress());
            it++;
        }
        else {
            if (it->transfer->getProgress() < 100)
                it->transfer->stop();
            if(it->sendingThread->joinable())
                it->sendingThread->join();
            it = transferList.erase(it);
        }
    }
}



