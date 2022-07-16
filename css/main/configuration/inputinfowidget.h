#ifndef INPUTINFOWIDGET_H
#define INPUTINFOWIDGET_H

#include <QWidget>

namespace Ui {
class InputInfoWidget;
}

class InputInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InputInfoWidget(QWidget *parent = nullptr);
    ~InputInfoWidget();
signals:
    void inputEditorDataChanged();

private:
    Ui::InputInfoWidget *ui;
};

#endif // INPUTINFOWIDGET_H
