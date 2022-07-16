#include "frameworkinfodlg.h"
#include "ui_frameworkinfodlg.h"
#include <QFileDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDir>

FrameworkInfoDlg::FrameworkInfoDlg(QWidget *parent, QVariantHash d, EditorMode m) :
    EditorBase(parent,d,m),
    ui(new Ui::FrameworkInfoDlg)
{
    ui->setupUi(this);
    m_httpClient->cpuConfigQueryList();
    m_httpClient->opSystemConfigQueryList();
}

FrameworkInfoDlg::~FrameworkInfoDlg()
{
    delete ui;
}

void FrameworkInfoDlg::on_pushButton_confirm_clicked()
{
    QString name = ui->lineEdit_name->text();
    QString verison = ui->lineEdit_version->text();
    QString sys_version = ui->comboBox_sys_version->currentText();
    QString sys_name = ui->comboBox_sys_name->currentText();
    QString cpu = ui->comboBox_cpu->currentText();
    QString type = ui->comboBox_type->currentText();
    const QString dir = ui->lineEdit_res->text();

    if(dir.isEmpty() || QDir(dir).isEmpty() ){
        QMessageBox::information(this, u8"提示", u8"无效框架资源路径");
        return;
    }

    QString fileName =  QString("%1/%2_%3.zip").arg(dir).arg(name).arg(verison);
    if (!compressDir(fileName, dir)){
        qDebug() << "compressDir failed" << fileName;
    }

    m_httpClient->frameworkInfoAdd(name, verison, sys_version, sys_name, cpu, type.toInt(), fileName);
}


void FrameworkInfoDlg::on_pushButton_cancel_clicked()
{
    close();
}


void FrameworkInfoDlg::on_pushButton_brow_clicked()
{
   const QString dir = QFileDialog::getExistingDirectory(this,  u8"框架资源选择");
   ui->lineEdit_res->setText(dir);
}

void FrameworkInfoDlg::on_result(bool state, const QString &respons)
{
    // qDebug() << state << respons;
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
                QVariantHash frameHash= {{"name",        ui->lineEdit_name->text()},
                                        {"ver",      ui->lineEdit_version->text()},
                                        {"systemver",  ui->comboBox_sys_version->currentText()},
                                        {"systemname",     ui->comboBox_sys_name->currentText()},
                                        {"cpuname",          ui->comboBox_cpu->currentText()},
                                        {"type",         ui->comboBox_type->currentText()},
                                        {"fileName",     ui->lineEdit_res->text()}};
                emit addFrameworkInfoSignal(frameHash);
                emit editorDataChanged();
            }
        }
    }
}

