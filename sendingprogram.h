#ifndef SENDINGPROGRAM_H
#define SENDINGPROGRAM_H

#include "define.h"
#include "UserInterface/showusers.h"
#include "Managers/sendermanager.h"
#include <QObject>
#include <vector>

//classe che gestisce l'invio di un file selezionato attraverso il menù a tendina del click destro
class SendingProgram : public QObject
{
    Q_OBJECT
private:
    std::vector<User> m_userVector;     //vettore degli utenti connessi al momento del click

    ShowUsers* m_chooseDestinationUI;   //ui
    SenderManager* m_fileTransfering;   //gestisce l'invio del file

    //funzioni ausiliarie
    wstring convertCharToWString(const char*);
public:
    explicit SendingProgram(const char* ,QObject* parent = 0);
    ~SendingProgram();

public slots:
    void showError(QString);
};

#endif // SENDINGPROGRAM_H
