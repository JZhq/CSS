#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QApplication>
#include <QGraphicsItem>
#include <QPainter>
#include <QMessageBox>
#include <QUuid>
#include <QDebug>
#include <QGraphicsGridLayout>
#include <QToolButton>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QGraphicsView>
#include <QRegularExpression>
#include <QUndoStack>
#include <QJsonDocument>

#include "graphicsscene.h"
#include "itemsfactory/itemsfactory.h"
#include "baseitem.h"
#include "graphicsitmshandle.h"
#include "common/propertycommon.h"
#include "polylineitem.h"
#include "json.h"
#include "common/commondata.h"
#include "regionselectitem.h"
#include "commands.h"
#include "itemsoperationtool.h"

using namespace PublicData;

GraphicsScene::GraphicsScene(QObject *parent):QGraphicsScene(parent)
{
    m_selectedPointCount = 0;
    m_mode = InsertItem;
    m_line = Q_NULLPTR;
    m_regionSelItem = Q_NULLPTR;
    m_editMode = FaultEdit;
    m_undostak = new QUndoStack(this);

    handleEvent();
}

void GraphicsScene::onSaveEditTree()
{
    QList<QGraphicsItem *> listItems = this->items();
    int nullCount = 0;
    int rootCount = 0;
    BaseItem *rootItem = Q_NULLPTR;
    BaseItem *baseItem = Q_NULLPTR;

    // 基础知识树，只包含一个描述节点
    int branchCount = 0;

    foreach (QGraphicsItem *item, listItems) {
        baseItem = dynamic_cast<BaseItem *>(item);
        if(baseItem)
        {
            if (baseItem->getTreeData()->getParentItem() == Q_NULLPTR && baseItem->getTreeData()->getChildren().size() == 0) {
                ++nullCount;
            }
            if( baseItem->getTreeData()->getParentItem() == Q_NULLPTR)
            {
                //这里需要判断场景中的根节点是否只有一个
                ++rootCount;
                rootItem = baseItem;
            }

            //验证算法表达式
            if(baseItem->getItemType() == ItemAndPropertyType::PlugNode)
            {
                Json json(baseItem->getTreeData()->getItemData().nodeProperty);
                QString t_pluginName = json.getString(QStringLiteral("插件名称"));
            }
            // 验证描述节点
            if (baseItem->getItemType() == ItemAndPropertyType::BranchNode)
            {
                branchCount++;
                Json json(baseItem->getTreeData()->getItemData().nodeProperty);
            }
        }
    }

    if (listItems.size() != 1 && nullCount > 1) {
        QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("存在未连接的节点"));
        return;
    }

    if (rootItem == Q_NULLPTR) {
        QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("根节点不存在，不能保存"));
        return;
    }

    if(rootCount == 1 && rootItem)
    {
        if(rootItem->getItemType() != ItemAndPropertyType::BranchNode)
        {
            QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("根节点不正确，请修改后保存"));
            return;
        }
        else
        {
            //向服务端更新故障树属性
            GraphicsItemsHandle::getInstance()->sendUpdateProjectPropertyReq(m_checkedItemData, PublicData::TreeOperationType::Save);
        }
    }
    else if(rootCount == 0 && rootItem == Q_NULLPTR)
    {
        //向服务端更新故障树属性
        GraphicsItemsHandle::getInstance()->sendUpdateProjectPropertyReq(m_checkedItemData, PublicData::TreeOperationType::Save);
    }
    else
    {
        QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("树形结构不正确，请修改后保存"));
        return;
    }

    //将缓存已被删除的节点推送到服务端删除
    if(m_checkedTreeData.auditStatus != 0)
    {
        if (!m_deleteNodeId.isEmpty())
            GraphicsItemsHandle::getInstance()->sendDeleteTreeNode(m_deleteNodeId);
    }

    //将界面中已经删除的节点id保存到界面中，如果点击了保存，则推送到服务端
    m_deleteNodeId.clear();

}

void GraphicsScene::onReloadEditTree(TreeNode *rootTreeData)
{
    //清空编辑区
    onClearEditView();

    //未加载到数据
    if(Q_NULLPTR == rootTreeData || rootTreeData->getItemData().id.isEmpty())
        return;
    //根节点
    PublicData::PelItemData itemData;
    QList<TreeNode*> childrenTreeData = rootTreeData->getChildren();
    itemData = rootTreeData->getItemData();
    BaseItem *itemRoot = ItemsFactory::instance()->creatItem(itemData.itemType);
    if(Q_NULLPTR == itemRoot)
        return;
    m_rootItem = itemRoot;
    itemRoot->setPos(itemData.pos);

    //设置节点的数据域
    itemRoot->setTreeData(itemData);
    //添加节点
    addItem(itemRoot);
    //连接删除槽
    connect(itemRoot, &BaseItem::signalDelNode, this, &GraphicsScene::onDeleteTreeNode, Qt::UniqueConnection);
    //连接复制槽
    connect(itemRoot, &BaseItem::signalCopyNode, this, &GraphicsScene::onCopyTreeNode, Qt::UniqueConnection);

    //子节点
    reloadChildrenTreeData(itemRoot,childrenTreeData);

    //自动设置场景的区域（只适用低性能场景）
    setSceneRect(this->itemsBoundingRect());
    //    qDebug() << "finished load all nodes:"<< time.elapsed() << "\n";
}

