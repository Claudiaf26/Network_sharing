#ifndef SHOWUSERS_H
#define SHOWUSERS_H

#include "define.h"
#include <QWidget>
#include <QCloseEvent>
#include <string>
#include <vector>

namespace Ui {
class ShowUsers;
}

//UI che mostra la lista di utenti connessi, sia a scopo informativo che per selezionare a chi inviare un file in seguito
class ShowUsers : public QWidget
{
    Q_OBJECT
private:
    Ui::ShowUsers* ui;
    uint16_t m_userCount;
    bool m_isList;

    const std::string m_sendingString = "Seleziona gli utenti a cui inviare il file\n";
    const std::string m_listingString = "Gli utenti attualmente connessi sono";

public:
    explicit ShowUsers(bool, std::string, QWidget *parent = 0);
    ~ShowUsers();

    void createList(std::vector<User>);

public slots:
    void addUser(User);
    void deleteUser(User);
    void on_pushButton_pressed();

    void closeEvent(QCloseEvent*);
signals:
    void sendToUsers(const std::vector<User>&);
};

#endif // SHOWUSERS_H
