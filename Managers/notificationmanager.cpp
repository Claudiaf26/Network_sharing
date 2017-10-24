#include "notificationmanager.h"

NotificationManager::NotificationManager(QObject *parent) : QObject(parent)
{
    notificationCount = 0;
}

void NotificationManager::showNotification(QString text, QString picture){
    if (!mode){
        Notification* newNotif = new Notification();
        QObject::connect(newNotif, SIGNAL(terminated()), this, SLOT(removeNotification()) );
        QPoint newNotifPosition = newNotif->pos();
        newNotifPosition.ry() -= (notificationCount * newNotif->size().rheight() );
        newNotif->move(newNotifPosition);
        newNotif->showNotification(text, picture);
        notificationQueue.push(newNotif);
        notificationCount++;
    }
}

void NotificationManager::removeNotification(){
    Notification* oldNotif = notificationQueue.front();
    notificationQueue.pop();
    delete oldNotif;
    notificationCount--;
}