void GraphicsScene::onLoadDragTreeNodes(TreeNode *rootTreeData)
{
    qDebug()<<"onLoadDragTreeNodes";
    //唯一区别是不清空编辑区

    //未加载到数据
    if(Q_NULLPTR == rootTreeData)
        return;

    QList<QGraphicsObject*> items;

    //根节点
    PublicData::PelItemData itemData;
    QList<TreeNode*> childrenTreeData = rootTreeData->getChildren();
    if(childrenTreeData.isEmpty())
        return;
    itemData = rootTreeData->getItemData();

    //更新所属树的id
    itemData.treeId = m_checkedTreeData.id;

    //重新设置位置(根节点位置为拖拽释放的位置)
    QPointF t_adjustPoint = m_dragPoint - itemData.pos;
    itemData.pos = m_dragPoint;

    BaseItem *itemRoot = ItemsFactory::instance()->creatItem(itemData.itemType);
    if(Q_NULLPTR == itemRoot)
        return;
    itemRoot->setPos(itemData.pos);

    //设置节点的数据域
    itemRoot->setTreeData(itemData);
    //添加节点
    // addItem(itemRoot);
    items.append(itemRoot);
    //连接删除槽
    connect(itemRoot, &BaseItem::signalDelNode, this, &GraphicsScene::onDeleteTreeNode, Qt::UniqueConnection);
    //连接复制槽
    connect(itemRoot, &BaseItem::signalCopyNode, this, &GraphicsScene::onCopyTreeNode, Qt::UniqueConnection);

    //子节点
    loadDragChildrenTreeData(itemRoot,childrenTreeData,t_adjustPoint, items);
    addItemsWithUndo(items);
}

void GraphicsScene::onDeleteTreeNode()
{
    CommonData::isTreeEdited = true;
    removeItemsWithUndo(this->selectedItems());
    //自动设置场景的区域（只适用低性能场景）
    setSceneRect(this->itemsBoundingRect());

}

void GraphicsScene::onCopyTreeNode()
{
    return;
    BaseItem *node = Q_NULLPTR;
    m_copyNodeDatas.clear();
    foreach (QGraphicsItem *item, this->selectedItems())
    {
        node = dynamic_cast<BaseItem *>(item);
        if(Q_NULLPTR != node)
        {
            //获取该节点的数据域
            PublicData::PelItemData itemData = node->getTreeData()->getItemData();

            m_copyNodeDatas.append(itemData);
        }
    }
}

void GraphicsScene::onPasteTreeNode()
{
    return;
    CommonData::isTreeEdited = true;

    QPoint t_point;
    QPointF t_scene;

    QList<QGraphicsView *> listViews = this->views();
    foreach (QGraphicsView *view, listViews) {
        //转换坐标
        t_point = view->mapFromGlobal(QCursor::pos());
        t_scene = view->mapToScene(t_point);
    }

    QList<QGraphicsObject*> items;
    foreach (PublicData::PelItemData itemData, m_copyNodeDatas) {
        PublicData::PelItemData t_itemData = itemData;
        if (t_itemData.itemType == ItemAndPropertyType::TestNode &&
                m_checkedTreeData.treeType == PublicData::TreeType::TaskAnalyTree) {
            QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("任务分析树中不允许挂载测试节点"));
            return;
        }
        if (t_itemData.itemType == ItemAndPropertyType::PlugNode &&
                m_checkedTreeData.treeType == PublicData::TreeType::TaskAnalyTree) {
            QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("任务分析树中不允许挂载插件节点"));
            return;
        }
        if (t_itemData.itemType == ItemAndPropertyType::PlugNode &&
                m_checkedTreeData.treeType == PublicData::TreeType::SysDiagnoseTree) {
            QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("系统诊断故障树中不允许挂载插件节点"));
            return;
        }
        BaseItem *item = ItemsFactory::instance()->creatItem(itemData.itemType);
        if (itemData.itemType == ItemAndPropertyType::BaseKnowledgeNode){
            QStringList ids = t_itemData.id.split("_");
            t_itemData.id = QString("%1_%2").arg(ids.at(0)).arg(QUuid::createUuid().toString());
        }
        else
            t_itemData.id = QUuid::createUuid().toString();
        t_itemData.parentId = "";
        t_itemData.pos = QPointF();
        //重新指定树的id
        t_itemData.treeId = m_checkedTreeData.id;
        //连接删除槽
        connect(item, &BaseItem::signalDelNode, this, &GraphicsScene::onDeleteTreeNode, Qt::UniqueConnection);
        //连接复制槽
        connect(item, &BaseItem::signalCopyNode, this, &GraphicsScene::onCopyTreeNode, Qt::UniqueConnection);

        item->setPos(t_scene);

        // addItem(item);
        items.append(item);
        item->setTreeData(t_itemData);

        //移动粘贴点
        t_scene += QPointF(50,0);
    }
    addItemsWithUndo(items);
}

