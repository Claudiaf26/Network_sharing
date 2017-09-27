#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QThread>
#include <QTimer>

class Notification: public QDialog {
    Q_OBJECT
    QBoxLayout* dialogLayout;
    QLabel* testo;
    QThread* timerThread;
    QTimer* timer;
public:
    Notification(QWidget *parent = 0);
    ~Notification();
public slots:
    void showNotification(QString);
    void hideNotification();
signals:
    void terminated();
};

#endif // NOTIFICATION_H
