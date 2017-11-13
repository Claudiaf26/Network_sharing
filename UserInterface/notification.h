#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDialog>
#include <QThread>
#include <QTimer>

namespace Ui {
class Notification;
}

//UI della notifica che appare quando un nuovo utente è connesso/disconnesso
class Notification : public QDialog
{
    Q_OBJECT
private:
    Ui::Notification* ui;

    QThread* m_timerThread;
    QTimer* m_timer;

    //funzioni ausiliarie
    void setPosition();
    void createIcon(const QString&);
public:
    explicit Notification(QWidget *parent = 0);
    ~Notification();

public slots:
    void showNotification(QString, QString);
    void hideNotification();
signals:
    void terminated();
};

#endif // NOTIFICATION_H