void GraphicsScene::onClearEditView()
{
    m_regionSelItem = Q_NULLPTR;
    this->clear();
    m_undostak->clear();
    qDebug() << "current undostack size:" << m_undostak->count();
}

void GraphicsScene::onSetNodesProperty(const QVariantHash& hash)
{
    //*按照节点的ID找到节点，然后将属性的json字符串设置到节点的数据域中
    //hash<id,属性json串>
    BaseItem *node = Q_NULLPTR;
    foreach (QGraphicsItem *item, this->items())
    {
        node = dynamic_cast<BaseItem *>(item);
        if(Q_NULLPTR != node)
        {
            QString id = node->getID();
            QHashIterator<QString, QVariant> iter(hash);
            while (iter.hasNext()) {
                iter.next();
                if(iter.key() == id)
                {
                    node->setNodeProperty(iter.value().toString());
                    node->setFocus();
                    break;
                }
            }
        }
    }
}

void GraphicsScene::onDrawItemName(const QString &id, const QString &name)
{
    qDebug()<<"onDrawItemName";
    //按照id找到界面上的图元
    QList<QGraphicsItem *> listItems = this->items();
    foreach (QGraphicsItem *item, listItems) {
        BaseItem *baseItem = dynamic_cast<BaseItem *>(item);
        if(baseItem)
        {
            QString t_itemId = baseItem->property("ItemID").toString();
            if(t_itemId == id)
            {
                baseItem->setProperty("name",name);
                baseItem->update();
            }
        }
    }
}

void GraphicsScene::onDrawItemVarName(const QString &id, const QString &varName)
{
    qDebug()<<"onDrawItemVarName";

    QList<QGraphicsItem *> listItems = this->items();
    foreach (QGraphicsItem *item, listItems) {
        BaseItem *baseItem = dynamic_cast<BaseItem *>(item);
        if(baseItem)
        {
            QString t_itemId = baseItem->property("ItemID").toString();
            if(t_itemId == id)
            {
                baseItem->setProperty("varName",varName);
                baseItem->update();
            }
        }
    }
}

void GraphicsScene::onDrawItemVarError(const QString &varName)
{
    if(!varName.isEmpty())
    {
        QString t_notice = QStringLiteral("变量名") + QString("'%0'").arg(varName) + QStringLiteral("与同级节点其他变量名重复，请检查！");
        QMessageBox::information(NULL,QStringLiteral("提示"),t_notice);
        return;
    }
}

/**
*@brief      设置场景的模式
*@parameter  Mode:场景的模式(枚举值）
*@return
**/
void GraphicsScene::setMode(int mode)
{
    m_mode = (GraphicsScene::Mode)mode;
}

void GraphicsScene::onSetFaultTreeData(const QVariantHash &itemData)
{
    //清除上颗树需要删除的节点
    m_deleteNodeId.clear();
    m_checkedItemData = itemData;
}

void GraphicsScene::updateFaultTreeData(const QVariantHash &treeData)
{
    //只更新变动的值
    m_checkedItemData = treeData;
}

