#include "MainThread.h"
#include <QApplication>
#include "mainwindow.h"  // RobotGUI
#include <thread> 

#include "record.h"

int main(int argc, char *argv[]) {


    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    //std::thread cmdThread(startMainThread);
    //cmdThread.detach();

    return a.exec();
}