#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QGraphicsObject>
#include "graphicsscene.h"

class BaseItem;

//!
//! \brief 添加图元的命令
//!
class AddItemsCommand : public QUndoCommand
{
public:
    AddItemsCommand(const QList<QGraphicsObject*> items, GraphicsScene* scene,
                    QUndoCommand* parent = Q_NULLPTR);

    virtual void undo() override;
    virtual void redo() override;

private:
    QList<QGraphicsObject*> m_items;
    GraphicsScene* m_scene;
};

//!
//! \brief 删除图元的命令
//!
class DeleteItemsCommand : public QUndoCommand
{
public:
    DeleteItemsCommand(const QList<QGraphicsItem*> items, QStringList *deletedNodesId, GraphicsScene* scene,
                    QUndoCommand* parent = Q_NULLPTR);

    virtual void undo() override;
    virtual void redo() override;

private:
    QList<QGraphicsItem*> m_items; // 选中的图元
    QMap<BaseItem*, TreeNode*> m_itemParents; // 图元 - 父节点
    GraphicsScene* m_scene;
    QStringList* m_deletedNodesId;
};

//!
//! \brief 移动图元的命令
//!
class MoveItemsCommand : public QUndoCommand
{
public:
    MoveItemsCommand(const QList<QGraphicsObject*> items,
                     const QPointF& movedPos,
                     GraphicsScene* scene,
                    QUndoCommand* parent = Q_NULLPTR);

    MoveItemsCommand(const QMap<QGraphicsItem*, QPair<QPointF, QPointF>>& itemsPos,
                     GraphicsScene* scene,
                    QUndoCommand* parent = Q_NULLPTR);

    virtual void undo() override;
    virtual void redo() override;

private:
    QList<QGraphicsObject*> m_items;
    QMap<QGraphicsObject*, QPointF> m_oldPoints;
    QMap<QGraphicsObject*, QPointF> m_newPoints;
    GraphicsScene* m_scene;
};

#endif // COMMANDS_H
