#include <QTableWidgetItem>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QCloseEvent>
#include "userselection.h"
#include "ui_userselection.h"

using namespace std;

UserSelection::UserSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserSelection)
{
    ui->setupUi(this);

    for (int i = 0; i < 3; ++i){
        for (int j = 0; j < 3; ++j){
            if ( (i != 2) || (j != 2)){
                QTableWidgetItem *item=new QTableWidgetItem(0);
                char imgNmb = (char)(i*3+j+49);
                QString imgStr(":/images/icons/Icons/");
                imgStr.append(imgNmb); imgStr.append(".png");
                QImage* img = new QImage(imgStr);
                item->setData(Qt::DecorationRole, QPixmap::fromImage(*img));
                ui->iconTable->setItem(i, j, item);
            }
        }
    }
}

UserSelection::~UserSelection()
{
    delete ui;
}

void UserSelection::on_selectButton_pressed(){
    if (ui->userEdit->text().isEmpty() || (ui->iconTable->selectedItems().size() == 0) )
        emit error("Seleziona un nome utente e un'icona prima di continuare!");
    else{
        this->hide();
        uint8_t selectedNmb = ui->iconTable->currentRow() * 3 + ui->iconTable->currentColumn();
        emit changeUser(ui->userEdit->text(), selectedNmb);
    }
}

void UserSelection::showOnTop(){
    this->setWindowTitle("Scegli l'username e l'icona");
    this->show();
    this->setWindowModality(Qt::ApplicationModal);
}

void UserSelection::changeSettings(uint8_t flags, string username, string icon, string directory){
    ui->userEdit->setText(QString::fromStdString(username));
}

void UserSelection::closeEvent (QCloseEvent *event){
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
