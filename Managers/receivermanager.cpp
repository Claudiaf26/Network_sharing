#include "receivermanager.h"
#include "define.h"
#include <QApplication>
#include <QMessageBox>

using namespace std;

const string m_folderString = "Ti sta venendo inviata la cartella\n";
const string m_fileString = "Ti sta venendo inviato il file\n";

ReceiverManager::ReceiverManager(QObject *parent):m_automaticMode(false), QObject(parent){
    m_socketThread = new QThread(this);
    m_timerThread = new QThread(this);
    m_timer = new QTimer();
    m_loopAccept = new SocketLoop();
    m_timer->setInterval(1000);
    QObject::connect(m_loopAccept, SIGNAL(createUI()), this, SLOT(createUI()), Qt::BlockingQueuedConnection);
    QObject::connect(m_socketThread, SIGNAL(started()), m_loopAccept, SLOT(loop()));
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(checkProgress()));
    QObject::connect(m_timerThread, SIGNAL(started()), m_timer, SLOT(start()));
    m_timer->moveToThread(m_timerThread);
    m_loopAccept->moveToThread(m_socketThread);
}

ReceiverManager::~ReceiverManager(){
    m_loopAccept->disable();
    m_socketThread->quit();
    m_socketThread->wait();
    delete m_loopAccept;
    m_timerThread->quit();
    m_timerThread->wait();
    delete m_timerThread;
    delete m_socketThread;

}
void ReceiverManager::start(){
    m_socketThread->start();
    m_timerThread->start();
}

void ReceiverManager::createUI(){
    TCPSocket socket = m_loopAccept->getSocket();
    int ret = QMessageBox::Ok;
    string username;
    emit searchUser(username, socket.getPeerIp());
    if (username.empty())
        username = "Un utente anonimo";
    uint8_t fileOrFolder = FT_FILE;
    string fileName("Error");

    ReceivingObject newReceiver(m_downloadPath, move(socket));
    newReceiver.receiver->getFileDetails(fileName, fileOrFolder);

    if(!m_automaticMode){
        string message(username);
        if(fileOrFolder == FT_FILE)
            message.append(m_fileString);
        else if(fileOrFolder == FT_DIRECTORY)
            message.append(m_folderString);
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
        m_receivingList.push_back(move(newReceiver));
    }
}

void ReceiverManager::checkProgress(){
    for (auto it = m_receivingList.begin(); it != m_receivingList.end();){
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
            /*double speed, time;
            it->transfer->getStatistics(speed, time);
            int totSeconds = static_cast<int>(time);
            int seconds = totSeconds % 60;
            int minutes = totSeconds / 60;
            QString speedStr = QString::number(speed, 'g', 2);
            QString timeStr =  QString::number(minutes);
            speedStr.append(" MBps");
            timeStr.append("' "); timeStr.append(QString::number(seconds));
            timeStr.append('s');*/

            it->progressUI->setProgress(status, "", "");
            it++;
        }
         else {
            if ( (status != FT_COMPLETE) && (status != FT_ERROR) )
                it->receiver->stop();
            if (it->receivingThread->joinable())
                it->receivingThread->join();
            it = m_receivingList.erase(it);
        }
    }
}

