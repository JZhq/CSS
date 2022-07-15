#ifndef MODULEINFODLG_H
#define MODULEINFODLG_H

#include "editorbase.h"

namespace Ui {
class ModuleInfoDlg;
}

class ModuleInfoDlg : public EditorBase
{
    Q_OBJECT

public:
    explicit ModuleInfoDlg(QWidget *parent = nullptr, QVariantHash d=QVariantHash(), EditorMode m=EditorMode::New);
    ~ModuleInfoDlg();

private slots:
    void on_result(bool state, const QString &respons);
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_brow_clicked();

signals:
    void addModuleInfo(const QVariantHash &);

private:
    Ui::ModuleInfoDlg *ui;
};

#endif // MODULEINFODLG_H
