#include "moduleinfodlg.h"
#include "ui_moduleinfodlg.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>
#include <QFileDialog>

ModuleInfoDlg::ModuleInfoDlg(QWidget *parent, QVariantHash d, EditorMode m) :
    EditorBase(parent, d, m),
    ui(new Ui::ModuleInfoDlg)
{
    ui->setupUi(this);
    ui->comboBox_type->addItems(QStringList() << "0" << "1");
    if (m==Update){
         m_oldHash = d;
         m_oldName = d.value("modu_name").toString();
         m_oldVer = d.value("ver").toString();
         ui->lineEdit_name->setText(d.value("modu_name").toString());
         ui->lineEdit_version->setText(d.value("ver").toString());
         ui->comboBox_sys_version->setCurrentText(d.value("systemver").toString());
         ui->comboBox_sys_name->setCurrentText(d.value("systemname").toString());
         ui->comboBox_cpu->setCurrentText(d.value("cpuname").toString());
         ui->comboBox_type->setCurrentText(d.value("type").toString());
    }

    if (m_httpClient){
        m_httpClient->cpuConfigQueryList();
        m_httpClient->opSystemConfigQueryList();
    }
}

ModuleInfoDlg::~ModuleInfoDlg()
{
    delete ui;
}

void ModuleInfoDlg::on_result(bool state, const QString &respons)
{
//    qDebug() << state << respons;
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
                QVariantHash cpuHash= {{"modu_name", ui->lineEdit_name->text()},
                                       {"ver", ui->lineEdit_version->text()},
                                       {"systemver", ui->comboBox_sys_version->currentText()},
                                       {"systemname", ui->comboBox_sys_name->currentText()},
                                       {"cpuname", ui->comboBox_cpu->currentText()},
                                       {"type", ui->comboBox_type->currentText()},
                                       {"fileName", ui->lineEdit_res->text()}};
                if (m_mode == New)
                    emit addModuleInfo(cpuHash);
                if (m_mode == Update)
                    emit editorDataUpdated(cpuHash);

                emit editorDataChanged();
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
    const QString dir = ui->lineEdit_res->text();
    if(dir.isEmpty() || QDir(dir).isEmpty() ){
        QMessageBox::information(this, u8"提示", u8"无效模块资源路径");
        return;
    }

    QString fileName =  QString("%1/%2_%3.zip").arg(dir).arg(name).arg(version);
    if (!compressDir(fileName, dir)){
        qDebug() << "compressDir failed" << fileName;
    }
    if (m_mode == New)
        m_httpClient->moduleInfoAdd(name, version, sys_name, sys_verion, cpu, type.toInt(), fileName);
    if (m_mode == Update)
        m_httpClient->moduleInfoUpdate(m_oldName, m_oldVer, name, version, sys_name, sys_verion, cpu, type.toInt(), fileName);
}


void ModuleInfoDlg::on_pushButton_cancel_clicked()
{
    close();
}


void ModuleInfoDlg::on_pushButton_brow_clicked()
{
    const QString dir = QFileDialog::getExistingDirectory(this, u8"模块资源选择");
    ui->lineEdit_res->setText(dir);
}

