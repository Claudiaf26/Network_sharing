#ifndef FILESENDING_H
#define FILESENDING_H

#include "define.h"
#include <QDialog>
#include <vector>

namespace Ui {
class FileSending;
}

class FileSending : public QDialog
{
    Q_OBJECT

public:
    explicit FileSending(QString, std::vector<User>, QWidget *parent = 0);
    ~FileSending();

public slots:
    void on_sendButton_pressed();

signals:
    void sendToUsers(const std::vector<User>&);

private:
    Ui::FileSending *ui;
};

#endif // FILESENDING_H
