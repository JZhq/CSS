#include "moduleinfodlg.h"
#include "ui_moduleinfodlg.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>

ModuleInfoDlg::ModuleInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModuleInfoDlg),
    m_httpClient(NULL)
{
    ui->setupUi(this);
    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)), this);
    m_httpClient->cpuConfigQueryList();
    m_httpClient->opSystemConfigQueryList();
}

ModuleInfoDlg::~ModuleInfoDlg()
{
    delete ui;
}

void ModuleInfoDlg::on_result(bool state, const QString &respons)
{
    qDebug() << state << respons;
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            auto resultSet = doc.object().value("resultset").toVariant().toList();
            QList<QVariantHash> results;
            QVariantHash hash;
            for(auto set : resultSet){
                results << set.toHash();
                hash = set.toHash();
            }
            if (hash.keys().contains("cpuname")){
                ui->comboBox_cpu->clear();
                for(auto set : resultSet){
                    ui->comboBox_cpu->addItem(set.toHash().value("cpuname").toString());
                }
                return;
            }
            if (hash.contains("systemname")){
                ui->comboBox_sys_version->clear();
                ui->comboBox_sys_name->clear();
                auto resultSet = doc.object().value("resultset").toVariant().toList();
                for(auto set : resultSet){
                    ui->comboBox_sys_name->addItem(set.toHash().value("systemname").toString());
                    ui->comboBox_sys_version->addItem(set.toHash().value("systemver").toString());
                }
                return;
            }

            if (isLogin && !hash.contains("count")){
                QVariantHash cpuHash= {{"name", ui->lineEdit_name->text()},
                                       {"ver", ui->lineEdit_version->text()},
                                       {"systemver", ui->comboBox_sys_version->currentText()},
                                       {"systemname", ui->comboBox_sys_name->currentText()},
                                       {"cpuname", ui->comboBox_cpu->currentText()},
                                       {"type", ui->comboBox_type->currentText()},
                                       {"fileName", ui->lineEdit_res->text()}};
                emit addModuleInfo(cpuHash);
            }
        }
    }
    close();
}

void ModuleInfoDlg::on_pushButton_confirm_clicked()
{
    QString name = ui->lineEdit_name->text();
    QString version = ui->lineEdit_version->text();
    QString sys_verion = ui->comboBox_sys_version->currentText();
    QString sys_name = ui->comboBox_sys_name->currentText();
    QString cpu = ui->comboBox_cpu->currentText();
    QString type = ui->comboBox_type->currentText();
    QString fileName = ui->lineEdit_res->text();

    m_httpClient->moduleInfoAdd(name, version, sys_name, sys_verion, cpu, type.toInt(), fileName);
}


void ModuleInfoDlg::on_pushButton_cancel_clicked()
{
    close();
}


void ModuleInfoDlg::on_pushButton_brow_clicked()
{

}

