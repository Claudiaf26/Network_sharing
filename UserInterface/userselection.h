#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QWidget>
#include <Qstring>
#include <string>

namespace Ui {
class UserSelection;
}

class UserSelection : public QWidget
{
    Q_OBJECT

public:
    explicit UserSelection(QWidget *parent = 0);
    ~UserSelection();
    void closeEvent(QCloseEvent*);

public slots:
    void on_selectButton_pressed();
    void showOnTop();
    void changeSettings(uint8_t, std::string, std::string, std::string);

signals:
    void changeUser(QString, uint8_t);
    void error(QString);

private:
    Ui::UserSelection* ui;
};

#endif // USERSELECTION_H