/**
*@brief      场景鼠标点击事件
*@parameter  QGraphicsSceneMouseEvent
*@return
**/
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    QPointF  point = mouseEvent->scenePos();
    BaseItem *item = dynamic_cast<BaseItem*>(itemAt(point,QTransform()));
    if (item == nullptr)
    {
        return QGraphicsScene::mousePressEvent(mouseEvent);
    }

    if(mouseEvent->button() == Qt::LeftButton && QApplication::keyboardModifiers() != Qt::ControlModifier && !item && m_mode != InsertLine)
    {
        // 没有按住ctrl时，框选
        m_regionSelItem = new RegionSelectItem;
        addItem(m_regionSelItem);
        m_regionSelItem->setStep(DrawSelToolStep::DRAWSELTOOLSTEP_1);
        m_regionSelItem->setStartPointX(point.x());
        m_regionSelItem->setStartPointY(point.y());
        m_regionSelItem->setEndPointX(point.x());
        m_regionSelItem->setEndPointY(point.y());
        m_regionSelItem->update();
    }
    if (mouseEvent->button() == Qt::LeftButton) {
        m_curItem = item;
        switch (m_mode) {
        case InsertItem:  //检测鼠标点是否在图元上
            if (item)
            {
                auto allSelItems = selectedItems();
                m_oldMovePos = item->pos();
                m_selectedPointCount = allSelItems.size() == 0 ? 1 : allSelItems.size();
                qDebug() << "selected item count :" << m_selectedPointCount << " pos:" << m_oldMovePos;
            }
            break;

        case InsertLine:  //检测鼠标点是否在图元上
            if (item)
            {
                m_line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),mouseEvent->scenePos()));
                m_line->setPen(QPen(Qt::black, 1));
                addItem(m_line);
            }

            break;
        default:
            break;
            ;
        }

        if(item)
        {
            //发送图元的属性到属性模块
            item->sendSelectItem();
        }
        else
        {
            //发送故障树属性到属性模块
            if(!m_checkedTreeData.id.isEmpty() && !m_checkedTreeData.treeName.isEmpty())
            {
                GraphicsItemsHandle::getInstance()->sendShowTreeProperty(m_checkedItemData);
            }
        }
    }

    return QGraphicsScene::mousePressEvent(mouseEvent);
}


/**
*@brief       场景鼠标移动事件
*@parameter   QGraphicsSceneMouseEvent
*@return
**/
void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_regionSelItem && m_regionSelItem->step() == DrawSelToolStep::DRAWSELTOOLSTEP_1
            && QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        QPointF  point = mouseEvent->scenePos();
        m_regionSelItem->setEndPointX(point.x());
        m_regionSelItem->setEndPointY(point.y());
        m_regionSelItem->update();
    }

    if (m_mode == InsertLine && m_line != Q_NULLPTR)
    {
        QLineF newLine(m_line->line().p1(), mouseEvent->scenePos());
        m_line->setLine(newLine);
    }
    else if (m_mode == InsertItem)
    {
        return QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

/**
*@brief      场景鼠标释放事件
*@parameter  QGraphicsSceneMouseEvent
*@return
**/
void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // 框选图元释放
    if(m_regionSelItem)
    {
        m_regionSelItem->setStep(DrawSelToolStep::DRAWSELTOOLSTEP_2);
        QPointF  point = mouseEvent->scenePos();
        m_regionSelItem->setEndPointX(point.x());
        m_regionSelItem->setEndPointY(point.y());
        m_regionSelItem->update();
        m_regionSelItem->setStep(DrawSelToolStep::DRAWSELTOOLSTEP_FINISH);
        m_regionSelItem->selectItems(this);
        m_regionSelItem->setStep(DrawSelToolStep::DRAWSELTOOLSTEP_INVALID);

        this->removeItem(m_regionSelItem);
        delete m_regionSelItem;
        m_regionSelItem = Q_NULLPTR;
    }

    // 移动图元
    //    if (m_selectedPointCount != 0 && m_mode == InsertItem && m_curItem) {
    //       if (m_curItem->pos() != m_oldMovePos ) {
    if (m_selectedPointCount != 0 && m_mode == InsertItem) {
        if (m_curItem&&m_curItem->pos() != m_oldMovePos ) {
            QPointF movedPos = m_curItem->pos() - m_oldMovePos;
            QList<QGraphicsItem *> allItems;
            QList<QGraphicsItem *> allSelItems = selectedItems().isEmpty() ? (QList<QGraphicsItem *>() << m_curItem) : selectedItems();
            for (auto item : allSelItems) {
                allItems.append(item);
                //                for (auto polyline : qgraphicsitem_cast<BaseItem*>(item)->getAllPolyLine()) {
                //                    allItems.append(polyline);
                //                }
            }
            moveItemsWithUndo(allItems, movedPos);
            qDebug() << "selected item count : " << m_selectedPointCount << "old point: " << m_oldMovePos
                     << "move: " << m_curItem->pos() - m_oldMovePos;
            m_selectedPointCount = 0;
        }
    }

    // 连接折线
    if (m_line != Q_NULLPTR && m_mode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(m_line->line().p1());
        if (startItems.count() && startItems.first() == m_line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(m_line->line().p2());
        if (endItems.count() && endItems.first() == m_line)
            endItems.removeFirst();

        removeItem(m_line);
        delete m_line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
                startItems.first() != endItems.first()) {
            BaseItem *startItem = qgraphicsitem_cast<BaseItem *>(startItems.first());
            BaseItem *endItem = qgraphicsitem_cast<BaseItem *>(endItems.first());

            // 是否节点之间的约束关系
            bool flag = constraintNode(startItem,endItem);
            if(flag)
            {
                PolyLineItem *polyLine = new PolyLineItem(startItem, endItem);
                polyLine->setColor(Qt::black);

                //                startItem->addPolyLine(polyLine);
                //                endItem->addPolyLine(polyLine);

                //                //设置节点的父子关系
                //                endItem->getTreeData()->setParentItem(startItem->getTreeData());
                //                startItem->getTreeData()->appendChild(endItem->getTreeData());

                //添加折线
                // addItem(polyLine);
                addItemsWithUndo(QList<QGraphicsObject*>() << polyLine);
                //                //更新折线位置
                //                polyLine->updatePosition();
            }
            else
            {
                QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("不符合节点之间约束关系,请重新连线"));
            }

        }
    }
    m_line = Q_NULLPTR;

    //自动设置场景的区域（只适用低性能场景）
    setSceneRect(this->itemsBoundingRect());

    return QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

