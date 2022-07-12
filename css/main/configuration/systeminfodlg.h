#ifndef systemInfoDlg_H
#define systemInfoDlg_H

#include <QWidget>
#include "httpclient.h"

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
    void on_result(bool state, const QString &respons);

signals:
    void addSystemInfo(const QVariantHash &);

private:
    Ui::systemInfoDlg *ui;
    HttpClient *m_httpClient;
};

#endif // systemInfoDlg_H
