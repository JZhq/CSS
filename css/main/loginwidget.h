#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "httpclient.h"

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    void loginChanged(bool);

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();
    void on_loginResult(bool  state, const QString& json);

private:
    Ui::LoginWidget         *ui;
    HttpClient              *m_httpClient;
};

#endif // LOGINWIDGET_H
