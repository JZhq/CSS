#include "systeminfodlg.h"
#include "ui_systeminfodlg.h"

SystemInfoDlg::SystemInfoDlg(QWidget *parent) :
    QWidget(parent),
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

}


void SystemInfoDlg::on_pushButton_2_clicked()
{

}

