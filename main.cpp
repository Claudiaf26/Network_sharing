//#include "settings.h"
#include "mainprogram.h"
#include "sendingprogram.h"
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include <string>
#include <algorithm>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    if (argc < 2){
        MainProgram m;
        m.start();
    }
    else
      SendingProgram s(argv[1]);

    return a.exec();
}
