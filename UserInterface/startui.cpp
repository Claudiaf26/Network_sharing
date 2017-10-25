#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QFileDialog>
#include "startui.h"
#include "ui_startui.h"
#include "define.h"

using namespace std;

void StartUI::setIcon(QString str){
    if (m_item != nullptr){
        m_scene->removeItem(m_item);
        delete m_item;
    }
    QImage* image = new QImage(str);
    m_item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    m_scene->addItem(m_item);
}

StartUI::StartUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartUI)
{
    ui->setupUi(this);

    m_scene = new QGraphicsScene();
    ui->iconView->setScene(m_scene);
    m_item = nullptr;

    ui->folderButton->setIcon(QIcon(":/images/icons/Style/FileIcon.png"));
    ui->folderButton->setIconSize(QSize(20, 20));


    QIcon* mainIcon = new QIcon(":/images/tray/Style/TrayIcon.png");
    QAction* settingAction = new QAction("Impostazioni", this);
    QAction* usersAction = new QAction("Utenti connessi", this);
    QAction* quitAction = new QAction("Esci", this);
    QMenu* trayIconMenu = new QMenu(this);
    m_trayIcon = new QSystemTrayIcon(*mainIcon, this);

    trayIconMenu->addAction(settingAction);
    trayIconMenu->addAction(usersAction);
    trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(trayIconMenu);

    QObject::connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    QObject::connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(settingAction, SIGNAL(triggered()), this, SLOT(show()));
    QObject::connect(usersAction, SIGNAL(triggered()), this, SLOT(showList()));
}

StartUI::~StartUI()
{
    delete ui;
    delete m_trayIcon;
    if (m_item != nullptr)
        delete m_item;
    delete m_scene;
}

void StartUI::setUser(QString username, uint8_t imgNmb){
    iconNumber = imgNmb;
    ui->nameButton->setText(username);
    QString imgStr(QString::fromStdString(iconString));
    imgStr.append(49+imgNmb); imgStr.append(".png");
    setIcon(imgStr);

    this->show();
}

void StartUI::changeSettings(uint8_t flg, string user, string icon, string direct){
    ui->nameButton->setText(QString::fromStdString(user));
    ui->folderEdit->setText(QString::fromStdString(direct)); //fai un check se la cartella esiste
    QString imgStr(QString::fromStdString(iconString));
    imgStr.append(QString::fromStdString(icon));
    setIcon(imgStr);
    iconNumber = (uint8_t)(icon[0] - 49);
    bool automaticMode = ((flg & AUTOMATIC_FLAG) != 0) ? true : false;
    bool notificationNoShowMode = ((flg & NOTIFICATION_FLAG) != 0) ? true : false;
    bool privateMode = ((flg & PRIVATE_FLAG) != 0) ? true : false;
    ui->automaticBox->setChecked(automaticMode);
    ui->notificationBox->setChecked(notificationNoShowMode);
    ui->privateBox->setChecked(privateMode);
}

void StartUI::on_startButton_pressed(){
    uint8_t flags = 0;
    if (ui->automaticBox->isChecked())
        flags |= AUTOMATIC_FLAG;
    if (ui->notificationBox->isChecked())
        flags |= NOTIFICATION_FLAG;
    if (ui->privateBox->isChecked())
        flags |= PRIVATE_FLAG;
    QString iconName((char)(iconNumber+49)); iconName.append(".png");
    emit startProgram(flags, ui->nameButton->text().toStdString(), iconName.toStdString(), ui->folderEdit->text().toStdString());
    m_trayIcon->show();
    this->hide();
}

void StartUI::on_folderButton_pressed(){
    QString currDir = ui->folderEdit->text();
    QString newDir = QFileDialog::getExistingDirectory(this, tr("Cartella di download"), currDir);
    if (!newDir.isEmpty())
        ui->folderEdit->setText(newDir);
}

void StartUI::closeEvent (QCloseEvent *event){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "FileSender",
                                                                tr("Sei sicuro di voler uscire?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
        QApplication::exit(0);
    }
}

void StartUI::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    default:
        ;
    }
}
