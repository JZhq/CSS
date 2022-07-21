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

    if (m == Update){
        m_oldCpuName = d.value("systemname").toString();
        ui->lineEdit_name->setText(m_oldCpuName);
        ui->lineEdit_version->setText(d.value("systemver").toString());
        ui->lineEdit_desc->setText(d.value("systemdesc").toString());
    }
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
    if (m_mode == New)
        m_httpClient->opSystemConfigAdd(name, version, desc);
    if (m_mode == Update)
        m_httpClient->cpuConfigUpdate(m_oldCpuName, name, desc);
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
                if (m_mode == New)
                    emit addSystemInfo(cpuHash);
                if (m_mode == Update)
                    emit editorDataUpdated(cpuHash);
                emit editorDataChanged();
            }
        }
    }
    close();
}

