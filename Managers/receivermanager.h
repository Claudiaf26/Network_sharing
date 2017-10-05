#ifndef RECEIVERMANAGER_H
#define RECEIVERMANAGER_H

#include "UserInterface/progressdialog.h"
#include "File/FileReceiver.h"
#include "define.h"
#include <QObject>
#include <QMetaObject>
#include <QTimer>
#include <QThread>
#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <atomic>

struct ReceivingObject : public QObject{
    ProgressDialog* progressUI;
    FileReceiver* receiver;
    std::unique_ptr<std::thread> receivingThread;

    ReceivingObject(std::wstring filePath/*, TCPSocket socket*/){
        progressUI = nullptr;
        //receiver = new FileReceiver(std::move(socket), filePath);
    }

    ~ReceivingObject(){
        delete progressUI;
        delete receiver;
    }

    ReceivingObject(ReceivingObject&& original) {
        this->progressUI = original.progressUI;
        this->receiver = original.receiver;
        this->receivingThread = std::move(original.receivingThread);
        original.progressUI = nullptr;
        original.receiver = nullptr;
    }
    ReceivingObject& operator=(ReceivingObject&& original){
        if (this != &original){
            delete this->progressUI;
            delete this->receiver;
            this->receivingThread.reset();
            this->progressUI = original.progressUI;
            this->receiver = original.receiver;
            this->receivingThread = std::move(original.receivingThread);
            original.progressUI = nullptr;
            original.receiver = nullptr;
        }
        return *this;
    }

    void createProgressUI(std::wstring filePath){
        progressUI = new ProgressDialog(QString::fromStdWString(filePath), true);
    }
};

class ReceiverManager : public QObject {
    Q_OBJECT
private:
    std::wstring path;
    ReceivingObject* temp;
    std::vector<ReceivingObject> receivingList;
    std::unique_ptr<TCPServerSocket> serverSock;
    QThread* timerThread;
    QTimer* timer;
    bool active;
    bool automaticMode; //da implementare
public:
    ReceiverManager();
    ~ReceiverManager();
    void setPath(std::wstring newPath){this->path = newPath;}
    bool isActive(){return active;}

public slots:
    void loop();
    void checkProgress();
    void setUI(){temp->createProgressUI(path);}

};
#endif // RECEIVERMANAGER_H
