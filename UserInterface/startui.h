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

//la principale UI con cui si interfaccia l'utente per settare le impostazioni
class StartUI : public QWidget
{
    Q_OBJECT
private:
    Ui::StartUI* ui;

    QSystemTrayIcon* m_trayIcon;

    QGraphicsScene* m_scene;
    QGraphicsPixmapItem* m_item;
    std::string m_iconName;

    //funzioni ausiliarie
    void createSystemTray();
    void setIcon(const QString&);

public:
    explicit StartUI(QWidget *parent = 0);
    ~StartUI();
    void closeEvent(QCloseEvent*);

public slots:
    void setUser(QString, QString);
    void on_nameButton_pressed(){emit userChoice();}
    void on_startButton_pressed();
    void on_folderButton_pressed();
    void on_helpButton_pressed(){emit helpRequested();}
    void changeSettings(uint8_t, std::string, std::string, std::string);
    void showList(){emit showUserList();}

signals:
    void userChoice();
    void startProgram(uint8_t, std::string, std::string, std::string);
    void showUserList();
    void helpRequested();
};
#endif // STARTUI_H
