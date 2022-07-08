#ifndef FRAMEWORKINFODLG_H
#define FRAMEWORKINFODLG_H

#include <QWidget>

namespace Ui {
class FrameworkInfoDlg;
}

class FrameworkInfoDlg : public QWidget
{
    Q_OBJECT

public:
    explicit FrameworkInfoDlg(QWidget *parent = nullptr);
    ~FrameworkInfoDlg();

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();

    void on_pushButton_brow_clicked();

private:
    Ui::FrameworkInfoDlg *ui;
};

#endif // FRAMEWORKINFODLG_H
