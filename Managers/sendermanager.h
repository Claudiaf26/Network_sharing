#ifndef SENDERMANAGER_H
#define SENDERMANAGER_H

#include "UserInterface/progressdialog.h"
#include "File/FileTransfer.h"
#include "define.h"
#include <QObject>
#include <QTimer>
#include <QThread>
#include <thread>
#include <string>
#include <memory>
#include <vector>

struct SendingObject{
    ProgressDialog* progressUI;
    FileTransfer* transfer;
    std::unique_ptr<std::thread> sendingThread;
    bool failed;

    SendingObject(std::wstring filePath, std::string username, std::string destinationIP):failed(false){
        progressUI = new ProgressDialog(QString::fromStdWString(filePath), QString::fromStdString(username), true);
        transfer = new FileTransfer(destinationIP, filePath);
    }

    ~SendingObject(){
        delete progressUI;
        delete transfer;
    }

    SendingObject(SendingObject&& original) {
        this->progressUI = original.progressUI;
        this->transfer = original.transfer;
        this->sendingThread = std::move(original.sendingThread);
        this->failed = original.failed;
        original.progressUI = nullptr;
        original.transfer = nullptr;
    }
    SendingObject& operator=(SendingObject&& original){
        if (this != &original){
            delete this->progressUI;
            delete this->transfer;
            this->sendingThread.reset();
            this->progressUI = original.progressUI;
            this->transfer = original.transfer;
            this->sendingThread = std::move(original.sendingThread);
            this->failed = original.failed;
            original.progressUI = nullptr;
            original.transfer = nullptr;
        }
        return *this;
    }

};

class SenderManager : public QObject {
    Q_OBJECT
private:
    std::wstring path;
    std::vector<SendingObject> transferList;
    QThread* timerThread;
    QTimer* timer;
public:
    SenderManager(std::wstring);
    ~SenderManager();
public slots:
    void sendToUsers(const std::vector<User>&);
    void checkProgress();


signals:
    void error(QString);
};

#endif // SENDERMANAGER_H
