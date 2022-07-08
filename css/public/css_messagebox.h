/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         适用于在非gui线程中使用的MessageBox
*@author        luox
*@date          2018-06-21
**/
#ifndef CSS_MESSAGEBOX_H
#define CSS_MESSAGEBOX_H

#include <QObject>
#include "public_global.h"

class GLOBAL_EXPORT CSSMessageBox : public QObject
{
    Q_OBJECT

public:
    explicit CSSMessageBox(const QString &title,const QString &message,QObject *parent = nullptr);
    static void show(const QString &title,const QString &message);

public slots:
    void onShow();

private:
    void readyShow();
private:
    const QString m_title;
    const QString m_message;
};

#endif // CSS_MESSAGEBOX_H
