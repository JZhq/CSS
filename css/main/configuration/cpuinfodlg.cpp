#include "cpuinfodlg.h"
#include "ui_cpuinfodlg.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

CpuInfoDlg::CpuInfoDlg(QWidget *parent, QVariantHash d, EditorMode m)
    : EditorBase{parent,  d, m},
      ui(new Ui::CpuInfoDlg)
{
    ui->setupUi(this);
    if (m == Update){
        m_oldName = d.value("cpuname").toString();
        ui->lineEdit_cpu->setText(d.value("cpuname").toString());
        ui->lineEdit_desc->setText(d.value("cpudesc").toString());
    }
}

CpuInfoDlg::~CpuInfoDlg()
{
    delete ui;
}

void CpuInfoDlg::on_result(bool state, const QString &respons)
{
    qDebug() << state << respons;
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            if (isLogin){
                QVariantHash cpuHash= {{"cpuname", ui->lineEdit_cpu->text()},
                                       {"cpudesc", ui->lineEdit_desc->text()}};
                qDebug() << cpuHash;
                if (m_mode == New)
                    emit addCpuInfo(cpuHash);
                if (m_mode == Update)
                    emit editorDataUpdated(cpuHash);
                emit editorDataChanged();
            }
        }
    }
    else{
        close();
    }
}

void CpuInfoDlg::on_pushButton_confirm_clicked()
{
    QString cpuInfo = ui->lineEdit_cpu->text();
    QString decInfo = ui->lineEdit_desc->text();
    if (m_mode == New)
        m_httpClient->cpuConfigAdd(cpuInfo,  decInfo);
    if (m_mode == Update)
        m_httpClient->cpuConfigUpdate(m_oldName, cpuInfo, decInfo);
}


void CpuInfoDlg::on_pushButton_cancel_clicked()
{
    close();
}

