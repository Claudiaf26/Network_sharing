#include <QTableWidgetItem>
#include <QMessageBox>
#include "showusers.h"
#include "ui_showusers.h"

using namespace std;

ShowUsers::ShowUsers(bool t_isList, string t_fileName, QWidget *parent) :
    m_isList(t_isList),
    m_userCount(0),
    QWidget(parent),
    ui(new Ui::ShowUsers)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    if(m_isList){
        ui->pushButton->hide();
        ui->label->setText(QString::fromStdString(m_listingString));
        ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    }
    else{
        this->setWindowTitle("Stai inviando un file...");
        ui->label->setText(QString::fromStdString(m_sendingString+t_fileName));
    }
}

ShowUsers::~ShowUsers()
{
    delete ui;
}


void ShowUsers::addUser(User t_newUser){
    ui->tableWidget->setRowCount(++m_userCount);
    QTableWidgetItem *item = new QTableWidgetItem(0);
    QImage* img = new QImage(":/images/icons/Icons/"+QString::fromStdString(t_newUser.picture));
    item->setData(Qt::DecorationRole, QPixmap::fromImage(*img));
    item->setData(Qt::DisplayRole, QString::fromStdString(t_newUser.name));
    item->setData(Qt::UserRole, QString::fromStdString(t_newUser.ip));
    ui->tableWidget->setItem(m_userCount-1, 0, item);
}
void ShowUsers::deleteUser(User t_deletedUser){
    for(int row = 0; row < m_userCount; ++row){
        QTableWidgetItem *rowItem = ui->tableWidget->item(row, 0);
        if ( (rowItem->text() == QString::fromStdString(t_deletedUser.name) )
           &&(rowItem->data(Qt::UserRole) == QString::fromStdString(t_deletedUser.ip)) ){
                ui->tableWidget->removeRow(row);
                break;
        }
    }
    m_userCount--;
}

void ShowUsers::createList(vector<User> t_userList){
    for (auto it = t_userList.begin(); it != t_userList.end(); ++it)
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
    if (!m_isList){
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
