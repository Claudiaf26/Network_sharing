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
    m_userVector = UserSingleton::get_instance().getList();
    m_chooseDestinationUI = new ShowUsers(false, path);
    m_fileTransfering = new SenderManager(filePath);
    QObject::connect(m_chooseDestinationUI, SIGNAL(sendToUsers(std::vector<User>)), m_fileTransfering, SLOT(sendToUsers(std::vector<User>)));
    QObject::connect(m_fileTransfering, SIGNAL(error(QString)), this, SLOT(showError(QString)), Qt::DirectConnection);
    m_chooseDestinationUI->createList(m_userVector);
    m_chooseDestinationUI->show();
}

void SendingProgram::showError(QString errorText){
    QMessageBox errorBox;
    errorBox.setText(errorText);
    errorBox.exec();
}
