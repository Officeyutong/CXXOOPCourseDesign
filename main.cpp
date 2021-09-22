#include <QApplication>
#include <QDebug>
#include "GeneralDataManager.h"
#include "mainwindow.h"
int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    qDebug() << (sizeof w) << " " << (sizeof a);
    w.show();
    return a.exec();
}
