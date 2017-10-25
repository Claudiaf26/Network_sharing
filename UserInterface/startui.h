#ifndef STARTUI_H
#define STARTUI_H

#include <QWidget>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSystemTrayIcon>
#include <string>

namespace Ui {
class StartUI;
}

class StartUI : public QWidget
{
    Q_OBJECT

public:
    explicit StartUI(QWidget *parent = 0);
    ~StartUI();
    void closeEvent(QCloseEvent*);
    uint8_t getIconNumber(){return iconNumber;}

public slots:
    void setUser(QString, uint8_t);
    void on_nameButton_pressed(){emit userChoice();}
    void on_startButton_pressed();
    void on_folderButton_pressed();
    void changeSettings(uint8_t, std::string, std::string, std::string);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showList(){emit showUserList();}

signals:
    void userChoice();
    void startProgram(uint8_t, std::string, std::string, std::string);
    void showUserList();

private:
    Ui::StartUI* ui;

    QSystemTrayIcon* m_trayIcon;

    QGraphicsScene* m_scene;
    QGraphicsPixmapItem* m_item;
    uint8_t iconNumber;

    void setIcon(QString);
};
#endif // STARTUI_H
