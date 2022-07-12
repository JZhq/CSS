#include "frameworkinfodlg.h"
#include "ui_frameworkinfodlg.h"
#include <QFileDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

FrameworkInfoDlg::FrameworkInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameworkInfoDlg),
    m_httpClient(NULL)
{
    ui->setupUi(this);
    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)), this);
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
    QString fileName = ui->lineEdit_res->text();

    m_httpClient->frameworkInfoAdd(name, verison, sys_version, sys_name, cpu, type.toInt(), fileName);
}


void FrameworkInfoDlg::on_pushButton_cancel_clicked()
{
    close();
}


void FrameworkInfoDlg::on_pushButton_brow_clicked()
{
    QFileDialog *urlDlg = new QFileDialog(this);
    urlDlg->setModal(QFileDialog::DirectoryOnly);
    urlDlg->exec();
    auto urls = urlDlg->selectedFiles();
    for(auto url : urls){
        qDebug() << url;
    }
}

void FrameworkInfoDlg::on_result(bool state, const QString &respons)
{
    qDebug() << state << respons;
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            if (isLogin){
                QVariantHash frameHash= {{"name",        ui->lineEdit_name->text()},
                                        {"ver",      ui->lineEdit_version->text()},
                                        {"systemver",  ui->comboBox_sys_version->currentText()},
                                        {"systemname",     ui->comboBox_sys_name->currentText()},
                                        {"cpuname",          ui->comboBox_cpu->currentText()},
                                        {"type",         ui->comboBox_type->currentText()},
                                        {"fileName",     ui->lineEdit_res->text()}};
                emit addFrameworkInfoSignal(frameHash);
            }
        }
    }
    close();
}

