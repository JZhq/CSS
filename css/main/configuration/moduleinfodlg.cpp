#include "moduleinfodlg.h"
#include "ui_moduleinfodlg.h"

ModuleInfoDlg::ModuleInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModuleInfoDlg)
{
    ui->setupUi(this);
}

ModuleInfoDlg::~ModuleInfoDlg()
{
    delete ui;
}

void ModuleInfoDlg::on_pushButton_confirm_clicked()
{

}


void ModuleInfoDlg::on_pushButton_cancel_clicked()
{

}


void ModuleInfoDlg::on_pushButton_brow_clicked()
{

}

