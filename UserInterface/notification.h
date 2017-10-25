#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDialog>
#include <QThread>
#include <QTimer>

namespace Ui {
class Notification;
}

class Notification : public QDialog
{
    Q_OBJECT

public:
    explicit Notification(QWidget *parent = 0);
    ~Notification();

public slots:
    void showNotification(QString, QString);
    void hideNotification();
signals:
    void terminated();

private:
    Ui::Notification* ui;

    QThread* m_timerThread;
    QTimer* m_timer;
};

#endif // NOTIFICATION_H
