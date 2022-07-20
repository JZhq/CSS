/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         所有图元的基类
*@author        luox
*@date          2018-05-22
**/
#ifndef BASEITEM_H
#define BASEITEM_H

#include <QGraphicsObject>
#include <QLineEdit>
#include <QPen>
#include "itemglobal.h"
#include "animation/animationstruct.h"
#include "common/treenode.h"

class QGraphicsScene;
class FlashEngine;
class PolyLineItem;
class Json;
// 缩放点大小（长宽都一样）
#define RESIZE_POINT_SIZE   6

class FocusLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    FocusLineEdit(QWidget* parent = 0);
protected:
    void focusOutEvent(QFocusEvent *);

Q_SIGNALS:
    void focusLost();
};

// 该类图元类的基类
class BaseItem : public QGraphicsObject
{
    Q_OBJECT

signals:
    //删除节点
    void signalDelNode();
    //节点的id
    void signalNodeId(const QString &id);
    //复制节点
    void signalCopyNode();

    void signalExpandBaseNode(const QString &id, const QString &name);

    // void expandBaseKnowNode(const QString &baseNodeId);
    //属性改变
    //void propertyChanged(const QString &property, const QVariant &value);

public:
    // id
    Q_PROPERTY(QString ItemID READ getID WRITE setID)
    // name
    Q_PROPERTY(QString name READ getName WRITE setName)
    // Variable name
    Q_PROPERTY(QString varName READ getVarName WRITE setVarName)
    // node status
    Q_PROPERTY(QString nodeStatus READ getNodeStatus WRITE setNodeStatus)

    /**
     * @brief BaseItem 构造函数
     * @param parent   父节点
     */
    explicit BaseItem(QGraphicsItem *parent = 0);

    /**
     * @brief ~BaseItem 析构函数
     */
    virtual ~BaseItem();

    /**
     * @brief 设置ID号
     * @param id
     */
    void setID(QString id);

    /**
     * @brief 获取ID号
     * @return
     */
    QString getID() const;

    /****************************************
     * Function:    type
     * Description: 获取图元类型,设置类型，可以根据类型
     *              转换函数进行图元直接的转换
     * Input:       无
     * Output:      无
     * Return:      图元类型
     ****************************************/
    virtual int	type () const;

    /**
     * @brief 设置图元名称
     * @param name
     */
    void setName(const QString &name);

    /**
     * @brief 获取图元名称
     * @param name
     * @return
     */
    QString getName() const;

    /**
     * @brief 设置变量名
     */
    void setVarName(const QString &varName);

    /**
     * @brief 获取变量名
     * @return
     */
    QString getVarName() const;

    /**
     * @brief 设置节点状态
     */
    void setNodeStatus(const QString &nodeStatus);

    /**
     * @brief 获取节点状态
     * @return
     */
    QString getNodeStatus() const;

    /**
     * @brief 图元绘制类，由各个具体图元类实现
     * @param painter
     * @param option
     * @param widget
     */
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    /**
     * @brief 绘制具体图元函数，由具体的图元类实现
     * @param painter
     */
    virtual void drawItem(QPainter * painter,const QStyleOptionGraphicsItem * option) = 0;

    /****************************************
     * Function:    boundingRect
     * Description: 返回图元区域
     * Input:       无
     * Output:      无
     * Return:      图元矩形区域
     ****************************************/
    virtual QRectF boundingRect() const;

    /**********************************************************************
     * Function:    selectShapeRect
     * Description: 选择shape矩形框
     * Input:       void
     * Output       void
     * Return:      void
     **********************************************************************/
    virtual QRectF selectShapeRect() const;

    /****************************************
     * Function:    selectedBoundingRect
     * Description: 返回被选中后的图元区域
     * Input:       无
     * Output:      无
     * Return:      图元矩形区域
     ****************************************/
    QRectF selectedBoundingRect(QRectF boundRect) const;

    /**
     * @brief 设置渲染区域
     * @param rect
     */
    virtual void setGeometryRect(const QRectF &rect);
    /**
     * @brief 编辑界面发射选中动作
     */
    void sendSelectItem();
    /**
     * @brief 展示界面发射选中动作
     */
    void sendDisplaySelectItem();

    /**
    * @brief 设置节点名称是否可编辑
    */
    bool isNodeNameEditable() const { return m_isNodeNameEditable; }
    void setNodeNameEditable(bool val) { m_isNodeNameEditable = val; }
public:
    /**
     * @brief setItemType 设置图元的类型
     * @param type
     */
    void setItemType(const ItemAndPropertyType &type);
    /**
     * @brief getItemType 获取图元的类型
     * @return
     */
    ItemAndPropertyType getItemType() const;
    /**
     * @brief 添加折线段
     * @param lineItem
     */
    void addPolyLine(PolyLineItem *lineItem);
    /**
     * @brief 设置折线段的颜色
     * @param color
     */
    void setPolyLineColor(const QColor &color);
    /**
     * @brief 移除折线段
     * @param lineItem
     */
    void removePolyLine(PolyLineItem *lineItem);
    /**
     * @brief 删除该节点下所有的折线段
     */
    void removeAllPolyLine();

    //!
    //! \brief 获取全部的折线段
    //! \return
    //!
    QList<QGraphicsObject *> getAllPolyLine();
    /**
     * @brief 获取节点数据
     * @return
     */
    TreeNode * getTreeData();
    /**
     * @brief 设置节点数据
     */
    void setTreeData(const PublicData::PelItemData& data);
    /**
     * @brief 设置节点属性
     * @param strProperty
     */
    void setNodeProperty(const QString& strProperty);
    /**
     * @brief 设置图元的颜色
     * @param color
     */
//    void setItemColor(const QColor &color);
    /**
     * @brief 设置图元的状态
     * @param status
     */
    void setItemSatus(const ItemStatus &status);

    /**
     * @brief 更新提示
     * @param tooltip
     */
    void updateTooltip();

    /**
     * @brief 设置节点数据
     * @param setItemData
     */
    void setNodeData(const QVariantHash &data);

    /**
     * @brief 获取节点数据
     * @param nodeData
     */
    QVariantHash nodeData() const;
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;

    //virtual void mousePressEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;
    //鼠标移入事件
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    //鼠标离开事件
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    //右键菜单
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
    //双击事件
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief 组装tooltip显示的内容
     * @return
     */
    virtual QString assembleToolTipStr();
private:
    /**
     * @brief 显示名称编辑框
     * @param
     */
    void showNameEditor();


private Q_SLOTS:
    /**
     * @brief 隐藏名称编辑框
     * @param
     */
    void onHideNameEditor();

    /**
     * @brief 应用名称改变
     * @param
     */
    void onNameEditorChanged();

protected:
    TreeNode *m_treeData = nullptr; //保存节点的树形结构

    QRectF m_prepareGeometry;       // geometry
    QRectF m_selectShapeRect;       // select shape rect

    QList<PolyLineItem *> m_lines;      // 节点所有的折线段
    ItemStatus m_itemStatus;            // 图元的状态
    QString m_varName;                  // 变量名
    QString m_nodeStatus;
    qreal   m_penWidth;                 // 画笔的宽度
    FocusLineEdit * m_lineEdit = nullptr;

    bool m_isNodeNameEditable;          // 可编辑名称

    QVariantHash m_data;
};

#endif // BASEITEM_H
