#include "sendermanager.h"
#include "define.h"
#include <QApplication>
#include <QMessageBox>

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
        uint8_t status = it->transfer->getStatus();
        if (status == FT_ERROR){
            it->progressUI->close();
            emit error("Il trasferimento è fallito");
        }

        if (!it->progressUI->isClosed()){
            it->progressUI->setProgress(status);
            it++;
        }
         else {
            if ( (status != FT_COMPLETE) && (status != FT_ERROR) )
                it->transfer->stop();
            if (it->sendingThread->joinable())
                it->sendingThread->join();
            it = transferList.erase(it);
        }
    }
}

void SenderManager::showError(QString errorText){
    QMessageBox errorBox;
    errorBox.setText(errorText);
    errorBox.exec();
}



