#include "cpuinfodlg.h"
#include "ui_cpuinfodlg.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

CpuInfoDlg::CpuInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuInfoDlg),
    m_httpClient(NULL)
{
    ui->setupUi(this);
    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)), this);
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
                emit addCpuInfo(cpuHash);
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
    m_httpClient->cpuConfigAdd(cpuInfo,  decInfo);
}


void CpuInfoDlg::on_pushButton_cancel_clicked()
{
    close();
}

