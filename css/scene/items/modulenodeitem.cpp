#include "modulenodeitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QGraphicsScene>
#include <QDebug>
#include <QTextDocument>
#include <QGraphicsView>


ModuleNodeItem::ModuleNodeItem(QGraphicsItem *parent):
    BaseItem(parent)
{
    setGeometryRect(QRectF(-50, -50, 100, 100));
}

ModuleNodeItem::~ModuleNodeItem()
{

}

int ModuleNodeItem::type() const
{
    return Type;
}

void ModuleNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    return BaseItem::paint(painter,option,widget);
}

void ModuleNodeItem::drawItem(QPainter *painter, const QStyleOptionGraphicsItem *option)
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
    painter->drawEllipse(target);

    painter->setPen(QColor(255,0,0));
    QFont font;
    font.setPixelSize(14);
    font.setFamily(QStringLiteral("微软雅黑"));
    painter->setFont(font);
    painter->drawText(target,Qt::AlignCenter,property("module").toString());

    painter->restore();
}

QPainterPath ModuleNodeItem::shape() const
{
    QPainterPath path;

    path.addRect(QRectF(-20.0, -20.0, 40, 40));

    return path;
}
