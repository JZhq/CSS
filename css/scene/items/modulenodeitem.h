/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         “叶子”节点绘制
*@author        luox
*@date          2018-05-23
**/
#ifndef LEAFNODEITEM_H
#define LEAFNODEITEM_H

#include <QString>

#include <QVariantHash>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include "baseitem.h"
#include "itemglobal.h"

class ModuleNodeItem: public BaseItem
{
    Q_OBJECT
public:

    enum { Type = QGraphicsItem::UserType + ModuleType};

    /**
     * @brief ModuleNodeItem
     * @param parent
     */
    explicit ModuleNodeItem(QGraphicsItem *parent = 0);
    virtual ~ModuleNodeItem();

    /****************************************
     * Function:    type
     * Description: 返回图元类型
     * Input:       无
     * Output:      无
     * Return:      该图元的类型
     ****************************************/
    virtual int type() const Q_DECL_OVERRIDE;

    /****************************************
     * Function:    paint
     * Description: 图元绘制类，由各个具体图元类实现
     * Input:       painter:画笔
     *              option:样式
     *              widget:绘制部件
     * Output:      无
     * Return:      无
     ****************************************/
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    /****************************************
     * Function:    drawItem
     * Description: 绘制具体图元函数，由具体的图元类实现
     * Input:       painter:画笔
     * Output:      无
     * Return:      无
     ****************************************/
    virtual void drawItem(QPainter * painter,const QStyleOptionGraphicsItem *option);

    /****************************************
     * Function:    shape
     * Description: 返回图元形状
     * Input:       无
     * Output:      无
     * Return:      图元形状
     ****************************************/
    QPainterPath shape() const;
};

#endif // LEAFNODEITEM_H
