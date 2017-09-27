#ifndef SETTINGS_H
#define SETTINGS_H

#include "define.h"
#include <QWidget>
#include <string>
#include <QSystemTrayIcon>
#include <QMenu>

using namespace std;


namespace Ui {
class Settings;
}

class Settings : public QWidget {
    Q_OBJECT

public:
    explicit Settings(QWidget* parent = 0);
    ~Settings();

public slots:
    void on_okButton_pressed();
    void on_fileButton_pressed();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void changeSettings(uint8_t, string, string);
    void addUser(User);
    void deleteUser(User);

signals:
    void startProgram(uint8_t, string, string);
private:
    Ui::Settings *ui;

    QAction* settingAction;
    QAction* quitAction;
    QMenu* trayIconMenu;

    QIcon* mainIcon;
    QSystemTrayIcon* trayIcon;
};

#endif // SETTINGS_H
