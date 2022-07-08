#include <QEventLoop>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include "css_messagebox.h"

CSSMessageBox::CSSMessageBox(const QString &title, const QString &message, QObject *parent) : QObject(parent)
                                ,m_title(title),
                                 m_message(message)
{

}

void CSSMessageBox::show(const QString &title, const QString &message)
{
    QEventLoop eventLoop;
    auto messageBox = new CSSMessageBox(title,message);
    connect(messageBox,SIGNAL(destroyed()),&eventLoop,SLOT(quit()));
    messageBox->readyShow();
    eventLoop.exec();
}

void CSSMessageBox::onShow()
{
    QMessageBox::information(NULL,m_title,m_message);
    this->deleteLater();
}

void CSSMessageBox::readyShow()
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0,this,SLOT(onShow()));
}
