#include "notification.h"
#include "ui_notification.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDesktopWidget>

Notification::Notification(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Notification)
{
    ui->setupUi(this);

    setModal(false);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    QRect screenSize = QApplication::desktop()->availableGeometry();
    int w = screenSize.width()/8; int h = screenSize.height()/8;
    w = (w < 77) ? 77 : w; h = (h < 77) ? 77 : h;
    setFixedSize(w,h);
    move(w*7,h*7-28);

    timerThread = new QThread(this);
    timer = new QTimer(0);
    timer->setInterval(5000);
    timer->moveToThread(timerThread);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hideNotification()));
    QObject::connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
}

Notification::~Notification()
{
    delete ui;
    timerThread->quit();
    timerThread->wait();
}

void Notification::showNotification(QString text, QString picture) {
    if(!isVisible()){
        ui->userLabel->setText(text);
        QGraphicsScene* scene = new QGraphicsScene();
        ui->iconView->setScene(scene);
        QImage* image = new QImage(picture);
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
        scene->addItem(item);
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
