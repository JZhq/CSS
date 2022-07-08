#ifndef CPUINFODLG_H
#define CPUINFODLG_H

#include <QWidget>

namespace Ui {
class CpuInfoDlg;
}

class CpuInfoDlg : public QWidget
{
    Q_OBJECT
public:
    explicit CpuInfoDlg(QWidget *parent = nullptr);
    ~CpuInfoDlg();

private:
    Ui::CpuInfoDlg *ui;
};

#endif // CPUINFODLG_H
