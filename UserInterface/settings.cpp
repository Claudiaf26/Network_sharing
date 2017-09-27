#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QListWidget>

Settings::Settings(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    mainIcon = new QIcon(":/images/icon/IconFileTransfer.png");
    settingAction = new QAction("Impostazioni", this);
    quitAction = new QAction("Esci", this);
    trayIconMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(*mainIcon, this);

    trayIconMenu->addAction(settingAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);

    QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    QObject::connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    QObject::connect(settingAction, SIGNAL(triggered()), this, SLOT(show()));
}

Settings::~Settings()
{
    delete ui;
    delete mainIcon;
    delete settingAction;
    delete quitAction;
    delete trayIconMenu;
    delete trayIcon;
}

void Settings::on_okButton_pressed(){
    uint8_t flags = 0;
    if (ui->automaticBox->isChecked())
        flags |= AUTOMATIC_FLAG;
    if (ui->notificationBox->isChecked())
        flags |= NOTIFICATION_FLAG;
    if (ui->privateBox->isChecked())
        flags |= PRIVATE_FLAG;
    QString username = ui->usernameEdit->text();
    QString directory = ui->directoryEdit->text();
    emit startProgram(flags, username.toStdString(), directory.toStdString());
    trayIcon->show();
    this->hide();
    ui->okButton->setText("Applica");
}

void Settings::on_fileButton_pressed(){
    QString currDir = ui->directoryEdit->text();
    QString newDir = QFileDialog::getExistingDirectory(this, tr("Cartella di download"), currDir);
    ui->directoryEdit->setText(newDir);
}

void Settings::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    case QSystemTrayIcon::MiddleClick:
        trayIcon->showMessage("Prova", "Testo di prova");
        break;
    default:
        ;
    }
}

void Settings::changeSettings(uint8_t flg, string user, string direct){
    ui->usernameEdit->setText(QString::fromStdString(user));
    ui->directoryEdit->setText(QString::fromStdString(direct)); //fai un check se la cartella esiste
    bool automaticMode = ((flg & AUTOMATIC_FLAG) != 0) ? true : false;
    bool notificationNoShowMode = ((flg & NOTIFICATION_FLAG) != 0) ? true : false;
    bool privateMode = ((flg & PRIVATE_FLAG) != 0) ? true : false;
    ui->automaticBox->setChecked(automaticMode);
    ui->notificationBox->setChecked(notificationNoShowMode);
    ui->privateBox->setChecked(privateMode);
}

void Settings::addUser(User newUser){
    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setText(QString::fromStdString(newUser.name));
    newItem->setData(Qt::UserRole, QString::fromStdString(newUser.ip));
    ui->listWidget->addItem(newItem);
}
void Settings::deleteUser(User deletedUser){
    for(int row = 0; row < ui->listWidget->count(); row++){
        QListWidgetItem* rowItem = ui->listWidget->item(row);
        if ( (rowItem->text() == QString::fromStdString(deletedUser.name) )
           &&(rowItem->data(Qt::UserRole) == QString::fromStdString(deletedUser.ip)) )
                delete ui->listWidget->takeItem(row);
    }
}
