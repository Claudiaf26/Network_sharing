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

struct ReceivingObject {
    ProgressDialog* progressUI;
    FileReceiver* receiver;
    std::unique_ptr<std::thread> receivingThread;

    ReceivingObject(std::wstring filePath, TCPSocket socket){
        progressUI = new ProgressDialog(QString::fromStdWString(filePath), true);
        receiver = new FileReceiver(std::move(socket), filePath);
    }

    ~ReceivingObject(){
        delete progressUI;
        delete receiver;
        receivingThread.release();
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

class SocketThread : public QObject {
    Q_OBJECT
private:
    unique_ptr<TCPServerSocket> serverSock;
    TCPSocket* newSocket;
    bool active;
public:
    SocketThread(): active(true){
        newSocket = nullptr;
        serverSock = unique_ptr<TCPServerSocket>(new TCPServerSocket(50000));
    }
    ~SocketThread(){
        if (newSocket != nullptr)
            delete newSocket;
        serverSock.release();
    }
    TCPSocket* getSocket() {
        TCPSocket* tempSocket = newSocket;
        newSocket = nullptr;
        return tempSocket;
    }
    bool getState(){return active;}
    void disable(){
        active = false;
        serverSock->Close();}
public slots:
    void loop(){
        while(active){
                try{
                    TCPSocket socket = serverSock->Accept();
                    newSocket = &socket;
                }catch(...){}

                if (active)
                    emit createUI();
        }
    }
signals:
    void createUI();
};

class ReceiverManager : public QObject {
    Q_OBJECT
private:
    std::wstring path;
    std::vector<ReceivingObject> receivingList;
    QThread* timerThread;
    QThread* socketThread;
    QTimer* timer;
    SocketThread* socketLoop;
    bool automaticMode; //da implementare
public:
    ReceiverManager();
    ~ReceiverManager();
    void setPath(std::wstring newPath){this->path = newPath;}
    void start();
    bool isActive(){return socketLoop->getState();}

public slots:
    void createUI();
    void checkProgress();

};
#endif // RECEIVERMANAGER_H
