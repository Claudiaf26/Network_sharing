#include "sendermanager.h"
#include "define.h"
#include <QApplication>
#include <QMessageBox>

using namespace std;

SenderManager::SenderManager(wstring thisPath):m_filePath(thisPath){
    m_timerThread = new QThread(this);
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    QObject::connect(m_timerThread, SIGNAL(finished()), this, SLOT(deleteLater()));
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
        newTransfer.sendingThread = unique_ptr<std::thread>(new std::thread(&FileTransfer::transfer, newTransfer.transfer.get()));
        newTransfer.progressUI->show();
        m_transferList.push_back(move(newTransfer));
    }

    m_timerThread->start();
}

void SenderManager::checkProgress(){
    if (m_transferList.empty())
        QApplication::quit();
    for (auto it = m_transferList.begin(); it != m_transferList.end();){        
        cout << "INIZIO\n";
        if (it->failed){
            cout << "failed == true\n";
            ++it;
            continue;
        }

        transferStatus status;
        it->transfer->getStatus(status);
        cout << "status: " << status.progress << "\n";
        if (status.progress == FT_ERROR){
            cout << "progress == FT_ERROR\n";
            if (!it->progressUI->isHidden()){
                it->progressUI->close();
                cout << "dovrebbe chiudere la finestra!\n";
            }
            it->failed = true;
            emit error("Il trasferimento è fallito");
        }

        if (!it->progressUI->isClosed()){
            cout << "la finestra è ancora aperta\n";
            QString speedStr = (status.speed == 0) ? "N/A": QString::number(status.speed, 'g', 2);
            speedStr.append(" MBps");
            QString timeStr;
            if (status.secondsLeft > 0){
                int seconds = status.secondsLeft % 60;
                int minutes = status.secondsLeft / 60;
                QString::number(minutes);
                timeStr.append("' "); timeStr.append(QString::number(seconds));
                timeStr.append('s');
            }
            else
                timeStr = "N/A";

            it->progressUI->setProgress(status.progress, speedStr, timeStr);
            ++it;
            cout << "l'aggiornamento dell'UI è terminato\n";
        }
         else {
            cout << "la finestra è chiusa \n";
            if ( ((status.progress != FT_COMPLETE) && (status.progress != FT_ERROR)) ){
                cout << "provo a stoppare il transferimento\n";
                it->transfer->stop();
            }
            if (it->sendingThread->joinable())
                it->sendingThread->join();
            it = m_transferList.erase(it);
            cout << "elemento cancellato\n";
        }
        cout << "FINE\n\n\n\n";
    }
}


