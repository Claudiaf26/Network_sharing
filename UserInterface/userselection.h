#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QWidget>
#include <QString>
#include <string>

namespace Ui {
class UserSelection;
}

//UI con cui si selezionano nome utente e icona
class UserSelection : public QWidget
{
    Q_OBJECT
private:
    Ui::UserSelection* ui;

public:
    explicit UserSelection(QWidget *parent = 0);
    ~UserSelection();
    void closeEvent(QCloseEvent*);

public slots:
    void on_selectButton_pressed();
    void showOnTop();
    void changeSettings(uint8_t, std::string, std::string, std::string);

signals:
    void changeUser(QString, QString);
    void error(QString);
};

#endif // USERSELECTION_H
