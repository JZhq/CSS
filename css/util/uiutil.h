/***************************************
 * CopyRight: sinux
 * Description: 操作界面的工具单例类
 * Authors: zs
 * Date: 2018-04-27
 * History:no
 ***************************************/

#ifndef UIUTIL_H
#define UIUTIL_H

#include "util_global.h"

class QString;
class QWidget;
class QStackedWidget;

class UTILSHARED_EXPORT UiUtil
{
public:
    //!
    //! \brief 获取此单例的实例
    //! \return
    //!
    static UiUtil* getInstance()
    {
        static UiUtil instance;
        return &instance;
    }

    void openFile(const QString& fileName);

    //!
    //! \brief 为整个应用程序加载 QSS
    //!
    void loadQss();

    //!
    //! \brief 修改过 widget 的属性后，使此属性对应的 Style Sheet 生效
    //! \param widget
    //!
    void updateQss(QWidget* widget);

    //!
    //! \brief 设置 widget 的 padding 和 spacing
    //! \param widget 被设置的界面
    //! \param padding
    //! \param spacing
    //!
    void setWidgetPaddingAndSpacing(QWidget *widget, int padding, int spacing);


    //!
    //! \brief 移动窗口到屏幕的中间
    //! \param window
    //!
    void centerWindow(QWidget *window);

    //!
    //! \brief 查找 w 所在窗口的顶层窗口
    //! \param w
    //! \return
    //!
    QWidget* findWindow(QWidget *w);

    //!
    //! \brief 把使用上面的函数 addWidgetIntoStackedWidget 添加过的 widget 设置为它的当前 widget
    //! \param widget
    //! \param stackedWidget
    //!
    void setCurrentWidgetOfStackedWidget(QWidget *widget, QStackedWidget *stackedWidget);

    //!
    //! \brief 把 widget 加入到 stacked widget 里, 可以设置向四个方向的伸展
    //! \param widget
    //! \param stackedWidget
    //! \param toLeft
    //! \param toTop
    //! \param toRight
    //! \param toBottom
    //!
    static void addWidgetIntoStackedWidget(QWidget *widget, QStackedWidget *stackedWidget,
                                           bool toLeft = true,
                                           bool toTop = true,
                                           bool toRight = true,
                                           bool toBottom = true);

private:
    UiUtil() {}
    ~UiUtil() {}
    UiUtil(const UiUtil& rParam) = delete;
    const UiUtil& operator = (const UiUtil& rParam) = delete;
};
#endif // UIUTIL_H
