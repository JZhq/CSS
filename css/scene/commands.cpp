#include "commands.h"
#include "../items/polylineitem.h"

#include <QStringList>

/*-----------------------------------------------------------------------------|
 |                         AddItemsCommand implementation                      |
 |----------------------------------------------------------------------------*/
AddItemsCommand::AddItemsCommand(const QList<QGraphicsObject *> items, GraphicsScene *scene, QUndoCommand* parent) :
    QUndoCommand(parent)
{
    m_scene = scene;
    for (auto item : items)
        m_items.append(item);
}

void AddItemsCommand::undo()
{
    for (auto item : m_items) {
        if (item != Q_NULLPTR) {
            if (qgraphicsitem_cast<PolyLineItem*>(item) != Q_NULLPTR) {
                PolyLineItem *polyLine =  qgraphicsitem_cast<PolyLineItem*>(item);
                auto startItem = polyLine->getStartItem();
                auto endItem   = polyLine->getEndItem();
                startItem->removePolyLine(polyLine);
                endItem->removePolyLine(polyLine);

                // 移除节点的父子关系
                endItem->getTreeData()->setParentItem(Q_NULLPTR);
                startItem->getTreeData()->removeChild(endItem->getTreeData());
            }
            m_scene->removeItem(item);
        }
    }
    m_scene->update();
}

void AddItemsCommand::redo()
{
    for (auto item : m_items) {
        if (item != Q_NULLPTR) {
            if (qgraphicsitem_cast<PolyLineItem*>(item) != Q_NULLPTR) {
                PolyLineItem *polyLine =  qgraphicsitem_cast<PolyLineItem*>(item);
                auto startItem = polyLine->getStartItem();
                auto endItem   = polyLine->getEndItem();
                startItem->addPolyLine(polyLine);
                endItem->addPolyLine(polyLine);

                // 设置节点的父子关系
                endItem->getTreeData()->setParentItem(startItem->getTreeData());

                if(!(startItem->getTreeData()->isChildNode(endItem->getTreeData())))
                {
                      startItem->getTreeData()->appendChild(endItem->getTreeData());
                }
                // 更新折线位置
                polyLine->updatePosition();
            }
            m_scene->addItem(item);
        }
    }
    m_scene->update();
}

/*-----------------------------------------------------------------------------|
 |                         DeleteItemsCommand implementation                   |
 |----------------------------------------------------------------------------*/
DeleteItemsCommand::DeleteItemsCommand(const QList<QGraphicsItem *> items,
                                       QStringList *deletedNodesId,
                                       GraphicsScene *scene, QUndoCommand* parent) :
    m_deletedNodesId(deletedNodesId),
    QUndoCommand(parent)
{
    m_scene = scene;
    for (auto item : items)
        m_items.append(item);
}

void DeleteItemsCommand::undo()
{
    QList<QGraphicsObject*> items;
    for (auto item : m_items) {
        BaseItem *node = dynamic_cast<BaseItem *>(item);
        if (node != Q_NULLPTR) {
            // 恢复连线
            items.append(node->getAllPolyLine());

            // 恢复节点之间的关系
            if(m_itemParents[node])
            {
                m_itemParents[node]->appendChild(node->getTreeData());
                node->getTreeData()->setParentItem(m_itemParents[node]);
            }
            for (auto child : node->getTreeData()->getChildren()) {
                child->setParentItem(node->getTreeData());
            }

            // 将界面中已经恢复的节点id保存到界面中，如果点击了保存，则推送到服务端
            m_deletedNodesId->removeOne(node->getTreeData()->getNodeId());
            items.append(static_cast<QGraphicsObject*>(item));
        }
    }

    for (auto item : items) {
        if (item != Q_NULLPTR) {
            m_scene->addItem(static_cast<QGraphicsItem*>(item));
        }
    }

    m_scene->update();
}

void DeleteItemsCommand::redo()
{
    QList<QGraphicsObject*> items;
    for (auto item : m_items) {
        BaseItem *node = dynamic_cast<BaseItem *>(item);
        if (node != Q_NULLPTR) {
            // 删除连线
            items.append(node->getAllPolyLine());

            // 移除节点之间的关系
            if(node->getTreeData()->getParentItem() != Q_NULLPTR)
            {
                m_itemParents[node] = node->getTreeData()->getParentItem();
                node->getTreeData()->getParentItem()->removeChild(node->getTreeData());
                node->getTreeData()->setParentItem(Q_NULLPTR);
            }
            for (auto child : node->getTreeData()->getChildren()) {
                child->setParentItem(Q_NULLPTR);
            }

            // 将界面中已经删除的节点id保存到界面中，如果点击了保存，则推送到服务端
            m_deletedNodesId->append(node->getTreeData()->getNodeId());
            items.append(static_cast<QGraphicsObject*>(item));
        }
    }

    for (auto item : items) {
        if (item != Q_NULLPTR) {
            m_scene->removeItem(static_cast<QGraphicsItem*>(item));
        }
    }
    m_scene->update();
}

/*-----------------------------------------------------------------------------|
 |                         MoveItemsCommand implementation                     |
 |----------------------------------------------------------------------------*/
MoveItemsCommand::MoveItemsCommand(const QList<QGraphicsObject *> items, const QPointF& movedPos, GraphicsScene *scene, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_scene = scene;
    for (auto item : items) {
        m_items.append(item);
        m_oldPoints[item] = item->pos() - movedPos;
        m_newPoints[item] = item->pos();
    }
}

MoveItemsCommand::MoveItemsCommand(const QMap<QGraphicsItem *, QPair<QPointF, QPointF> > &itemsPos, GraphicsScene *scene, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_scene = scene;
    for (auto itor = itemsPos.constBegin(); itor != itemsPos.constEnd(); ++itor) {
        auto item = qgraphicsitem_cast<QGraphicsObject *>(itor.key());
        if (item != Q_NULLPTR) {
            m_items.append(item);
            m_oldPoints[item] = itor.value().second;
            m_newPoints[item] = itor.value().first;
        }

    }
}

void MoveItemsCommand::undo()
{
    for (auto item : m_items) {
        if (item != Q_NULLPTR && m_oldPoints.contains(item)) {
            if (!qgraphicsitem_cast<PolyLineItem*>(item)) {
                item->setPos(m_oldPoints[item]);
                for (auto polyLineItem : qgraphicsitem_cast<BaseItem*>(item)->getAllPolyLine()) {
                    qgraphicsitem_cast<PolyLineItem*>(polyLineItem)->updatePosition();
                }
            }
            qgraphicsitem_cast<BaseItem*>(item)->getTreeData()->setNodePos(m_oldPoints[item]);
            item->update();
        }
    }
    m_scene->update();
}

void MoveItemsCommand::redo()
{
    for (auto item : m_items) {
        if (item != Q_NULLPTR && m_newPoints.contains(item)) {
            if (!qgraphicsitem_cast<PolyLineItem*>(item)) {
                item->setPos(m_newPoints[item]);
                for (auto polyLineItem : qgraphicsitem_cast<BaseItem*>(item)->getAllPolyLine()) {
                    qgraphicsitem_cast<PolyLineItem*>(polyLineItem)->updatePosition();
                }
            }
            qgraphicsitem_cast<BaseItem*>(item)->getTreeData()->setNodePos(m_newPoints[item]);
            item->update();
        }
    }
    m_scene->update();
}
