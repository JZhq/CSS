/***************************************
 * CopyRight: sinux
 * Description: 需要“确认”“取消”按钮的弹出界面，需要继承自它
 * Authors: zs
 * Date: 2018-05-08
 * History:no
 ***************************************/
#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include "public_global.h"
#include <QWidget>

class GLOBAL_EXPORT PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr);

    //!
    //! \brief 当确认按钮被点击后应该处理的事情
    //! \return 确认后关闭窗口返回 true，否则返回faose
    //!
    virtual bool afterConfirmButtonClicked() = 0;

    //!
    //! \brief 当取消按钮被点击后应该处理的事情
    //! \return 确认后关闭窗口返回 true，否则返回faose
    //!
    virtual bool afterCancelButtonClicked() = 0;

signals:
    //!
    //! \brief 当内容改变后，需要将标题加上 *，就 emit 此信号
    //!
    void signalWindowModified(bool);

public slots:
};

#endif // POPUPWIDGET_H
