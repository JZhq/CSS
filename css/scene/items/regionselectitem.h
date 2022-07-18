#ifndef REGIONSELECTITEM_H
#define REGIONSELECTITEM_H

#include "baseitem.h"

// 图元绘制的步骤
typedef enum DrawSelToolStep
{
    DRAWSELTOOLSTEP_INVALID,
    DRAWSELTOOLSTEP_1,
    DRAWSELTOOLSTEP_2,
    DRAWSELTOOLSTEP_FINISH,
}enumDrawSelToolStep;

class GraphicsScene;
class RegionSelectItem: public BaseItem
{
public:
    enum { Type = QGraphicsItem::UserType + ModuleType};

    explicit RegionSelectItem(QGraphicsItem *parent = 0);

    /**********************************************************************
     * Function:    setStep
     * Description: 设置选中的步骤
     * Input:       步骤
     * Output       void
     * Return:      void
     **********************************************************************/
    void setStep(DrawSelToolStep step);

    /**********************************************************************
     * Function:    setStep
     * Description: 当前的步骤
     * Input:       void
     * Output       void
     * Return:      void
     **********************************************************************/
    enumDrawSelToolStep step() const;

    /**********************************************************************
     * Function:    selectItems
     * Description: 选中区域内的图元
     * Input:       场景
     * Output       void
     * Return:      void
     **********************************************************************/
    void selectItems(GraphicsScene *pScene);

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
    virtual void drawItem(QPainter * painter,const QStyleOptionGraphicsItem * option);

    /****************************************
     * Function:    shape
     * Description: 返回图元形状
     * Input:       无
     * Output:      无
     * Return:      图元形状
     ****************************************/
    QPainterPath shape() const;

    /****************************************
     * Function:    startPointX
     * Description: 设置开始点x坐标
     * Input:       startPointX：开始点x坐标
     * Output:      无
     * Return:      无
     ****************************************/
    virtual void setStartPointX(float startPointX);

    /****************************************
     * Function:    getStartPointX
     * Description: 获取开始点X坐标
     * Input:       无
     * Output:      无
     * Return:      开始点x坐标
     ****************************************/
    virtual float getStartPointX() const;

    /****************************************
     * Function:    setStartPointY
     * Description: 设置开始点y坐标
     * Input:       startPointY：开始点y坐标
     * Output:      无
     * Return:      无
     ****************************************/
    virtual void setStartPointY(float startPointY);

    /****************************************
     * Function:    getStartPointY
     * Description: 获取开始点Y坐标
     * Input:       无
     * Output:      无
     * Return:      开始点y坐标
     ****************************************/
    virtual float getStartPointY() const;

    /****************************************
     * Function:    setEndPointX
     * Description: 设置结束点x坐标
     * Input:       endPointX：结束点x坐标
     * Output:      无
     * Return:      无
     ****************************************/
    virtual void setEndPointX(float endPointX);

    /****************************************
     * Function:    getEndPointX
     * Description: 获取结束点x坐标
     * Input:       无
     * Output:      无
     * Return:      结束点x坐标
     ****************************************/
    virtual float getEndPointX() const;

    /****************************************
     * Function:    setEndPointY
     * Description: 设置结束点y坐标
     * Input:       endPointY：结束点y坐标
     * Output:      无
     * Return:      无
     ****************************************/
    virtual void setEndPointY(float endPointY);

    /****************************************
     * Function:    getEndPointY
     * Description: 获取结束点y坐标
     * Input:       无
     * Output:      无
     * Return:      结束点y坐标
     ****************************************/
    virtual float getEndPointY() const;

    /****************************************
     * Function:    boundingRect
     * Description: 矩形区域
     * Input:       无
     * Output:      无
     * Return:      无
     ****************************************/
    QRectF boundingRect() const;

    /**********************************************************************
     * Function:    prepareGeometry
     * Description: 计算geometry
     * Input:       void
     * Output       void
     * Return:      void
     **********************************************************************/
    virtual void prepareGeometry();

protected:
    QPointF m_startPoint;   // 开始点
    QPointF m_endPoint;     // 结束点
    DrawSelToolStep m_eStep;
};

#endif // REGIONSELECTITEM_H
