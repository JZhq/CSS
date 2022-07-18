/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         基础图元
*@author        luox
*@date          2018-05-25
**/
#include "baseitem.h"
#include "animation/flashengine.h"
#include "graphicsitmshandle.h"
#include "polylineitem.h"
#include "json.h"
#include "graphicsscene.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QToolTip>
#include <Json.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <memory>
#include <QCoreApplication>
#include <QDir>
#include <uiutil.h>
#include <QMessageBox>
#include <QMetaObject>
#include <QFile>
#include <QMenu>
#include <QAction>


BaseItem::BaseItem(QGraphicsItem *parent)
    :QGraphicsObject(parent)
    , m_lineEdit(0)
    , m_isNodeNameEditable(true)
{
    this->setObjectName(QString("BaseItem"));


    m_prepareGeometry = QRectF(0, 0, 0, 0);
    m_selectShapeRect = QRectF(0, 0, 0, 0);

    setZValue(ITEM_Z_COMMON);

    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setAcceptHoverEvents(true);

    //设置可选中
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //设置可移动
    setFlag(QGraphicsItem::ItemIsMovable, true);
    //发送大小区域改变
    setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);

    m_treeData = new TreeNode;
    //    m_color = Qt::gray;
    m_itemStatus = ItemStatus::NotRun;
    m_penWidth = 2;
}

BaseItem::~BaseItem()
{

}

void BaseItem::setID(QString id)
{
    m_treeData->setNodeId(id);
}

QString BaseItem::getID() const
{
    return m_treeData->getNodeId();
}

int BaseItem::type () const
{
    return (QGraphicsItem::UserType+BaseItemType);
}

void BaseItem::setName(const QString &name)
{
    m_treeData->setNodeName(name);
}

void BaseItem::setVarName(const QString &varName)
{
    m_varName = varName;
}

QString BaseItem::getVarName() const
{
    return m_varName;
}

void BaseItem::setNodeStatus(const QString &nodeStatus)
{
    m_nodeStatus = nodeStatus;
}

QString BaseItem::getNodeStatus() const
{
    return m_nodeStatus;
}

QString BaseItem::getName() const
{
    return m_treeData->getNodeName();
}

void BaseItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)

    painter->save();
    //反走样
    painter->setRenderHints(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(m_penWidth);
    // 设置渐变色
    QLinearGradient linear(QPointF(0, 20.0), QPointF(0, -20));

    if(m_itemStatus == ItemStatus::NotRun)
    {
        pen.setColor(QColor(143,143,143));
        linear.setColorAt(0, QColor(109,109,109));
        linear.setColorAt(1, QColor(160,160,160));
    }
    else if(m_itemStatus == ItemStatus::End)
    {
        //        pen.setColor(QColor(6,233,190));
        //        linear.setColorAt(0, QColor(28,162,152));
        //        linear.setColorAt(1, QColor(64,244,188));

        pen.setColor(QColor(173,236,111));
        linear.setColorAt(0, QColor(93,162,25));
        linear.setColorAt(1, QColor(153,223,84));
    }
    else if(m_itemStatus == ItemStatus::Run)
    {
        pen.setColor(QColor(0,156,255));
        linear.setColorAt(0, QColor(30,97,165));
        linear.setColorAt(1, QColor(64,193,243));
    }
    else if(m_itemStatus == ItemStatus::Fault)
    {
        pen.setColor(QColor(255,0,0));
        linear.setColorAt(0, QColor(168,47,32));
        linear.setColorAt(1, QColor(244,64,84));
    }
    painter->setPen(pen);
    painter->setBrush(linear);

    //绘制形状
    drawItem(painter,option);

    painter->setPen(Qt::black);
    QFont font;
    font.setPixelSize(14);
    font.setFamily(QStringLiteral("微软雅黑"));
    QTextOption textOption(Qt::AlignCenter);
    textOption.setWrapMode(QTextOption::WordWrap);
    painter->setFont(font);

    QRectF textRect(-200.0,20.0,400,20);

    if(!m_nodeStatus.isEmpty())
    {
        painter->drawText(textRect,this->property("name").toString(), textOption);
    }
    else
    {
        painter->drawText(textRect,this->property("name").toString(), textOption);
    }


    if (option->state & QStyle::State_Selected)
    {
        // 边框区域颜色
        QColor color = QColor(Qt::magenta);
        // 绘制虚线
        painter->setPen(QPen(color, 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(25, 25, -25, -25));
    }
    painter->restore();

    //return QGraphicsObject::paint(painter,option,widget);
}


QRectF BaseItem::boundingRect() const
{
    if (this->isSelected())
    {
        return m_selectShapeRect;
    }
    else
    {
        return m_prepareGeometry;
    }
}

QRectF BaseItem::selectShapeRect() const
{
    return m_selectShapeRect;
}

QRectF BaseItem::selectedBoundingRect(QRectF boundRect) const
{
    QRectF selectedBoundRect = boundRect;

    if (boundRect.isValid())
    {
        qreal margW = RESIZE_POINT_SIZE / 2;
        QMargins margins(margW, margW, margW, margW);
        selectedBoundRect = boundRect.marginsAdded(margins);
    }

    return selectedBoundRect;
}

void BaseItem::setGeometryRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_prepareGeometry = rect;
}