/**
*@brief      拖拽进入场景事件
*@parameter  QGraphicsSceneDragDropEvent
*@return
**/
void GraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(m_mode == NotEdit || m_mode == InsertLine)
    {
        event->setDropAction(Qt::IgnoreAction);
    }
    else
    {
        event->acceptProposedAction();
    }
}

/**
*@brief      拖拽在场景中移动事件
*@parameter  QGraphicsSceneDragDropEvent
*@return
**/
void GraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(m_mode == NotEdit || m_mode == InsertLine)
    {
        event->setDropAction(Qt::IgnoreAction);
    }
    else
    {

    }
}

/**
*@brief     拖拽在场景中释放事件
*@parameter QGraphicsSceneDragDropEvent
*@return
**/
void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    //!插入节点
    //!
    static bool root = false;
    if(m_mode == InsertItem)
    {
        BaseItem *item = Q_NULLPTR;
        //!----------------约束性检查（第一个节点必须为描述节点）--------------------
        QList<QGraphicsItem *> items = this->items();
        if(items.count() == 0)
        {
            QString t_str = event->mimeData()->text();

            if(t_str == DescribeNodeStr)
            {
                //event->acceptProposedAction();
                root = true;
            }
            else
            {
                QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("根节点必须为描述节点!"));
                return;
            }
        }
        //!
        //! \brief t_str
        //!
        QString t_str = event->mimeData()->text();
        //节点数据
        PublicData::PelItemData nodeData;
        if(DescribeNodeStr == t_str)
        {
            item = ItemsFactory::instance()->creatItem(ItemAndPropertyType::BranchNode);
            nodeData.itemType = ItemAndPropertyType::BranchNode;
            if(root)
            {
                m_rootItem =item;
                root = false;
            }
        }
        else if(ParamNodeStr == t_str)
        {
            item = ItemsFactory::instance()->creatItem(ItemAndPropertyType::LeafNode);
            nodeData.itemType = ItemAndPropertyType::LeafNode;
        }
        else if(TestNodeStr == t_str)
        {
            //!约束
            if(m_checkedTreeData.treeType == PublicData::TreeType::TaskAnalyTree)
            {
                QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("任务分析树中不允许挂载测试节点"));
                return;
            }
            item = ItemsFactory::instance()->creatItem(ItemAndPropertyType::TestNode);
            nodeData.itemType = ItemAndPropertyType::TestNode;
        }
        else if(PluginNodeStr == t_str)
        {
            //!约束
            if(m_checkedTreeData.treeType == PublicData::TreeType::SysDiagnoseTree)
            {
                QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("系统诊断故障树中不允许挂载插件节点"));
                return;
            }
            if(m_checkedTreeData.treeType == PublicData::TreeType::TaskAnalyTree)
            {
                QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("任务分析故障树中不允许挂载插件节点"));
                return;
            }
            item = ItemsFactory::instance()->creatItem(ItemAndPropertyType::PlugNode);
            nodeData.itemType = ItemAndPropertyType::PlugNode;
        }
        else if(ArithmeticStr == t_str)
        {
            item = ItemsFactory::instance()->creatItem(ItemAndPropertyType::Arithmetic);
            nodeData.itemType = ItemAndPropertyType::Arithmetic;
        }
        else if(FaultTreeDatas == t_str)
        {
            //拖拽故障树
            QString treeid = QString::fromStdString(event->mimeData()->data("text/csv").toStdString());
            //从服务端请求数据
            CommonData::isTreeEdited = true;
            GraphicsItemsHandle::getInstance()->sendLoadDragShowNodesReq(treeid);

            m_dragPoint = event->scenePos();
            return;
        }
        else
        {
            event->acceptProposedAction();
            return;
        }

        if(Q_NULLPTR == item)
        {
            event->acceptProposedAction();
            return;
        }
        nodeData.treeId = m_checkedTreeData.id;
        nodeData.pos = event->scenePos();
        //连接删除槽
        connect(item, &BaseItem::signalDelNode, this, &GraphicsScene::onDeleteTreeNode, Qt::UniqueConnection);
        //连接复制槽
        connect(item, &BaseItem::signalCopyNode, this, &GraphicsScene::onCopyTreeNode, Qt::UniqueConnection);

        //这里不向服务器请求节点id，使用本地的UUID，设置到节点的ID中去
        QString strId = QUuid::createUuid().toString();
        nodeData.id = strId;

        //获取在场景坐标系中的坐标
        QPointF point = event->scenePos();
        //设置节点位置
        item->setPos(point);
        //addItem(item);
        addItemsWithUndo(QList<QGraphicsObject*>() << item);
        //设置节点的数据域
        item->setTreeData(nodeData);
        //设置图元初始化名称
        item->setProperty("name","itemName");
        //让属性模块添加属性
        CommonData::isTreeEdited = true;
        GraphicsItemsHandle::getInstance()->sendAddItemProperty(strId, nodeData.itemType, m_checkedTreeData.treeType);

        event->acceptProposedAction();
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

