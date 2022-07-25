#include "projectwidget.h"
#include "ui_projectwidget.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QMessageBox>
#include "configurationglobal.h"

ProjectWidget::ProjectWidget(EditMode m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectWidget),
    m_httpClient(NULL)
{
    ui->setupUi(this);
    m_httpClient = new HttpClient(ConfigGlobalInterface->serverAddr(), 8080, this, SLOT(on_loginResult(bool, const QString&)));
}

ProjectWidget::~ProjectWidget()
{
    delete ui;
}

void ProjectWidget::on_pushButton_confirm_clicked()
{
    QString prjName = ui->lineEdit_proName->text();
    QString prjDesc = ui->lineEdit_proDesc->text();
    QVariantHash projectInfos = {{"projectname", prjName},
                                 {"desc", prjDesc}};

    m_httpClient->addProject(prjName, prjDesc);

    emit addproject(projectInfos);
}


void ProjectWidget::on_pushButton_cancel_clicked()
{
    close();
}

void ProjectWidget::on_loginResult(bool state, const QString &respons)
{
    if (state){
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            bool rst = doc.object().value("result").toBool();
            if (!rst){
                QString errStr = doc.object().value("errstr").toString();
                QMessageBox::warning(this, u8"提示信息", errStr);
            }
            qDebug() << respons << rst;
        }
    }

    close();
}

