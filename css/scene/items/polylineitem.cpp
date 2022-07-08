#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include "polylineitem.h"

PolyLineItem::PolyLineItem(BaseItem *startItem, BaseItem *endItem, QGraphicsItem *parent)
{

    m_startItem = startItem;
    m_endItem = endItem;

    //获取开始和结束节点的位置
    getPoint();

    //线1
    QGraphicsLineItem *item = new QGraphicsLineItem(m_points.top_x, m_points.top_y, m_points.top_x, m_points.mid_y,parent);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    item->setZValue(-1);
    m_vectorLines.push_back(item);

    //线2
    item = new QGraphicsLineItem(m_points.top_x, m_points.mid_y, m_points.bottom_x, m_points.mid_y, parent);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    item->setZValue(-1);
    m_vectorLines.push_back(item);

    //线3
    item = new QGraphicsLineItem(m_points.bottom_x, m_points.mid_y, m_points.bottom_x, m_points.bottom_y, parent);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    item->setZValue(-1);
    m_vectorLines.push_back(item);

    // 线4
    item = new QGraphicsLineItem(m_points.bottom_x, m_points.bottom_y, m_points.bottom_x - 5 , m_points.bottom_y - 8, parent);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    item->setZValue(-1);
    m_vectorLines.push_back(item);

    // 线5
    item = new QGraphicsLineItem(m_points.bottom_x, m_points.bottom_y, m_points.bottom_x + 5 , m_points.bottom_y - 8, parent);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    item->setZValue(-1);
    m_vectorLines.push_back(item);

    m_path.moveTo(m_points.top_x,m_points.top_y);
    m_path.lineTo(m_points.top_x,m_points.mid_y);
    m_path.lineTo(m_points.bottom_x,m_points.mid_y);
    m_path.lineTo(m_points.bottom_x,m_points.bottom_y);
    m_path.moveTo(m_points.bottom_x,m_points.mid_y);
    m_path.lineTo(m_points.bottom_x - 5, m_points.bottom_y - 8);
    m_path.lineTo(m_points.bottom_x + 5, m_points.bottom_y - 8);
}

PolyLineItem::~PolyLineItem()
{
    QGraphicsLineItem *item;
    QVector<QGraphicsLineItem*>::iterator lineItem;
    for(lineItem = m_vectorLines.begin(); lineItem != m_vectorLines.end(); lineItem++)
    {
        item = (QGraphicsLineItem*)*lineItem;
        delete item;
    }
}

QRectF PolyLineItem::boundingRect() const
{
    return m_path.controlPointRect();
}

QPainterPath PolyLineItem::shape() const
{
    return m_path;
}

void PolyLineItem::updatePosition()
{
    getNewPoint();
    QLineF line(m_points.top_x, m_points.top_y,m_points.top_x, m_points.mid_y);
    m_vectorLines.at(0)->setLine(line);

    QLineF line1(m_points.top_x, m_points.mid_y,m_points.bottom_x, m_points.mid_y);
    m_vectorLines.at(1)->setLine(line1);

    QLineF line2(m_points.bottom_x, m_points.mid_y,m_points.bottom_x, m_points.bottom_y);
    m_vectorLines.at(2)->setLine(line2);

    QLineF line3(m_points.bottom_x, m_points.bottom_y, m_points.bottom_x - 5 , m_points.bottom_y - 8);
    m_vectorLines.at(3)->setLine(line3);

    QLineF line4(m_points.bottom_x, m_points.bottom_y, m_points.bottom_x + 5 , m_points.bottom_y - 8);
    m_vectorLines.at(4)->setLine(line4);

    m_path.moveTo(m_points.top_x,m_points.top_y);
    m_path.lineTo(m_points.top_x,m_points.mid_y);
    m_path.lineTo(m_points.bottom_x,m_points.mid_y);
    m_path.lineTo(m_points.bottom_x,m_points.bottom_y);
    m_path.moveTo(m_points.bottom_x,m_points.mid_y);
    m_path.lineTo(m_points.bottom_x - 5, m_points.bottom_y - 8);
    m_path.lineTo(m_points.bottom_x + 5, m_points.bottom_y - 8);
}

BaseItem* PolyLineItem::getStartItem()
{
    return m_startItem;
}

BaseItem* PolyLineItem::getEndItem()
{
    return m_endItem;
}

void PolyLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (m_startItem->collidesWithItem(m_endItem))
        return;

    QPen myPen;
    myPen.setColor(m_color);
    painter->setPen(myPen);
    painter->setBrush(m_color);

    //计算折线路径(由3条直线组成)
    QGraphicsLineItem *lineItem;
    foreach (lineItem, m_vectorLines) {
        painter->drawLine(lineItem->line());
    }
    if (isSelected()) {
        painter->setPen(QPen(m_color, 2, Qt::DashLine));
        foreach (lineItem, m_vectorLines) {
           QLineF myLine = lineItem->line();
           myLine.translate(0,4.0);
           painter->drawLine(myLine);
           myLine.translate(0,-8.0);
           painter->drawLine(myLine);
        }
    }

}

void PolyLineItem::getPoint()
{
    //double h = m_startItem->offsetHeight();

    m_points.top_x = m_startItem->pos().rx();
    m_points.top_y = m_startItem->pos().ry()+38;

    m_points.bottom_x = m_endItem->pos().rx();
    m_points.bottom_y = m_endItem->pos().ry() - 24;

    m_points.mid_x = (m_points.bottom_x - m_points.top_x) / 2 + m_points.top_x;
    m_points.mid_y = (m_points.bottom_y - m_points.top_y) / 2 + m_points.top_y;

}

void PolyLineItem::getNewPoint()
{
    //double h = _startItem->offsetHeight();

    QPointF point1 = this->m_vectorLines.at(0)->mapFromItem(m_startItem, 0, 0);
    QPointF point2 = this->m_vectorLines.at(0)->mapFromItem(m_endItem, 0, 0);

    m_points.top_x = point1.x();
    m_points.top_y = point1.y() + 38;

    m_points.bottom_x = point2.x();
    m_points.bottom_y = point2.y() - 24;

    m_points.mid_x = (m_points.bottom_x - m_points.top_x) / 2 + m_points.top_x;
    m_points.mid_y = (m_points.bottom_y - m_points.top_y) / 2 + m_points.top_y;

}
