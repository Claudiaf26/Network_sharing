#ifndef RECEIVERMANAGER_H
#define RECEIVERMANAGER_H

#include "UserInterface/progressdialog.h"
#include "File/FileReceiver.h"
#include "define.h"
#include <QObject>
#include <QTimer>
#include <QThread>
#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <atomic>

struct ReceivingObject{
    ProgressDialog* progressUI;
    FileReceiver* receiver;
    std::unique_ptr<std::thread> receivingThread;

    ReceivingObject(std::string filePath, TCPSocket socket){
        progressUI = new ProgressDialog(QString::fromStdString(filePath), true);
        boost::filesystem::path boostPath(filePath);
        receiver = new FileReceiver(std::move(socket), boostPath);
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

};

class ReceiverManager : public QObject {
    Q_OBJECT
private:
    std::string path;
    std::vector<ReceivingObject> receivingList;
    std::unique_ptr<TCPServerSocket> serverSock;
    QThread* timerThread;
    QTimer* timer;
    bool active;
public:
    ReceiverManager();
    ~ReceiverManager();
    void setPath(std::string newPath){this->path = newPath;}
    bool isActive(){return active;}

public slots:
    void loop();
    void checkProgress();
};
#endif // RECEIVERMANAGER_H
