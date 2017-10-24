#ifndef SENDINGPROGRAM_H
#define SENDINGPROGRAM_H

#include "define.h"
#include "UserInterface/showusers.h"
#include "Managers/sendermanager.h"
#include <QObject>
#include <vector>

class SendingProgram : public QObject
{
    Q_OBJECT
private:
    std::vector<User> userVect;

    ShowUsers* chooseDestUI;
    SenderManager* transfer;

public:
    explicit SendingProgram(const char* ,QObject *parent = 0);

public slots:
    void showError(QString);
};

#endif // SENDINGPROGRAM_H
