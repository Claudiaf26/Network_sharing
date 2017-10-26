#ifndef UDP_MANAGER_H
#define UDP_MANAGER_H

#include "UDPDiscover/UDPDiscover.h"
#include "DataStruct/userlistsingleton.h"
#include <QObject>
#include <QThread>
#include <QTimer>
#include <queue>
#include <thread>
#include <string>

//classe che gestisce la ricezione di nuovi utenti connessi
class UDP_Manager : public QObject
{
    Q_OBJECT
private:
    UDPDiscover* m_udpDiscovery;

    std::queue<struct User> m_newUsersQueue;
    std::queue<struct User> m_deletedUsersQueue;

    std::string m_username;
    std::string m_picture;

    std::thread* m_newThread;
    std::thread* m_deletedThread;

    bool m_running;
    bool m_privateMode;

    QThread* m_timerThread;
    QTimer* m_timer;

    void run(bool);
public:
    explicit UDP_Manager(QObject *parent = 0 );
    ~UDP_Manager();
    void start(string, bool, std::string);
signals:
    void showSignal(QString, QString);
    void addUser(User);
    void deleteUser(User);
public slots:
    void checkUser();
};

#endif // UDP_MANAGER_H
