#include "regionselectitem.h"
#include "graphicsscene.h"

#include <QPainter>
#include <QDebug>

RegionSelectItem::RegionSelectItem(QGraphicsItem *parent) :
    BaseItem(parent)
{
    m_startPoint = QPointF(0, 0);
    m_endPoint = QPointF(0, 0);
    m_eStep = DRAWSELTOOLSTEP_INVALID;
}

void RegionSelectItem::setStep(DrawSelToolStep step)
{
    m_eStep = step;
}

enumDrawSelToolStep RegionSelectItem::step() const
{
    return m_eStep;
}

void RegionSelectItem::selectItems(GraphicsScene *pScene)
{

    QRectF rect = m_prepareGeometry;
    if (pScene && rect.isValid() && !rect.isEmpty() && !rect.isNull())
    {
        QList<QGraphicsItem *> tmpItems = pScene->items();
        QGraphicsItem *pItem = 0;
        for (int i=0; i<tmpItems.size(); i++)
        {
            pItem = tmpItems.at(i);
            if (pItem)
            {
                //qDebug() << "i = " << i << pItem->x() << " " << pItem->y() << " " << pItem->boundingRect().width() << " " << pItem->boundingRect().height();
                QRectF itemRect(pItem->x(), pItem->y(), 40, 40);
                pItem->setSelected(itemRect.intersects(rect));
                pItem->update();
            }
        }
        //qDebug() << "rect" << rect.x() << " " << rect.y() << " " << rect.width() << " " << rect.height();
         pScene->update();
    }
}

int RegionSelectItem::type() const
{
    return Type;
}

void RegionSelectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    drawItem(painter, option);
   // return BaseItem::paint(painter,option,widget);
}

void RegionSelectItem::drawItem(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(option)

    if (painter)
    {
        painter->save();
        painter->setPen(QPen(Qt::red));
        if (DRAWSELTOOLSTEP_1 == m_eStep ||
                 DRAWSELTOOLSTEP_2 == m_eStep ||
                 DRAWSELTOOLSTEP_FINISH == m_eStep)
        {
            painter->drawRect(m_prepareGeometry);
        }
        painter->restore();
    }
}

QPainterPath RegionSelectItem::shape() const
{
    QPainterPath path;

    QRectF rect = boundingRect();
    path.addRect(rect);

    return path;
}

void RegionSelectItem::setStartPointX(float startPointX)
{
    this->m_startPoint.setX(startPointX);
    prepareGeometry();      // 重新计算geometry
    this->update();
}

float RegionSelectItem::getStartPointX() const
{
    return this->m_startPoint.x();
}

void RegionSelectItem::setStartPointY(float startPointY)
{
    this->m_startPoint.setY(startPointY);
    prepareGeometry();      // 重新计算geometry
    this->update();
}

float RegionSelectItem::getStartPointY() const
{
    return this->m_startPoint.y();
}

void RegionSelectItem::setEndPointX(float endPointX)
{
    this->m_endPoint.setX(endPointX);
    prepareGeometry();      // 重新计算geometry
    this->update();
}

float RegionSelectItem::getEndPointX() const
{
    return this->m_endPoint.x();
}

void RegionSelectItem::setEndPointY(float endPointY)
{
    this->m_endPoint.setY(endPointY);
    prepareGeometry();      // 重新计算geometry
    this->update();
}

float RegionSelectItem::getEndPointY() const
{
    return this->m_endPoint.y();
}

QRectF RegionSelectItem::boundingRect() const
{
    return m_prepareGeometry;
}

void RegionSelectItem::prepareGeometry()
{
    // bounding rect 改变之前的预处理
    prepareGeometryChange();
    // 计算新的bounding rect
    if (DRAWSELTOOLSTEP_1 == m_eStep ||
             DRAWSELTOOLSTEP_2 == m_eStep ||
             DRAWSELTOOLSTEP_FINISH == m_eStep)
    {
        m_prepareGeometry = QRectF(m_startPoint, m_endPoint);
        m_prepareGeometry = m_prepareGeometry.normalized();
    }
    else
    {
        m_prepareGeometry = QRectF();
    }

    // 计算select shape rect
    m_selectShapeRect = m_prepareGeometry;
}