/**
 * @brief 绘制场景的背景
 * @param painter
 * @param rect
 */
void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QColor c(Qt::darkCyan);
    QPen p(c);
    p.setStyle(Qt::DashLine);
    p.setWidthF(0.2);
    painter->setPen(p);

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing,false);
    painter->fillRect(rect,Qt::white);
    for (int x=rect.left() ;x <rect.right()  ;x+=20) {
        painter->drawLine(x,rect.top(),x,rect.bottom());

    }
    for (int y=rect.top();y<rect.bottom() ;y+=20)
    {
        painter->drawLine(rect.left(),y,rect.right(),y);
    }
    p.setStyle(Qt::SolidLine);
    p.setColor(Qt::black);
    painter->drawLine(rect.right(),rect.top(),rect.right(),rect.bottom());
    painter->drawLine(rect.left(),rect.bottom(),rect.right(),rect.bottom());

    //draw shadow
    QColor c1(Qt::black);
    painter->fillRect(QRect(rect.right()+1,rect.top()+2,2,rect.height()),c1.dark(200));
    painter->fillRect(QRect(rect.left()+2,rect.bottom()+2,rect.width(),2),c1.dark(200));

    painter->restore();

    return QGraphicsScene::drawBackground(painter,rect);
}

void GraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem *selectedItem = itemAt(event->scenePos(),QTransform());

    if (!selectedItem)
    {
        QMenu menu;
        QAction *pasteAction = new QAction(QStringLiteral("粘贴(Ctrl+V)"), this);

        connect(pasteAction, &QAction::triggered, this, &GraphicsScene::onPasteTreeNode);
        menu.addAction(pasteAction);
        menu.exec(QCursor::pos());
    }
    else
    {
        QGraphicsScene::contextMenuEvent(event);
    }
}

/**
*@brief     KnowledgeItem *：父节点  QList<TreeData *>:孩子节点的数据
*@parameter
*@return
**/
void GraphicsScene::reloadChildrenTreeData(BaseItem *itemParent, QList<TreeNode *> listTreeData)
{
    TreeNode *treeData;
    PublicData::PelItemData itemData;
    foreach (treeData, listTreeData) {
        //子节点
        itemData = treeData->getItemData();
        BaseItem *item = ItemsFactory::instance()->creatItem(itemData.itemType);

        if(item == NULL)
            continue;
        //设置节点位置
        item->setPos(itemData.pos);

        //添加图元到场景中
        addItem(item);
        //连接删除槽
        connect(item, &BaseItem::signalDelNode, this, &GraphicsScene::onDeleteTreeNode, Qt::UniqueConnection);
        //连接复制槽
        connect(item, &BaseItem::signalCopyNode, this, &GraphicsScene::onCopyTreeNode, Qt::UniqueConnection);

        //设置节点的数据域
        item->setTreeData(itemData);

        //添加连线(需要开始节点和结束节点)
        PolyLineItem *polyLine = new PolyLineItem(itemParent, item);
        polyLine->setColor(Qt::black);

        itemParent->addPolyLine(polyLine);
        item->addPolyLine(polyLine);

        //添加到场景中
        addItem(polyLine);
        //更新位置信息
        polyLine->updatePosition();
        //设置节点之间的关系
        item->getTreeData()->setParentItem(itemParent->getTreeData());
        itemParent->getTreeData()->appendChild(item->getTreeData());

        //回调
        reloadChildrenTreeData(item,treeData->getChildren());
    }
}

