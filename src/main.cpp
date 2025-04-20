#include "MainThread.h"
#include <QApplication>
#include "mainwindow.h"  // 来自 RobotGUI
#include <thread> 

#include "record.h"

int main(int argc, char *argv[]) {


    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    // 启动后端线程，但不阻塞主线程
    //std::thread cmdThread(startMainThread);
    //cmdThread.detach();

    return a.exec();
}