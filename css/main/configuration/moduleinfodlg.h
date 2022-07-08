#ifndef MODULEINFODLG_H
#define MODULEINFODLG_H

#include <QWidget>

namespace Ui {
class ModuleInfoDlg;
}

class ModuleInfoDlg : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleInfoDlg(QWidget *parent = nullptr);
    ~ModuleInfoDlg();

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();

    void on_pushButton_brow_clicked();

private:
    Ui::ModuleInfoDlg *ui;
};

#endif // MODULEINFODLG_H
