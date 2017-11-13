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
#include <exception>

/*struttura ausiliaria utilizzata per tenere collegate l'UI con la barra di progresso,
 *la classe che riceve il file e il thread ad essa collegato*/
struct ReceivingObject {
    ProgressDialog* progressUI;
    std::unique_ptr<FileReceiver> receiver;
    std::unique_ptr<std::thread> receivingThread;
    bool failed;

    ReceivingObject(std::wstring filePath, TCPSocket socket):failed(false){
        progressUI = nullptr;
        receiver = std::unique_ptr<FileReceiver>(new FileReceiver(std::move(socket), filePath));
    }

    ~ReceivingObject(){
        safeDelete(progressUI);

    }

    ReceivingObject(ReceivingObject&& original) {
        this->progressUI = original.progressUI;
        this->receiver = std::move(original.receiver);
        this->receivingThread = std::move(original.receivingThread);
        this->failed = original.failed;
        original.progressUI = nullptr;
        original.receiver = nullptr;
    }
    ReceivingObject& operator=(ReceivingObject&& original){
        if (this != &original){
            delete this->progressUI;
            this->receiver.reset();
            this->receivingThread.reset();
            this->progressUI = original.progressUI;
            this->receiver = std::move(original.receiver);
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

//classe ausiliaria per gestire l'accettazione di nuovi socket
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

//classe che gestisce la ricezione di file
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
