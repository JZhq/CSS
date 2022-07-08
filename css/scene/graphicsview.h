/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         编辑区视图
*@author        luox
*@date          2018-05-07
**/
#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include "common/scene_global.h"

class QWheelEvent;
class QKeyEvent;
class SuspendWidget;
class TopSuspendWidget;
class ItemsOperationSuspendWidget;
class QUndoStack;

class SCENE_SHARED_EXPORT GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = Q_NULLPTR);
    ~GraphicsView();

    // 平移速度
    void setTranslateSpeed(qreal speed);
    qreal translateSpeed() const;

    // 缩放的增量
    void setZoomDelta(qreal delta);
    qreal zoomDelta() const;

    void setTopSuspendWidgetVisible(bool visible);
    void setItemsOperateSuspendWidgetVisible(bool visible);
    void installUndoAction(QUndoStack* stack, QAction* undoAct, QAction* redoAct);
    void installOperataScene(QGraphicsScene* scene);

public Q_SLOTS:
    void zoomIn();  // 放大
    void zoomOut();  // 缩小
    void zoom(float scaleFactor); // 缩放 - scaleFactor：缩放的比例因子
    void translate(QPointF delta);  // 平移
    void onPrintScene();//快照
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    // 平移
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    // 放大/缩小
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;//鼠标滑轮事件
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
private:
    void handleEvents();
private:
    Qt::MouseButton m_translateButton;  // 平移按钮
    qreal m_translateSpeed;  // 平移速度
    qreal m_zoomDelta;  // 缩放的增量
    bool m_bMouseTranslate;  // 平移标识
    QPoint m_lastMousePos;  // 鼠标最后按下的位置
    qreal m_scale;  // 缩放值

    SuspendWidget *m_susWiget;// 底部悬浮窗口
    TopSuspendWidget *m_topSuspendWidget; // 顶部的悬浮窗口
    ItemsOperationSuspendWidget *m_itemsOperateSusWidget; // 图元对齐功能悬浮窗口
};

#endif // GRAPHICSVIEW_H
