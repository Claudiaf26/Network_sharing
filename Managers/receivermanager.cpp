#include "receivermanager.h"
#include <QApplication>

using namespace std;

ReceiverManager::ReceiverManager() {
    socketThread = new QThread(this);
    timerThread = new QThread(this);
    timer = new QTimer();
    socketLoop = new SocketThread();
    timer->setInterval(1000);
    QObject::connect(socketLoop, SIGNAL(createUI()), this, SLOT(createUI()), Qt::BlockingQueuedConnection);
    QObject::connect(socketThread, SIGNAL(started()), socketLoop, SLOT(loop()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkProgress()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
    timer->moveToThread(timerThread);
    socketLoop->moveToThread(socketThread);
}

ReceiverManager::~ReceiverManager(){
    socketLoop->disable();
    delete socketLoop;
    delete timer;
    socketThread->quit();
    socketThread->wait();
    timerThread->quit();
    timerThread->wait();
    delete timerThread;
    delete socketThread;

}
void ReceiverManager::start(){
    socketThread->start();
    timerThread->start();
}

void ReceiverManager::createUI(){
    ReceivingObject newReceiver(path, move(*(socketLoop->getSocket())));
    newReceiver.receivingThread = unique_ptr<std::thread>(new std::thread(&FileReceiver::receive, newReceiver.receiver));
    newReceiver.progressUI->show();
    receivingList.push_back(move(newReceiver));
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

