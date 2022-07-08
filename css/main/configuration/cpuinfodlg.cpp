#include "cpuinfodlg.h"
#include "ui_cpuinfodlg.h"

CpuInfoDlg::CpuInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuInfoDlg)
{
    ui->setupUi(this);
}

CpuInfoDlg::~CpuInfoDlg()
{
    delete ui;
}
