/***************************************
 * CopyRight: sinux
 * Description: 用于二级界面打开的通用界面类
 * Authors: zs
 * Date: 2018-05-07
 * History:no
 ***************************************/
#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include "public_global.h"
#include <QWidget>

namespace Ui {
class PopupWindow;
}

class PopupWindowPrivate;

class PopupWidget;
class TopWindow;
class PopupTitle;
class GLOBAL_EXPORT PopupWindow : public QWidget
{
    Q_OBJECT

public:
    //!
    //! \brief PopupWindow 构造函数
    //! \param center 中央界面
    //! \param buttonVisible
    //!
    explicit PopupWindow(QWidget* center, bool buttonVisible = false);
    ~PopupWindow();

    //!
    //! \brief 获取是否确认的结果
    //! \return 点击确认返回 true，取消返回 false
    //!
    bool accepted() const;

    //!
    //! \brief 获取自定义标题栏的地址
    //! \return 自定义标题栏的地址
    //!
    PopupTitle *titleBar() const;

    //!
    //! \brief 设置窗口是否改变的属性
    //! \param modified
    //!
    void setWindowModified(bool modified);

    //!
    //! \brief 判断窗口是否改变
    //! \return 若窗口改变了返回 true，反之返回 false
    //!
    bool isWindowModified();

public:
    //!
    //! \brief 模态的打开一个窗口
    //! \param w 中央界面
    //! \param title 标题
    //! \param resizable 是否可变换大小
    //! \param miniBtn 最小化按钮
    //! \param maxBtn 最大化按钮
    //! \param closeBtn 关闭按钮
    //! \param confirmBtn 下放确认和取消按钮
    //!
    static void exec(QWidget *w,
            const QString &title,
            bool resizable = true,
            bool miniBtn = true,
            bool maxBtn = true,
            bool closeBtn = true,
            bool confirmBtn = false
            );

    //!
    //! \brief 默认是带有确认和取消按钮的模态窗口
    //! \param w 中央界面
    //! 注意：必须使用 new 在堆上开辟空间，函数会自动释放内存
    //! 否则程序将你会崩溃
    //! \param title 标题
    //! \param parent 父窗口地址
    //!
    static bool exec(const QString &title, PopupWidget *w);

    //!
    //! \brief 非模态的打开一个窗口
    //! \param w 中央界面
    //! \param title 标题
    //! \param resizable 是否可变换大小
    //! \param miniBtn 最小化按钮
    //! \param maxBtn 最大化按钮
    //! \param closeBtn 关闭按钮
    //! \param confirmBtn 下放确认和取消按钮
    //!
    static void show(
            QWidget *w,
            const QString &icon,
            const QString &title,
            bool resizable = true,
            bool miniBtn = true,
            bool maxBtn = true,
            bool closeBtn = true,
            bool confirmBtn = false
            );

    //!
    //! \brief 默认是带有确认和取消按钮的非模态窗口
    //! \param 标题
    //! \param 中央界面
    //! \return
    //!
    static bool show(const QString &icon, const QString &title, PopupWidget *w);

private:
    //!
    //! \brief 初始化界面
    //!
    void initializeUi();

    //!
    //! \brief 信号槽事件处理
    //!
    void handleEvents();

    //!
    //! \brief 设置窗口的属性
    //! \param window 窗口
    //! \param center 中央界面
    //! \param title 标题界面
    //! \param resizable 是否可改变大小
    //! \param isDialog 是否是Dialog
    //!
    static void setWindow(TopWindow *window, QWidget *center,
                          PopupTitle *title, bool resizable, bool isDialog);

    //!
    //! \brief 标题栏事件绑定
    //! \param title 标题界面
    //! \param window 窗口
    //!
    static void handleTitleBarEvents(PopupTitle *title, TopWindow *window);

private:
    Ui::PopupWindow *ui;
    PopupWindowPrivate *d;
};

#endif // POPUPWINDOW_H
