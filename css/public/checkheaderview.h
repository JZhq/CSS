#ifndef CHECKHEADERVIEW_H
#define CHECKHEADERVIEW_H

#include "public_global.h"
#include <QHeaderView>

class CheckHeaderViewPrivate;
class GLOBAL_EXPORT CheckHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    CheckHeaderView(Qt::Orientation orientation, QWidget *parent);
    ~CheckHeaderView();

private:
    //!
    //! \brief 初始化界面
    //!
    void initializeUi();

    //!
    //! \brief 信号槽事件处理
    //!
    void handleEvents();

signals:
    //!
    //! \brief 状态改变信号
    //! \param state
    //!
    void stateChanged(int state);

public slots:
    //!
    //! \brief 用于更新复选框状态
    //! \param state
    //!
    void onStateChanged(int state);

protected:

    //!
    //! \brief 绘制复选框
    //! \param painter
    //! \param rect
    //! \param logicalIndex
    //!
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

    //!
    //! \brief 鼠标按下表头
    //! \param e
    //!
    virtual void mousePressEvent(QMouseEvent *event) override;

    //!
    //! \brief 鼠标从表头释放，发送信号，更新model数据
    //! \param e
    //!
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    //!
    //! \brief 鼠标滑过、离开，更新复选框状态
    //! \param e
    //! \return
    //!
    virtual bool event(QEvent *event) override;


private:
    CheckHeaderViewPrivate* d;
};

#endif // CHECKHEADERVIEW_H
