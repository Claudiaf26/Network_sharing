#include "sendermanager.h"
#include "define.h"
#include <QApplication>
#include <QMessageBox>

using namespace std;

SenderManager::SenderManager(wstring thisPath):m_filePath(thisPath){
    m_timerThread = new QThread(this);
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(checkProgress()));
    QObject::connect(m_timerThread, SIGNAL(started()), m_timer, SLOT(start()));
    m_timer->moveToThread(m_timerThread);
}

SenderManager::~SenderManager(){
    m_timerThread->quit();
    m_timerThread->wait();
    delete m_timerThread;
    delete m_timer;
}

void SenderManager::sendToUsers(const vector<User>& selectedUsers){
    for (auto it = selectedUsers.begin(); it != selectedUsers.end(); ++it){
        SendingObject newTransfer(m_filePath, it->name, it->ip);
        newTransfer.sendingThread = unique_ptr<std::thread>(new std::thread(&FileTransfer::transfer, newTransfer.transfer));
        newTransfer.progressUI->show();
        m_transferList.push_back(move(newTransfer));
    }

    m_timerThread->start();
}

void SenderManager::checkProgress(){
    if (m_transferList.empty())
        QApplication::quit();
    for (auto it = m_transferList.begin(); it != m_transferList.end();){
        uint8_t status = it->transfer->getStatus();
        if (it->failed){
            ++it;
            continue;
        }
        if (status == FT_ERROR){

            it->progressUI->close();
            it->failed = true;
            emit error("Il trasferimento è fallito");
        }

        if (!it->progressUI->isClosed()){
            double speed, time;
            it->transfer->getStatistics(speed, time);
            int totSeconds = static_cast<int>(time);
            int seconds = totSeconds % 60;
            int minutes = totSeconds / 60;
            QString speedStr = QString::number(speed, 'g', 2);
            QString timeStr =  QString::number(minutes);
            speedStr.append(" MBps");
            timeStr.append("' "); timeStr.append(QString::number(seconds));
            timeStr.append('s');

            it->progressUI->setProgress(status, speedStr, timeStr);
            ++it;
        }
         else {
            if ( (status != FT_COMPLETE) && (status != FT_ERROR) )
                it->transfer->stop();
            if (it->sendingThread->joinable())
                it->sendingThread->join();
            it = m_transferList.erase(it);
        }
    }
}


