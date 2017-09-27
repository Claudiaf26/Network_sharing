#include "notification.h"
#include <QApplication>
#include <QDesktopWidget>


Notification::Notification(QWidget *parent)
    : QDialog(parent){
    setModal(false);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    QRect screenSize = QApplication::desktop()->availableGeometry();
    int w = screenSize.width()/9; int h = screenSize.height()/9;
    setFixedSize(w,h);
    move(w*8,h*8-28);
    dialogLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    testo = new QLabel(this);
    dialogLayout->addWidget(testo, 0, Qt::AlignHCenter);
    setLayout(dialogLayout);
    timerThread = new QThread(this);
    timer = new QTimer(0);
    timer->setInterval(5000);
    timer->moveToThread(timerThread);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hideNotification()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
}

Notification::~Notification() {
    delete dialogLayout;
    delete testo;
    timerThread->quit();
    timerThread->wait();
}

void Notification::showNotification(QString text) {
    if(!isVisible()){
        testo->setText(text);
        show();

        timerThread->start();
    }
}

void Notification::hideNotification(){
    if(isVisible()){
        hide();

        emit terminated();
    }
}
