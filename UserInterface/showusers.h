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

class ShowUsers : public QWidget
{
    Q_OBJECT

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

private:
    Ui::ShowUsers *ui;
    uint16_t userCount;
    bool list;

    const std::string sendingString = "Seleziona gli utenti a cui inviare il file\n";
    const std::string listingString = "Gli utenti attualmente connessi sono";
};

#endif // SHOWUSERS_H
