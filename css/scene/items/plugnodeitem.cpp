#include "plugnodeitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QGraphicsScene>
#include <QDebug>
#include <QTextDocument>
#include <QGraphicsView>


PlugNodeItem::PlugNodeItem(QGraphicsItem *parent):
    BaseItem(parent)
{
    setGeometryRect(QRectF(-50, -50, 100, 100));
}

PlugNodeItem::~PlugNodeItem()
{

}

int PlugNodeItem::type() const
{
    return Type;
}

void PlugNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    return BaseItem::paint(painter,option,widget);
}

void PlugNodeItem::drawItem(QPainter *painter,const QStyleOptionGraphicsItem *option)
{    

    painter->save();

//    QPen pen;
//    pen.setWidth(2);
//    pen.setColor(QColor(86,197,255));
//    painter->setPen(pen);


    //切换item坐标系原点到渲染区域中心
    QRectF rect = this->boundingRect();
    painter->translate(rect.center());


    QRectF target(-20.0, -20.0, 40, 40);
    QPainterPath path;
    path.moveTo(QPointF(0,-20));
    path.lineTo(QPointF(-20,0));
    path.lineTo(QPointF(-10,20));
    path.lineTo(QPointF(10,20));
    path.lineTo(QPointF(20,0));
    path.closeSubpath();
    painter->drawPath(path);

    //painter->drawEllipse(target);

    painter->setPen(QColor(255,255,255));
    QFont font;
    font.setPixelSize(14);
    font.setFamily(QStringLiteral("微软雅黑"));
    painter->setFont(font);
    painter->drawText(target,Qt::AlignCenter,property("varName").toString());

    painter->restore();
}

QPainterPath PlugNodeItem::shape() const
{
    QPainterPath path;

    path.addRect(QRectF(-20.0, -20.0, 40, 40));

    return path;
}
