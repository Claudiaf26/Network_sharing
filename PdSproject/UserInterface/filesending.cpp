#include "filesending.h"
#include "ui_filesending.h"

using namespace std;

FileSending::FileSending(QString path, vector<User> userList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSending)
{
    ui->setupUi(this);
    ui->fileLabel->setText(path);
    if(!userList.empty())
        for (auto it = userList.begin(); it != userList.end(); it++){
            QListWidgetItem *newItem = new QListWidgetItem();
            newItem->setText(QString::fromStdString((*it).name));
            newItem->setData(Qt::UserRole, QString::fromStdString((*it).ip));
            ui->listWidget->addItem(newItem);
        }
}

FileSending::~FileSending()
{
    delete ui;
}

void FileSending::on_sendButton_pressed(){
    vector<User> selectedUsers;
    foreach (QListWidgetItem* item, ui->listWidget->selectedItems()){
        User newUser;
        newUser.name = item->text().toUtf8().constData();
        newUser.ip = item->data(Qt::UserRole).toString().toUtf8().constData();
        selectedUsers.push_back(newUser);
    }
    emit sendToUsers(selectedUsers);
}
