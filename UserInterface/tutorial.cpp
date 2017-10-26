#include "tutorial.h"
#include "ui_tutorial.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

Tutorial::Tutorial(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tutorial)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowSystemMenuHint | Qt::MSWindowsFixedSizeDialogHint);

    QImage* image1 = new QImage(":/images/tutorial/Tutorial/Tutorial02.png");
    QGraphicsPixmapItem* item1 = new QGraphicsPixmapItem(QPixmap::fromImage(*image1));
    QGraphicsScene* scene1 = new QGraphicsScene();
    scene1->addItem(item1);

    QImage* image2 = new QImage(":/images/tutorial/Tutorial/Tutorial01.png");
    QGraphicsPixmapItem* item2 = new QGraphicsPixmapItem(QPixmap::fromImage(*image2));
    QGraphicsScene* scene2 = new QGraphicsScene();
    scene2->addItem(item2);

    ui->graphicsView->setScene(scene2);
    ui->graphicsView_2->setScene(scene1);
}

Tutorial::~Tutorial()
{
    delete ui;
}
