#ifndef POPUPTITLE_H
#define POPUPTITLE_H

#include <QWidget>

namespace Ui {
class PopupTitle;
}

class PopupTitlePrivate;
class PopupTitle : public QWidget
{
    Q_OBJECT

public:
    explicit PopupTitle(QWidget *parent = 0);
    ~PopupTitle();

    //!
    //! \brief 获取标题名
    //! \return 标题名
    //!
    QString title();

    //!
    //! \brief 设置标题
    //! \param title 标题名
    //!
    void setTitle(const QString& title);

    //!
    //! \brief 设置图表
    //! \param title 图表路径
    //!
    void setIcon(const QString &icon);

    //!
    //! \brief 控制标题栏按钮显/隐
    //! \param min 最小化按钮
    //! \param max 最大化按钮
    //! \param close 关闭按钮
    //!
    void setButtonsVisibale(bool min, bool max, bool close);

    //!
    //! \brief 设置标题栏被改变，若标题名是以 [*] 初始化结尾的，则将显示 *
    //! \param modified
    //!
    void setTitleBarModified(bool modified);

signals:
    void signalMin();
    void signalRestore();
    void signalMax();
    void signalClose();

private:
    //!
    //! \brief 初始化界面
    //!
    void initializeUi();

    //!
    //! \brief 信号槽事件处理
    //!
    void handleEvents();

private:
    Ui::PopupTitle *ui;
    PopupTitlePrivate *d;
};

#endif // POPUPTITLE_H
