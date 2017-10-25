#include "notificationmanager.h"

NotificationManager::NotificationManager(QObject *parent) :
    QObject(parent),
    m_notificationCount(0)
{}

void NotificationManager::showNotification(QString text, QString picture){
    if (!mode){
        Notification* newNotif = new Notification();
        QObject::connect(newNotif, SIGNAL(terminated()), this, SLOT(removeNotification()) );
        QPoint newNotifPosition = newNotif->pos();
        newNotifPosition.ry() -= (m_notificationCount * newNotif->size().rheight() );
        newNotif->move(newNotifPosition);
        newNotif->showNotification(text, picture);
        m_notificationQueue.push(newNotif);
        m_notificationCount++;
    }
}

void NotificationManager::removeNotification(){
    Notification* oldNotif = m_notificationQueue.front();
    m_notificationQueue.pop();
    delete oldNotif;
    m_notificationCount--;
}
