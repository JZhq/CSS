#ifndef EDITWINDOWS_H
#define EDITWINDOWS_H

//#include "popupwidget.h"
namespace Ui {
class EditWindows;
}

class EditWindows : public PopupWidget
{
    Q_OBJECT

public:
    explicit EditWindows(QWidget *parent = 0);
    ~EditWindows();
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
private:
    Ui::EditWindows *ui;
};

#endif // EDITWINDOWS_H
