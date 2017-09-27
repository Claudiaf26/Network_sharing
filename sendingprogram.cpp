#include "sendingprogram.h"
#include "DataStruct/usersingleton.h"
#include <string>

using namespace std;

SendingProgram::SendingProgram(const char* path, QObject *parent) : QObject(parent){
    string filePath = path;
    if (!UserSingleton::get_instance().open())
        exit(EXIT_FAILURE);
    userVect = UserSingleton::get_instance().getList();
    chooseDestUI = new FileSending(QString::fromStdString(filePath), userVect);
    transfer = new SenderManager(filePath);
    QObject::connect(chooseDestUI, SIGNAL(sendToUsers(const std::vector<User>&)), transfer, SLOT(sendToUsers(const std::vector<User>&)));
    chooseDestUI->show();
}