void GraphicsScene::loadDragChildrenTreeData(BaseItem *itemParent, QList<TreeNode *> listTreeData, QPointF adjustPointF, QList<QGraphicsObject*>& items)
{
    TreeNode *treeData;
    PublicData::PelItemData itemData;
    foreach (treeData, listTreeData) {
        //子节点
        itemData = treeData->getItemData();
        BaseItem *item = ItemsFactory::instance()->creatItem(itemData.itemType);

        if(item == NULL)
            continue;

        //更新所属树的id
        itemData.treeId = m_checkedTreeData.id;
        //调整树节点的位置

        itemData.pos += adjustPointF;


        //设置节点位置
        item->setPos(itemData.pos);

        //添加图元到场景中
        // addItem(item);
        items.append(item);
        //连接删除槽
        connect(item, &BaseItem::signalDelNode, this, &GraphicsScene::onDeleteTreeNode, Qt::UniqueConnection);
        //连接复制槽
        connect(item, &BaseItem::signalCopyNode, this, &GraphicsScene::onCopyTreeNode, Qt::UniqueConnection);

        //设置节点的数据域
        item->setTreeData(itemData);

        //添加连线(需要开始节点和结束节点)
        PolyLineItem *polyLine = new PolyLineItem(itemParent, item);
        polyLine->setColor(Qt::black);

        itemParent->addPolyLine(polyLine);
        item->addPolyLine(polyLine);

        //添加到场景中
        // addItem(polyLine);
        items.append(polyLine);

        //更新位置信息
        polyLine->updatePosition();
        //设置节点之间的关系
        item->getTreeData()->setParentItem(itemParent->getTreeData());
        QList<TreeNode*> t_treenode = itemParent->getTreeData()->getChildren();
        itemParent->getTreeData()->appendChild(item->getTreeData());

        //回调
        loadDragChildrenTreeData(item,treeData->getChildren(),adjustPointF, items);
    }

}

void GraphicsScene::handleEvent()
{
    //发送查找所有函数请求
    // GraphicsItemsHandle::getInstance()->sendFindAllFunctionReq();

    connect(GraphicsItemsHandle::getInstance(),SIGNAL(signalChangeSceneMode(int)),this,SLOT(setMode(int)));

    //获得当前选中的故障树属性
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalPitchFaultTree,this,&GraphicsScene::onSetFaultTreeData);
    //重新加载故障树节点
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalLoadTreeNodesRes,this,&GraphicsScene::onReloadEditTree);
    //清除编辑区内容
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalClearEditView,this,&GraphicsScene::onClearEditView);
    //设置节点的属性到节点的数据域中
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalSetNodesProperty,this,&GraphicsScene::onSetNodesProperty);
    //绘制图元名称
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalDrawItemName,this,&GraphicsScene::onDrawItemName);

    //更新故障树属性
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalUpdateTreeProperty,this,&GraphicsScene::updateFaultTreeData);
    //展示拖拽树的节点
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalDragShowNodesRes,this,&GraphicsScene::onLoadDragTreeNodes);

    //保存属性（需要验证）
    connect(GraphicsItemsHandle::getInstance(),&GraphicsItemsHandle::signalSaveProperty,this,&GraphicsScene::onSaveEditTree);

    // 图元对齐
    qRegisterMetaType<QMap<QGraphicsItem*, QPair<QPointF, QPointF>>>("QMap<QGraphicsItem*, QPair<QPointF, QPointF>>");
    connect(ItemsOperationToolInstance, &ItemsOperationTool::signalItemAlign, this, [this](QMap<QGraphicsItem*, QPair<QPointF, QPointF>> itemsPos) {
        QUndoCommand *moveCommand = new MoveItemsCommand(itemsPos, this);
        m_undostak->push(moveCommand);
    });
    connect(ItemsOperationToolInstance, &ItemsOperationTool::signalItemAlignSpacing, this, [this](QMap<QGraphicsItem*, QPair<QPointF, QPointF>> itemsPos) {
        QUndoCommand *moveCommand = new MoveItemsCommand(itemsPos, this);
        m_undostak->push(moveCommand);
    });
}

