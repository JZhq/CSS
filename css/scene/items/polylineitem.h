/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         折线段图元
*@author        luox
*@date          2018-05-23
**/
#ifndef POLYLINEITEM_H
#define POLYLINEITEM_H

#include <QString>
#include <QVariantHash>
#include <QGraphicsObject>

#include "common/configureglobal.h"
#include "baseitem.h"

typedef struct _PolyPoints{

    _PolyPoints(){
        top_x = 0.0;
        top_y = 0.0;
        mid_x = 0.0;
        mid_y = 0.0;
        bottom_x = 0.0;
        bottom_y = 0.0;
    }
    void operator = (const _PolyPoints& other)
    {
        this->top_x = other.top_x;
        this->top_y = other.top_y;
        this->mid_x = other.mid_x;
        this->mid_y = other.mid_y;
        this->bottom_x = other.bottom_x;
        this->bottom_y = other.bottom_y;
    }

    qreal top_x ;
    qreal top_y ;
    qreal mid_x ;
    qreal mid_y ;
    qreal bottom_x;
    qreal bottom_y;

}PolyPoints;

class PolyLineItem: public QGraphicsObject
{
    Q_OBJECT
public:

    enum { Type = QGraphicsItem::UserType + PolyLine};

    explicit PolyLineItem(BaseItem *startItem,BaseItem *endItem,QGraphicsItem *parent = 0);
    ~PolyLineItem();

    int type() const Q_DECL_OVERRIDE {return Type;}
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    /**
     * @brief setColor 设置折线段的颜色值
     * @param color
     */
    void setColor(const QColor &color){m_color = color;}
    /**
     * @brief updatePosition
     */
    void updatePosition();

    BaseItem* getStartItem();

    BaseItem* getEndItem();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    void getPoint();
    void getNewPoint();
private:
    BaseItem *m_startItem;//起始节点
    BaseItem *m_endItem;//结束节点
    QColor m_color;//线的颜色
    PolyPoints m_points;//折线段上的点

    QVector<QGraphicsLineItem*> m_vectorLines;//折线段中保存的直线
    QPainterPath m_path;//折线段的路径区域
};

#endif // POLYLINEITEM_H
