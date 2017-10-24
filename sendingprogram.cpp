#include "sendingprogram.h"
#include "DataStruct/usersingleton.h"
#include <string>
#include <cstdlib>
#include <QMessageBox>

using namespace std;

SendingProgram::SendingProgram(const char* path, QObject *parent) : QObject(parent){
    //converte la stringa di caratteri in wstring
    const size_t pathSize = strlen(path);
    wstring filePath;
    if (pathSize > 0){
        filePath.resize(pathSize);
        mbstowcs(&filePath[0], path, pathSize);
    }

    if (!UserSingleton::get_instance().open())
        exit(EXIT_FAILURE);
    userVect = UserSingleton::get_instance().getList();
    chooseDestUI = new ShowUsers(false, path);
    transfer = new SenderManager(filePath);
    QObject::connect(chooseDestUI, SIGNAL(sendToUsers(std::vector<User>)), transfer, SLOT(sendToUsers(std::vector<User>)));
    QObject::connect(transfer, SIGNAL(error(QString)), this, SLOT(showError(QString)), Qt::DirectConnection);
    chooseDestUI->createList(userVect);
    chooseDestUI->show();
}

void SendingProgram::showError(QString errorText){
    QMessageBox errorBox;
    errorBox.setText(errorText);
    errorBox.exec();
}
