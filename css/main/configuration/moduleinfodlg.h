#ifndef MODULEINFODLG_H
#define MODULEINFODLG_H

#include <QWidget>
#include "httpclient.h"

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
    void on_result(bool state, const QString &respons);
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_brow_clicked();

signals:
    void addModuleInfo(const QVariantHash &);

private:
    Ui::ModuleInfoDlg *ui;
    HttpClient *m_httpClient;
};

#endif // MODULEINFODLG_H
