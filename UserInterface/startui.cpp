#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QFont>
#include "startui.h"
#include "ui_startui.h"
#include "define.h"

using namespace std;

void StartUI::setIcon(const QString& t_string){
    if (m_item != nullptr){
        m_scene->removeItem(m_item);
        delete m_item;
    }
    QImage* image = new QImage(t_string);
    m_item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    m_scene->addItem(m_item);
}

void StartUI::createSystemTray(){
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

    QObject::connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(settingAction, SIGNAL(triggered()), this, SLOT(show()));
    QObject::connect(usersAction, SIGNAL(triggered()), this, SLOT(showList()));
}

StartUI::StartUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartUI)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    m_scene = new QGraphicsScene();
    ui->iconView->setScene(m_scene);
    m_item = nullptr;

    ui->folderButton->setIcon(QIcon(":/images/icons/Style/FileIcon.png"));
    ui->folderButton->setIconSize(QSize(20, 20));

    createSystemTray();
}

StartUI::~StartUI()
{
    delete ui;
    delete m_trayIcon;
    safeDelete(m_item);
    delete m_scene;
}

void StartUI::setUser(QString t_username, QString t_pictureString){
    m_iconName = t_pictureString.toStdString();
    ui->nameButton->setText(t_username);
    QString imgStr(QString::fromStdString(iconString));
    imgStr.append(t_pictureString);
    setIcon(imgStr);

    this->show();
}

void StartUI::changeSettings(uint8_t t_flags, string t_username, string t_icon, string t_directory){
    ui->nameButton->setText(QString::fromStdString(t_username));
    ui->folderEdit->setText(QString::fromStdString(t_directory)); //fai un check se la cartella esiste
    QString imgStr(QString::fromStdString(iconString));
    imgStr.append(QString::fromStdString(t_icon));
    setIcon(imgStr);
    m_iconName = t_icon;
    bool automaticMode = ((t_flags & AUTOMATIC_FLAG) != 0) ? true : false;
    bool notificationNoShowMode = ((t_flags & NOTIFICATION_FLAG) != 0) ? true : false;
    bool privateMode = ((t_flags & PRIVATE_FLAG) != 0) ? true : false;
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
    emit startProgram(flags, ui->nameButton->text().toStdString(), m_iconName, ui->folderEdit->text().toStdString());
    m_trayIcon->show();
    this->hide();
}

void StartUI::on_folderButton_pressed(){
    QString currDir = ui->folderEdit->text();
    QString newDir = QFileDialog::getExistingDirectory(this, tr("Cartella di download"), currDir);
    if (!newDir.isEmpty())
        ui->folderEdit->setText(newDir);
}

void StartUI::closeEvent (QCloseEvent* event){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "FileSender",
                                                                "Sei sicuro di voler uscire?\n",
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);

    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
        QApplication::exit(0);
    }
}
