#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "UserInterface/notification.h"
#include <QObject>
#include <queue>
#include <memory>

class NotificationManager : public QObject
{
    Q_OBJECT
private:
  std::queue<Notification*> notificationQueue;
  uint16_t notificationCount;
  bool mode; //notificationNoShowMode
public:
    explicit NotificationManager(QObject *parent = 0);
    void setMode(bool m){mode = m;}
signals:

public slots:
    void showNotification(QString, QString);
    void removeNotification();
};

#endif // NOTIFICATIONMANAGER_H
