#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDebug>
#include "popupwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QString qss;
//    QFile file(qApp->applicationDirPath() +"/" + "qss/common.qss");
//    if (!file.open(QIODevice::ReadOnly)) {
//        qDebug() << "load qss file failed";
//    }
//    qss.append(file.readAll()).append("\n");
//    file.close();

//    if (!qss.isEmpty()) {
//        qApp->setStyleSheet(qss);
//    }

    MainWindow w;
    PopupWindow::show(&w, QString(), QStringLiteral("组合配置系统"), true, true, true, true, false);

    return a.exec();
}
