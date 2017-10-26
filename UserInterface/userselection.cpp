#include <QTableWidgetItem>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QCloseEvent>
#include "userselection.h"
#include "ui_userselection.h"
#include "define.h"

using namespace std;

UserSelection::UserSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserSelection)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    for (int i = 0; i < 5; ++i){
        for (int j = 0; j < 3; ++j){
            QTableWidgetItem *item=new QTableWidgetItem(0);
            QString imgStr(QString::fromStdString(iconString));
            imgStr.append(QString::number(i*3+j+1)); imgStr.append(".png");
            QImage* img = new QImage(imgStr);
            item->setData(Qt::DecorationRole, QPixmap::fromImage(*img));
            ui->iconTable->setItem(i, j, item);
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
        uint8_t selectedNmb = ui->iconTable->currentRow() * 3 + ui->iconTable->currentColumn() +1;
        QString selectedStr = QString::number(selectedNmb); selectedStr.append(".png");
        emit changeUser(ui->userEdit->text(), selectedStr);
    }
}

void UserSelection::showOnTop(){
    this->setWindowTitle("Scegli l'username e l'icona");
    this->show();
    if (!this->isModal())
        this->setWindowModality(Qt::ApplicationModal);
}

void UserSelection::changeSettings(uint8_t t_flags, string t_username, string t_icon, string t_directory){
    ui->userEdit->setText(QString::fromStdString(t_username));
    int iconNumber = stoi(t_icon.substr(0, t_icon.find(".png"))) - 1;
    ui->iconTable->setCurrentCell(iconNumber / 3, iconNumber % 3);
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
