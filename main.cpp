#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Screen Recorder Eccezionale");    
    w.show();

    //QObject::connect(q,SIGNAL(),&app, SLOT());

    return a.exec();
}
