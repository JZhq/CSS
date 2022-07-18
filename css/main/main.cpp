#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDebug>
#include "popupwindow.h"
#include "loginwidget.h"
#include "QObject"

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

    LoginWidget *login = new  LoginWidget();
    QObject::connect(login, &LoginWidget::loginChanged, [=](bool isLogin){
        if (isLogin){
            MainWindow *w = new MainWindow;
            PopupWindow::show(w, QString(), QStringLiteral("组合配置系统"), true, true, true, true, false);
        }
        login->deleteLater();
    });
    login->show();

    return a.exec();
}
