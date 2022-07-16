#include "systeminfodlg.h"
#include "ui_systeminfodlg.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>

SystemInfoDlg::SystemInfoDlg(QWidget *parent, QVariantHash d, EditorMode m) :
    EditorBase(parent, d, m),
    ui(new Ui::systemInfoDlg)
{
    ui->setupUi(this);
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
                emit editorDataChanged();
            }
        }
    }
    close();
}

