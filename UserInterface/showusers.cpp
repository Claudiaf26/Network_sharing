#include <QTableWidgetItem>
#include <QMessageBox>
#include "showusers.h"
#include "ui_showusers.h"

using namespace std;

ShowUsers::ShowUsers(bool isList, string fileName, QWidget *parent) :
    list(isList),
    userCount(0),
    QWidget(parent),
    ui(new Ui::ShowUsers)
{
    ui->setupUi(this);

    if(list){
        ui->pushButton->hide();
        ui->label->setText(QString::fromStdString(listingString));
        ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    }
    else{
        ui->label->setText(QString::fromStdString(sendingString+fileName));
    }
}

ShowUsers::~ShowUsers()
{
    delete ui;
}


void ShowUsers::addUser(User newUser){
    ui->tableWidget->setRowCount(++userCount);
    QTableWidgetItem *item = new QTableWidgetItem(0);
    QImage* img = new QImage(":/images/icons/Icons/"+QString::fromStdString(newUser.picture));
    item->setData(Qt::DecorationRole, QPixmap::fromImage(*img));
    item->setData(Qt::DisplayRole, QString::fromStdString(newUser.name));
    item->setData(Qt::UserRole, QString::fromStdString(newUser.ip));
    ui->tableWidget->setItem(userCount-1, 0, item);
}
void ShowUsers::deleteUser(User deletedUser){
    for(int row = 0; row < userCount; row++){
        QTableWidgetItem *rowItem = ui->tableWidget->item(row, 0);
        if ( (rowItem->text() == QString::fromStdString(deletedUser.name) )
           &&(rowItem->data(Qt::UserRole) == QString::fromStdString(deletedUser.ip)) ){
                ui->tableWidget->removeRow(row);
                break;
        }
    }
    userCount--;
}

void ShowUsers::createList(vector<User> userList){
    for (auto it = userList.begin(); it != userList.end(); it++)
        addUser(*it);
}

void ShowUsers::on_pushButton_pressed(){
    vector<User> selectedUsers;
    foreach (QTableWidgetItem* item, ui->tableWidget->selectedItems()){
        User newUser;
        newUser.name = item->data(Qt::DisplayRole).toString().toUtf8().constData();
        newUser.ip = item->data(Qt::UserRole).toString().toUtf8().constData();
        newUser.picture = item->data(Qt::DecorationRole).toString().toUtf8().constData();
        selectedUsers.push_back(newUser);
    }
    if (selectedUsers.empty())
        this->close();
    else{
        this->hide();
        emit sendToUsers(selectedUsers);
    }
}

void ShowUsers::closeEvent (QCloseEvent *event){
    if (!list){
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "FileSender",
                                                                    tr("Sei sicuro di voler chiudere senza inviare?\n"),
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
            QApplication::exit(0);
        }
    }
    else
        event->accept();
}
