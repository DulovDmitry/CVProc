#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QSplashScreen *splash = new QSplashScreen();
//    splash->setPixmap(QPixmap(":/new/prefix1/splashScreen"));
//    splash->show();

//    Qt::Alignment top_right = Qt::AlignRight | Qt::AlignTop;
//    splash->showMessage("Setting up the main window ...", top_right, Qt::white);

    MainWindow w;
    w.show();

//    splash->finish(&w);
//    delete splash;
    return a.exec();
}
