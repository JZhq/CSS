/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         编辑区视图
*@author        luox
*@date          2018-05-07
**/
#include <QKeyEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QApplication>
#include <QUndoStack>

#include "graphicsview.h"
#include "suspendwidget.h"
#include "topsuspendwidget.h"
#include "itemsoperationsuspendwidget.h"

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

GraphicsView::GraphicsView(QWidget *parent):
               QGraphicsView(parent),
               m_translateButton(Qt::LeftButton),
               m_scale(1.0),
               m_zoomDelta(0.1),
               m_translateSpeed(1.0),
               m_bMouseTranslate(false)
{
    //无缓存
    setCacheMode(CacheNone);
    //全部视图更新
    setViewportUpdateMode(FullViewportUpdate);
    //反走样
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2,  VIEW_HEIGHT / 2);
    centerOn(mapToScene(newCenter));
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    setContextMenuPolicy(Qt::CustomContextMenu);

//    // 去掉滚动条
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_susWiget = new SuspendWidget(this);
    m_susWiget->show();

    m_topSuspendWidget = new TopSuspendWidget(this);
    m_topSuspendWidget->show();

    m_itemsOperateSusWidget = new ItemsOperationSuspendWidget(this);
    m_itemsOperateSusWidget->show();

    //连接信号与槽
    handleEvents();

    setContextMenuPolicy(Qt::DefaultContextMenu);
}

GraphicsView::~GraphicsView()
{

}

//平移速度
void GraphicsView::setTranslateSpeed(qreal speed)
{
    // 建议速度范围
    Q_ASSERT_X(speed >= 0.0 && speed <= 2.0,
               "InteractiveView::setTranslateSpeed", "Speed should be in range [0.0, 2.0].");
    m_translateSpeed = speed;
}

qreal GraphicsView::translateSpeed() const
{
    return m_translateSpeed;
}

//缩放增量
void GraphicsView::setZoomDelta(qreal delta)
{
    // 建议增量范围
    Q_ASSERT_X(delta >= 0.0 && delta <= 1.0,
               "InteractiveView::setZoomDelta", "Delta should be in range [0.0, 1.0].");
    m_zoomDelta = delta;
}

qreal GraphicsView::zoomDelta() const
{
    return m_zoomDelta;
}

void GraphicsView::setTopSuspendWidgetVisible(bool visible)
{
    if (m_topSuspendWidget)
        m_topSuspendWidget->setVisible(visible);
}

void GraphicsView::setItemsOperateSuspendWidgetVisible(bool visible)
{
    if (m_itemsOperateSusWidget)
        m_itemsOperateSusWidget->setVisible(visible);
}

void GraphicsView::installUndoAction(QUndoStack* stack, QAction *undoAct, QAction *redoAct)
{
    if (m_topSuspendWidget)
        m_topSuspendWidget->installUndoAction(stack, undoAct, redoAct);
}

void GraphicsView::installOperataScene(QGraphicsScene *scene)
{
    if (scene)
        m_itemsOperateSusWidget->setScene(scene);
}

//放大
void GraphicsView::zoomIn()
{
    zoom(1 + m_zoomDelta);
}

//缩小
void GraphicsView::zoomOut()
{
    zoom(1 - m_zoomDelta);
}

// 缩放 - scaleFactor：缩放的比例因子
void GraphicsView::zoom(float scaleFactor)
{
    // 防止过小或过大
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
    m_scale *= scaleFactor;
}

void GraphicsView::translate(QPointF delta)
{
    // 根据当前 zoom 缩放平移数
    delta *= m_scale;
    delta *= m_translateSpeed;

    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(),  VIEW_HEIGHT / 2 - delta.y());
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void GraphicsView::onPrintScene()
{

   //QImage image(130 * scene()->width()/ scene()->height(),130 ,QImage::Format_ARGB32);
   QImage image(scene()->width(),scene()->height(),QImage::Format_ARGB32);
   QPainter painter(&image);
   painter.setRenderHint(QPainter::Antialiasing);
   scene()->render(&painter);

    const QString format = "png";
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += tr("/untitled.") + format;

    QFileDialog fileDialog(this, QStringLiteral("保存到"), initialPath,tr("Image Files (*.png)"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);

    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fileName = fileDialog.selectedFiles().first();


    if(!image.save(fileName,"png"))
    {
        qDebug()<<"save widget screen failed";
        QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("保存失败"));
    }
    else
    {
        QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("保存成功"));
    }
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
//    qDebug()<<size;
//    qDebug()<<this->pos();



    if(m_susWiget)
    {
        qreal x = size.width()-150;
        qreal y = size.height()-50;
        m_susWiget->move(QPoint(x,y));
    }

    if (m_topSuspendWidget)
    {
        qreal x = size.width()-120;
        qreal y = 0;
        m_topSuspendWidget->move(QPoint(x,y));
    }
    if (m_itemsOperateSusWidget)
    {
        qreal x = 0;
        qreal y = 0;
        m_itemsOperateSusWidget->move(QPoint(x,y));
    }
}

//平移
void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMouseTranslate){
        QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
        translate(mouseDelta);
    }

    m_lastMousePos = event->pos();

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == m_translateButton && QApplication::keyboardModifiers () == Qt::ControlModifier) {
        // 当光标底下没有 item 时，才能移动
        QPointF point = mapToScene(event->pos());
        if (scene()->itemAt(point, transform()) == NULL)  {
            m_bMouseTranslate = true;
            m_lastMousePos = event->pos();
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == m_translateButton)
        m_bMouseTranslate = false;

    QGraphicsView::mouseReleaseEvent(event);
}



/**
*@brief       重写widget鼠标滑轮事件函数，用于缩放view
*@parameter   QWheelEvent *:鼠标滑轮事件
*@return
**/
void GraphicsView::wheelEvent(QWheelEvent *event)
{
    // 滚轮的滚动量
    QPoint scrollAmount = event->angleDelta();
    // 正值表示滚轮远离使用者（放大），负值表示朝向使用者（缩小）
    scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    return QGraphicsView::contextMenuEvent(event);
}

void GraphicsView::handleEvents()
{
    //缩小
    connect(m_susWiget,&SuspendWidget::signalZoomOut,this,&GraphicsView::zoomOut);
    //放大
    connect(m_susWiget,&SuspendWidget::signalZoomIn,this,&GraphicsView::zoomIn);
    //快照
    // connect(m_susWiget,&SuspendWidget::signalPrintScene,this,&GraphicsView::onPrintScene);


}