bool GraphicsScene::constraintNode(BaseItem *startItem, BaseItem *endItem)
{
    if(startItem == Q_NULLPTR || endItem == Q_NULLPTR)
        return false;

    ItemAndPropertyType startItemType = startItem->getItemType();
    int parentChildCount = startItem->getTreeData()->getChildren().count();
    ItemAndPropertyType endItemType = endItem->getItemType();

    if(startItemType == ItemAndPropertyType::BranchNode)
    {
        if(endItemType == ItemAndPropertyType::Arithmetic && parentChildCount == 0 && endItem->getTreeData()->getParentItem() == Q_NULLPTR)
        {
            return true;
        }
    }
    else if(endItemType == ItemAndPropertyType::LeafNode)
    {
        if(startItemType == ItemAndPropertyType::Arithmetic && endItem->getTreeData()->getParentItem() == Q_NULLPTR)
        {
            return true;
        }
    }
    else if(endItemType == ItemAndPropertyType::TestNode)
    {
        if(startItemType == ItemAndPropertyType::Arithmetic && endItem->getTreeData()->getParentItem() == Q_NULLPTR)
        {
            return true;
        }
    }
    else if(endItemType == ItemAndPropertyType::PlugNode)
    {
        if(startItemType == ItemAndPropertyType::Arithmetic && endItem->getTreeData()->getParentItem() == Q_NULLPTR)
        {
            return true;
        }
    }
    else if (endItemType == ItemAndPropertyType::BaseKnowledgeNode)   // 基础知识节点
    {
        if (startItemType == ItemAndPropertyType::Arithmetic && endItem->getTreeData()->getParentItem() == Q_NULLPTR)
        {
            return true;
        }
    }
    else if(startItemType == ItemAndPropertyType::Arithmetic)
    {
        if(endItemType == ItemAndPropertyType::BranchNode && endItem->getTreeData()->getParentItem() == Q_NULLPTR)
        {
            for (auto child : endItem->getTreeData()->getChildren()) {
                if (child == startItem->getTreeData()) {
                    return false;
                }
            }
            return true;
        }
    }

    return false;
}

void GraphicsScene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_D && (event->modifiers() == Qt::ControlModifier))
    {
        // 删除
        onDeleteTreeNode();
    }
    if(event->key() == Qt::Key_C && (event->modifiers() == Qt::ControlModifier))
    {
        // 复制
        onCopyTreeNode();
    }
    if(event->key() == Qt::Key_V && (event->modifiers() == Qt::ControlModifier))
    {
        // 粘贴
        onPasteTreeNode();
    }
    if(event->key() == Qt::Key_S && (event->modifiers() == Qt::AltModifier))
    {
        // 保存
        onSaveEditTree();
    }
    //add by led
    if(m_curItem)
    {
        if(event->key() == Qt::Key_Up)
        {
            int y=m_curItem->pos().y();
            int x=m_curItem->pos().x();
            y=y-1;
            m_curItem->setPos(x,y);
            qDebug()<<".................up............";
        }
        else if(event->key() == Qt::Key_Down)
        {
            int y=m_curItem->pos().y();
            int x=m_curItem->pos().x();
            y=y+1;
            m_curItem->setPos(x,y);
            qDebug()<<".................down............";
        }
        else if(event->key() == Qt::Key_Left)
        {
            int y=m_curItem->pos().y();
            int x=m_curItem->pos().x();
            x=x-1;
            m_curItem->setPos(x,y);
            qDebug()<<".................left............";
        }
        else if(event->key() == Qt::Key_Right)
        {
            int y=m_curItem->pos().y();
            int x=m_curItem->pos().x();
            x=x+1;
            m_curItem->setPos(x,y);
            qDebug()<<".................right............";
        }
    }
}

void GraphicsScene::addItemsWithUndo(const QList<QGraphicsObject *> items)
{
    QUndoCommand *addCommand = new AddItemsCommand(items, this);
    m_undostak->push(addCommand);
    qDebug() << "current undostack size:" << m_undostak->count();
}

void GraphicsScene::removeItemsWithUndo(const QList<QGraphicsItem *> items)
{
    QUndoCommand *deleteCommand = new DeleteItemsCommand(items, &m_deleteNodeId, this);
    m_undostak->push(deleteCommand);
    qDebug() << "current undostack size:" << m_undostak->count();
}

void GraphicsScene::moveItemsWithUndo(const QList<QGraphicsItem *>& items, const QPointF &movedPos)
{
    QList<QGraphicsObject *> objects;
    for (auto item : items) {
        objects.append(qgraphicsitem_cast<QGraphicsObject *>(item));
    }
    QUndoCommand *moveCommand = new MoveItemsCommand(objects, movedPos, this);
    m_undostak->push(moveCommand);
    qDebug() << "current undostack size:" << m_undostak->count();
}
