#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QWidget>
#include "popupwidget.h"
namespace Ui {
class EditWindow;
}

class EditWindow : public PopupWidget
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = 0);
    ~EditWindow();
    //!
    //! \brief 当确认按钮被点击后应该处理的事情
    //! \return 确认后关闭窗口返回 true，否则返回faose
    //!
    virtual bool afterConfirmButtonClicked() override;

    //!
    //! \brief 当取消按钮被点击后应该处理的事情
    //! \return 确认后关闭窗口返回 true，否则返回faose
    //!
    virtual bool afterCancelButtonClicked() override;

    void setText(const QString &text);
signals:
    void sendBiaoDaShi(const QString &msg);
private:
    Ui::EditWindow *ui;
};

#endif // EDITWINDOW_H
