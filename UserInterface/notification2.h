#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QThread>
#include <QTimer>

class Notification2: public QDialog {
    Q_OBJECT
    QBoxLayout* dialogLayout;
    QLabel* testo;
    QThread* timerThread;
    QTimer* timer;
public:
    Notification2(QWidget *parent = 0);
    ~Notification2();
public slots:
    void showNotification(QString);
    void hideNotification();
signals:
    void terminated();
};

#endif // NOTIFICATION_H
