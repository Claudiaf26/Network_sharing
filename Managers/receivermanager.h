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
#include <queue>
#include <atomic>
#include <iostream>

struct ReceivingObject {
    ProgressDialog* progressUI;
    FileReceiver* receiver;
    std::unique_ptr<std::thread> receivingThread;
    bool failed;

    ReceivingObject(std::wstring filePath, TCPSocket socket):failed(false){
        progressUI = nullptr;
        receiver = new FileReceiver(std::move(socket), filePath);
    }

    ~ReceivingObject(){
        if (progressUI != nullptr)
            delete progressUI;
        if (receiver != nullptr)
            delete receiver;
    }

    ReceivingObject(ReceivingObject&& original) {
        this->progressUI = original.progressUI;
        this->receiver = original.receiver;
        this->receivingThread = std::move(original.receivingThread);
        this->failed = original.failed;
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
            this->failed = original.failed;
            original.progressUI = nullptr;
            original.receiver = nullptr;
        }
        return *this;
    }

    void setUI(std::string fileName, std::string username){
        progressUI = new ProgressDialog(QString::fromStdString(fileName), QString::fromStdString(username), false);
    }

};

class SocketLoop : public QObject {
    Q_OBJECT
private:
    unique_ptr<TCPServerSocket> m_serverSocket;
    std::queue<TCPSocket> m_newSockets;
    bool m_active;
public:
    SocketLoop(): m_active(true){
        m_serverSocket = unique_ptr<TCPServerSocket>(new TCPServerSocket(50000));
    }
    ~SocketLoop(){}
    TCPSocket getSocket() {
        TCPSocket tempSocket = std::move(m_newSockets.front());
        m_newSockets.pop();
        return std::move(tempSocket);
    }
    bool getState(){return m_active;}
    void disable(){
        m_active = false;
        m_serverSocket->Close();}
public slots:
    void loop(){
        while(m_active){
                try{
                    TCPSocket socket = m_serverSocket->Accept();
                if (m_active){
                    m_newSockets.push(std::move(socket));
                    emit createUI();
                }
                }catch(...){}
            }
    }
signals:
    void createUI();
};

class ReceiverManager : public QObject {
    Q_OBJECT
private:
    const string m_folderString = " ti sta inviando la cartella\n";
    const string m_fileString = " ti sta inviando il file\n";

    std::wstring m_downloadPath;
    std::vector<ReceivingObject> m_receivingList;
    QThread* m_timerThread;
    QThread* m_socketThread;
    QTimer* m_timer;
    SocketLoop* m_loopAccept;
    bool m_automaticMode;
public:
    ReceiverManager(QObject *parent = 0);
    ~ReceiverManager();
    void setPath(std::wstring newPath){this->m_downloadPath = newPath;}
    void start();
    bool isActive(){return m_loopAccept->getState();}
    void setMode(bool mode){m_automaticMode = mode;}

public slots:
    void createUI();
    void checkProgress();

signals:
    void error(QString);
    void searchUser(std::string&, std::string);

};
#endif // RECEIVERMANAGER_H