void BaseItem::sendSelectItem()
{
    //向属性模块发送节点数据
    //向属性模块发送同级节点数据 by zyg
    if(m_treeData->getParentItem())
    {
        QList<TreeNode*> t_friend = m_treeData->getParentItem()->getChildren();
        QList<PublicData::PelItemData> t_friendData;
        t_friendData.clear();
        foreach (TreeNode* t_singleFriend, t_friend)
        {
            PublicData::PelItemData t_singleItemData = t_singleFriend->getItemData();
            if(t_singleItemData == m_treeData->getItemData())
            {
                continue;
            }
            t_friendData.append(t_singleItemData);
        }
        GraphicsItemsHandle::getInstance()->sendSelectItemWithFriend(m_treeData->getItemData(), t_friendData);
    }

    else
    {
        GraphicsItemsHandle::getInstance()->sendSelectItem(m_treeData->getItemData());
    }

    //向展示界面发送节点id
    emit signalNodeId(m_treeData->getNodeId());
    this->update();
}

void BaseItem::sendDisplaySelectItem()
{
    //向展示界面发送节点id
    emit signalNodeId(m_treeData->getNodeId());
    this->update();
}

void BaseItem::setItemType(const ItemAndPropertyType &type)
{
    m_treeData->setNodeType(type);
}

ItemAndPropertyType BaseItem::getItemType() const
{
    return m_treeData->getNodeType();
}

void BaseItem::addPolyLine(PolyLineItem *lineItem)
{
    m_lines.append(lineItem);
}

void BaseItem::setPolyLineColor(const QColor &color)
{
    foreach (PolyLineItem *lineItem, m_lines) {
        if(lineItem)
        {
            lineItem->setColor(color);
        }
    }
}

void BaseItem::removePolyLine(PolyLineItem *lineItem)
{
    int index = m_lines.indexOf(lineItem);

    if (index != -1)
        m_lines.removeAt(index);
}

void BaseItem::removeAllPolyLine()
{
    foreach (PolyLineItem *line, m_lines) {

        line->getStartItem()->removePolyLine(line);
        line->getEndItem()->removePolyLine(line);

        scene()->removeItem(line);
        delete line;
        line = NULL;
    }
}

QList<QGraphicsObject *> BaseItem::getAllPolyLine()
{
    QList<QGraphicsObject *>  items;
    foreach (PolyLineItem *line, m_lines) {
        items.append(qobject_cast<QGraphicsObject *>(line));
    }
    return items;
}

TreeNode *BaseItem::getTreeData()
{
    return m_treeData;
}

void BaseItem::setTreeData(const PublicData::PelItemData &data)
{
    m_treeData->setItemData(data);
    //将json串中变量名提取出来放在节点的属性中以便绘制到图元上
    Json json(data.nodeProperty);

    QString t_varName = json.getString(QStringLiteral("变量名"));
    setProperty("varName",t_varName);
}

