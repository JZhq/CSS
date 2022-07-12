#include "systeminfodlg.h"
#include "ui_systeminfodlg.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>

SystemInfoDlg::SystemInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::systemInfoDlg),
    m_httpClient(NULL)
{
    ui->setupUi(this);
    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)), this);
}

SystemInfoDlg::~SystemInfoDlg()
{
    delete ui;
}

void SystemInfoDlg::on_pushButton_confirm_clicked()
{
    QString name = ui->lineEdit_name->text();
    QString version = ui->lineEdit_version->text();
    QString desc = ui->lineEdit_desc->text();

    m_httpClient->opSystemConfigAdd(name, version, desc);
}


void SystemInfoDlg::on_pushButton_2_clicked()
{
    close();
}

void SystemInfoDlg::on_result(bool state, const QString &respons)
{
    qDebug() << state << respons;
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            if (isLogin){
                QVariantHash cpuHash= {{"systemver", ui->lineEdit_version->text()},
                                       {"systemname", ui->lineEdit_name->text()},
                                       {"systemdesc", ui->lineEdit_desc->text()}};
                emit addSystemInfo(cpuHash);
            }
        }
    }
    close();
}

