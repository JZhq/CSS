#ifndef SUSPENDWIDGET_H
#define SUSPENDWIDGET_H

#include <QWidget>
class QToolButton;

class SuspendWidget : public QWidget
{
    Q_OBJECT
public:
    SuspendWidget(QWidget *parent = Q_NULLPTR);
    ~SuspendWidget();

signals:
    void signalZoomOut();
    void signalZoomIn();
    void signalPrintScene();
private:
    /**
     * @brief 初始化ui
     */
    void initUi();
    /**
     * @brief 连接信号与槽
     */
    void handleEvents();
private:
    //缩小按钮
    QToolButton *m_zoomOutBtn;
    //放大按钮
    QToolButton *m_zoomInBtn;
    //快照
    QToolButton *m_printBtn;
};

#endif // SUSPENDWIDGET_H
