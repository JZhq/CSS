#ifndef FRAMEWORKINFODLG_H
#define FRAMEWORKINFODLG_H

#include <QWidget>
#include "httpclient.h"

namespace Ui {
class FrameworkInfoDlg;
}

class FrameworkInfoDlg : public QWidget
{
    Q_OBJECT

public:
    explicit FrameworkInfoDlg(QWidget *parent = nullptr);
    ~FrameworkInfoDlg();
signals:
    void addFrameworkInfoSignal(const QVariantHash &);

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_brow_clicked();
    void on_result(bool state, const QString& respons);

private:
    Ui::FrameworkInfoDlg *ui;
    HttpClient *m_httpClient;
    QStringList m_resourDirs;
};

#endif // FRAMEWORKINFODLG_H
