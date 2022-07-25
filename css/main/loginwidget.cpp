#include "loginwidget.h"
#include "ui_loginwidget.h"

#include <QDebug>
#include <qcryptographichash.h>
#include <json.h>
#include <QJsonDocument>
#include <QJsonParseError>
#include "configurationglobal.h"


LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget),
    m_httpClient(NULL)
{
    ui->setupUi(this);
    m_httpClient = new HttpClient(ConfigGlobalInterface->serverAddr(), 8080, this, SLOT(on_loginResult(bool, const QString&)));
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_pushButton_confirm_clicked()
{
    auto user = ui->lineEdit_user->text();
    auto pwd = ui->lineEdit_pwd->text();

    QCryptographicHash md(QCryptographicHash::Md5);
    QByteArray _data = pwd.toLatin1();
    md.addData(_data, _data.size());
    //  qDebug() << md.result().toHex();
    m_httpClient->userLogin(user, md.result().toHex());
}


void LoginWidget::on_pushButton_cancel_clicked()
{
    exit(0);
}

void LoginWidget::on_loginResult(bool state, const QString &respons)
{
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            qDebug() << respons << isLogin;
        }
        emit loginChanged(isLogin);
    }
}

