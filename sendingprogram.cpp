#include "sendingprogram.h"
#include "DataStruct/usersingleton.h"
#include <string>
#include <cstdlib>

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
    chooseDestUI = new FileSending(QString::fromStdWString(filePath), userVect);
    transfer = new SenderManager(filePath);
    QObject::connect(chooseDestUI, SIGNAL(sendToUsers(std::vector<User>)), transfer, SLOT(sendToUsers(std::vector<User>)));
    chooseDestUI->show();
}
