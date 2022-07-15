#ifndef FRAMEWORKINFODLG_H
#define FRAMEWORKINFODLG_H

#include "editorbase.h"

namespace Ui {
class FrameworkInfoDlg;
}

class FrameworkInfoDlg : public EditorBase
{
    Q_OBJECT

public:
    explicit FrameworkInfoDlg( QWidget *parent = nullptr, QVariantHash d=QVariantHash(), EditorMode m=EditorMode::New );
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
    QStringList m_resourDirs;
};

#endif // FRAMEWORKINFODLG_H
