#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QWidget>
#include "httpclient.h"

namespace Ui {
class ProjectWidget;
}

class ProjectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(QWidget *parent = nullptr);
    ~ProjectWidget();

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();
     void on_loginResult(bool  state, const QString& json);

signals:
    void addproject(const QVariantHash &);

private:
    Ui::ProjectWidget *ui;
    HttpClient *m_httpClient;
};

#endif // PROJECTWIDGET_H
