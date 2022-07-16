#ifndef systemInfoDlg_H
#define systemInfoDlg_H

#include "editorbase.h"
namespace Ui {
class systemInfoDlg;
}

class SystemInfoDlg : public EditorBase
{
    Q_OBJECT
public:
    explicit SystemInfoDlg(QWidget *parent = nullptr, QVariantHash d=QVariantHash(), EditorMode m=EditorMode::New);
    ~SystemInfoDlg();

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_2_clicked();
    void on_result(bool state, const QString &respons);

signals:
    void addSystemInfo(const QVariantHash &);

private:
    Ui::systemInfoDlg *ui;
};

#endif // systemInfoDlg_H
