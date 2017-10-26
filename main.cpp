//#include "settings.h"
#include "mainprogram.h"
#include "sendingprogram.h"
#include "UserInterface/showusers.h"
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include <string>
#include <algorithm>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    QIcon mainIcon(":/images/icons/Style/SystemIcon.png");
    a.setWindowIcon(mainIcon);
        if (argc < 2){
            MainProgram m;
            m.start();
            return a.exec();
        }
        else{
            SendingProgram s(argv[1]);
            return a.exec();
     }
}
