#include "sendingprogram.h"
#include "DataStruct/usersingleton.h"
#include "define.h"
#include <string>
#include <cstdlib>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QApplication>

using namespace std;

SendingProgram::SendingProgram(const char* t_path, QObject *parent) : QObject(parent){
    m_chooseDestinationUI = nullptr;
    m_fileTransfering = nullptr;

    if (!UserSingleton::get_instance().open()){
        showError("ERRORE GRAVE: Impossibile aprire la memoria condivisa!");
        exit(EXIT_FAILURE);
    }

    QFile f(":/style/better/Style/better.stylesheet");
    if (!f.exists()) {
        showError("Impossibile settare lo stile, il file non esiste");
    }
    else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }


    try{
        m_userVector = UserSingleton::get_instance().getList();
    }catch(runtime_error error){
        showError("Errore nell'apertura della lista utenti condivisa,\nil programma verrà chiuso");
        exit(EXIT_FAILURE);
    }

    m_chooseDestinationUI = new ShowUsers(false, t_path);
    wstring filePath = convertCharToWString(t_path);
    m_fileTransfering = new SenderManager(filePath);

    QObject::connect(m_chooseDestinationUI, SIGNAL(sendToUsers(std::vector<User>)), m_fileTransfering, SLOT(sendToUsers(std::vector<User>)));
    QObject::connect(m_fileTransfering, SIGNAL(error(QString)), this, SLOT(showError(QString)), Qt::DirectConnection);

    m_chooseDestinationUI->createList(m_userVector);
    m_chooseDestinationUI->show();
}

SendingProgram::~SendingProgram(){
    safeDelete(m_chooseDestinationUI);
    safeDelete(m_fileTransfering);
}

void SendingProgram::showError(QString errorText){
    QMessageBox errorBox;
    errorBox.setText(errorText);
    errorBox.exec();
}
//converte la stringa di caratteri in wstring
wstring SendingProgram::convertCharToWString(const char* t_charString){
    const size_t stringSize = strlen(t_charString);
    wstring returnWString;
    if (stringSize > 0){
        returnWString.resize(stringSize);
        mbstowcs(&returnWString[0], t_charString, stringSize);
    }
    return returnWString;
}