void BaseItem::setNodeProperty(const QString &strProperty)
{
    m_treeData->setNodeProperty(strProperty);
}

void BaseItem::setItemSatus(const ItemStatus &status)
{
    m_itemStatus = status;
}

void BaseItem::updateTooltip()
{

    QString t_str = this->assembleToolTipStr();
    setToolTip(t_str);
}

QVariant BaseItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    //！改变选中的返回图元的有效区域
    if (change & QGraphicsItem::ItemSelectedHasChanged)
    {
        prepareGeometryChange();
        m_selectShapeRect = m_prepareGeometry;
    }

    //!更新折线段和图元的位置信息
    if (change == QGraphicsItem::ItemPositionChange) {
        //更新折线段的位置信息
        foreach (PolyLineItem *line, m_lines) {
            line->updatePosition();
            CommonData::isTreeEdited = true;
        }
        //更新图元的位置信息
        m_treeData->setNodePos(this->pos());
    }
    return  QGraphicsObject::itemChange(change, value);;
}

void BaseItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // qDebug()<<"hoverEnterEvent";
    m_penWidth = 4;
    QString t_str = assembleToolTipStr();
    setToolTip(t_str);

    return QGraphicsObject::hoverEnterEvent(event);
}

void BaseItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_penWidth = 2;
    return QGraphicsObject::hoverLeaveEvent(event);
}

void BaseItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    setSelected(true);
    QAction *deleteAction = new QAction(QStringLiteral("删除(Ctrl+D)"), this);
    QAction *copyAction = new QAction(QStringLiteral("复制(Ctrl+C)"), this);

    connect(deleteAction, &QAction::triggered, this, [this]() {
        emit signalDelNode();
    });

    connect(copyAction, &QAction::triggered, this, [this]() {
        emit signalCopyNode();
    });

    menu.addAction(deleteAction);
    menu.addAction(copyAction);
    menu.exec(event->screenPos());
}

QString BaseItem::assembleToolTipStr()
{
    return {};
}

void BaseItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (isNodeNameEditable())
    {
        showNameEditor();
    }

    QGraphicsObject::mouseDoubleClickEvent(event);
}

FocusLineEdit::FocusLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setWindowModality(Qt::ApplicationModal);
}

void FocusLineEdit::focusOutEvent(QFocusEvent *)
{
    Q_EMIT focusLost();
}

void BaseItem::showNameEditor()
{
    if(!m_lineEdit)
    {
        m_lineEdit = new FocusLineEdit(scene()-> views().first());
        m_lineEdit->setWindowFlag(Qt::FramelessWindowHint, true);
        connect(m_lineEdit, &FocusLineEdit::returnPressed, this, &BaseItem::onNameEditorChanged);
        connect(m_lineEdit, &FocusLineEdit::focusLost, this, &BaseItem::onHideNameEditor);
        m_lineEdit->resize(120, 40);
    }
    QGraphicsView * v = scene()-> views().first();
    QPointF sceneP = mapToScene(boundingRect().topLeft()); //boundingRect().topLeft()
    QPoint viewP = v->mapFromScene(sceneP);
    m_lineEdit->setGeometry(viewP.x() + boundingRect().width()/2-m_lineEdit->width()/2,
                            viewP.y() + boundingRect().height()/2-m_lineEdit->height()/2,
                            m_lineEdit->width(),
                            m_lineEdit->height());
    m_lineEdit->setText(getName());
    m_lineEdit->show();
    m_lineEdit->setFocus();
}



void BaseItem::onHideNameEditor()
{
    m_lineEdit->hide();
}

#include "common/propertycommon.h"
void BaseItem::onNameEditorChanged()
{
    //保存
    //ManagerItemPropertyInstane->setProperty()

    // Q_EMIT GZZD::PRIMITIVEPROPERTYS::PropertyBrowerInstance::getPropertyWidget()->propertyChanged(PrimitivePropertyConstants::ITEMNAME, m_lineEdit->text());
    sendSelectItem();

    m_lineEdit->hide();
}

