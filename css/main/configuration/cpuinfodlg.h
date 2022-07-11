#ifndef CPUINFODLG_H
#define CPUINFODLG_H

#include <QWidget>
#include "httpclient.h"

namespace Ui {
class CpuInfoDlg;
}

class CpuInfoDlg : public QWidget
{
    Q_OBJECT
public:
    explicit CpuInfoDlg(QWidget *parent = nullptr);
    ~CpuInfoDlg();

private slots:
    void on_result(bool state, const QString &respons);
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();

signals:
    void addCpuInfo(const QVariantHash &);

private:
    Ui::CpuInfoDlg *ui;
    HttpClient *m_httpClient;
};

#endif // CPUINFODLG_H
