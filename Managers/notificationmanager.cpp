#include "notificationmanager.h"

NotificationManager::NotificationManager(QObject *parent) :
    QObject(parent),
    m_notificationCount(0)
{}

void NotificationManager::showNotification(QString t_text, QString t_picture){
    if (!mode){
        Notification* newNotif = new Notification();
        QObject::connect(newNotif, SIGNAL(terminated()), this, SLOT(removeNotification()) );
        newNotif->showNotification(t_text, t_picture);
        QPoint newNotifPosition = newNotif->pos();
        newNotifPosition.ry() -= (m_notificationCount * newNotif->size().rheight() );
        newNotif->move(newNotifPosition);
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
