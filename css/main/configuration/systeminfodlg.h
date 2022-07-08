#ifndef systemInfoDlg_H
#define systemInfoDlg_H

#include <QWidget>

namespace Ui {
class systemInfoDlg;
}

class SystemInfoDlg : public QWidget
{
    Q_OBJECT

public:
    explicit SystemInfoDlg(QWidget *parent = nullptr);
    ~SystemInfoDlg();

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::systemInfoDlg *ui;
};

#endif // systemInfoDlg_H
