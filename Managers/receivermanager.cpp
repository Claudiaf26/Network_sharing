#include "receivermanager.h"
#include "define.h"
#include <QApplication>
#include <QMessageBox>

using namespace std;

const string sFolder = "Ti sta venendo inviata la cartella\n";
const string sFile = "Ti sta venendo inviato il file\n";

ReceiverManager::ReceiverManager(QObject *parent):automaticMode(false), QObject(parent){
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
    socketThread->quit();
    QThread::sleep(2);
    socketThread->wait();
    delete socketLoop;
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
    TCPSocket socket = socketLoop->getSocket();
    int ret = QMessageBox::Ok;
    string username;
    emit searchUser(username, socket.getPeerIp());
    uint8_t fileOrFolder = FT_FILE;
    string fileName("Error");

    ReceivingObject newReceiver(path, move(socket));
    newReceiver.receiver->getFileDetails(fileName, fileOrFolder);

    if(!automaticMode){
        string message(username);
        if(fileOrFolder == FT_FILE)
            message.append(sFile);
        else if(fileOrFolder == FT_DIRECTORY)
            message.append(sFolder);
        message.append(fileName);

        QMessageBox msgBox;
        msgBox.setText(message.c_str());
        msgBox.setInformativeText("Vuoi accettarlo?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
        ret = msgBox.exec();
    }

    if (ret == QMessageBox::Ok){
        newReceiver.setUI(fileName, username);
        newReceiver.receivingThread = unique_ptr<std::thread>(new std::thread(&FileReceiver::receive, newReceiver.receiver));
        newReceiver.progressUI->show();
        receivingList.push_back(move(newReceiver));
    }
}

void ReceiverManager::checkProgress(){
    for (auto it = receivingList.begin(); it != receivingList.end();){
        if (it->failed){
            it++;
            continue;
        }
        uint8_t status = it->receiver->getStatus();
        if (status == FT_ERROR){
            it->progressUI->close();
            it->failed = true;
            emit error("Il trasferimento è fallito");
        }

        if (!it->progressUI->isClosed()){
            it->progressUI->setProgress(status);
            it++;
        }
         else {
            if ( (status != FT_COMPLETE) && (status != FT_ERROR) )
                it->receiver->stop();
            if (it->receivingThread->joinable())
                it->receivingThread->join();
            it = receivingList.erase(it);
        }
    }
}

