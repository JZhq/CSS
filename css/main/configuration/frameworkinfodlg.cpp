#include "frameworkinfodlg.h"
#include "ui_frameworkinfodlg.h"
#include <QFileDialog>
#include <QDebug>

FrameworkInfoDlg::FrameworkInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameworkInfoDlg)
{
    ui->setupUi(this);
}

FrameworkInfoDlg::~FrameworkInfoDlg()
{
    delete ui;
}

void FrameworkInfoDlg::on_pushButton_confirm_clicked()
{

}


void FrameworkInfoDlg::on_pushButton_cancel_clicked()
{

}


void FrameworkInfoDlg::on_pushButton_brow_clicked()
{
}

