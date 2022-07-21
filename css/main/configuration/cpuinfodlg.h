#ifndef CPUINFODLG_H
#define CPUINFODLG_H

#include "editorbase.h"

namespace Ui {
class CpuInfoDlg;
}

class CpuInfoDlg : public EditorBase
{
    Q_OBJECT
public:
    explicit CpuInfoDlg(QWidget *parent = nullptr, QVariantHash d=QVariantHash(), EditorMode m=EditorMode::New);
    ~CpuInfoDlg();

private slots:
    void on_result(bool state, const QString &respons);
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();

signals:
    void addCpuInfo(const QVariantHash &);

private:
    Ui::CpuInfoDlg *ui;
    QString m_oldName;
};

#endif // CPUINFODLG_H
