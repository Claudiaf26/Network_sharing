//#include "settings.h"
#include "mainprogram.h"
#include "sendingprogram.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
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
