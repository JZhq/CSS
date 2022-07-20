/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         管理图元类
*@author        luox
*@date          2018-05-07
**/
#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H
#include <QGraphicsScene>
#include "common/configureglobal.h"
#include "common/publicdata.h"
#include "common/scene_global.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QGraphicsSceneDragDropEvent;
class QUndoStack;
class QAction;
class QGraphicsObject;
QT_END_NAMESPACE

class TreeNode;
class BaseItem;
class RegionSelectItem;

class SCENE_SHARED_EXPORT GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, NotEdit };
    enum EditMode{ FaultEdit,  BaseKnowEdit };
    enum ItemEdit{ AddItem, DeleteItem, UpdateItem};
    explicit GraphicsScene(QObject *parent = Q_NULLPTR);

    int getFaultTreeType() const
    {
        return m_checkedTreeData.treeType;
    }

    QUndoStack* getUndoStack() const
    {
        return m_undostak;
    }
signals:
    void itemChanged(const QVariantHash&, int);
public slots:
    /**
     * @brief setMode
     * @param mode
     */
    void setMode(int mode);
    /**
     * @brief 保存编辑树
     */
    void onSaveEditTree();
    /**
     * @brief 设置当前选中故障树属性
     * @param treeData
     */
    void onSetFaultTreeData(const QVariantHash &itemData);
    /**
     * @brief 更新故障树的属性
     * @param treeData
     */
    void updateFaultTreeData(const QVariantHash &treeData);
    /**
     * @brief 重新加载编辑树
     * @param rootTreeData
     */
    void onReloadEditTree(TreeNode *rootTreeData);
    /**
     * @brief onLoadDragTreeNodes
     * @param rootTreeData
     */
    void onLoadDragTreeNodes(TreeNode *rootTreeData);
    /**
     * @brief 删除树的节点
     */
    void onDeleteTreeNode();
    /**
     * @brief 复制树的节点
     */
    void onCopyTreeNode();
    /**
     * @brief 粘贴树的节点
     */
    void onPasteTreeNode();
    /**
     * @brief 清除编辑区内容
     */
    void onClearEditView();
    /**
     * @brief 设置节点的属性
     */
    void onSetNodesProperty(const QVariantHash &hash);
    /**
     * @brief 绘制图元名称
     * @param id
     * @param name
     */
    void onDrawItemName(const QString &id,const QString &name);
    /**
     * @brief 绘制变量名
     * @param id
     * @param varName
     */
    void onDrawItemVarName(const QString &id, const QString &varName);

    /**
     * @brief 绘制变量名
     * @param id
     * @param varName
     */
    void onDrawItemVarError(const QString &varName);

protected:
    /**
     * @brief mousePressEvent
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    /**
     * @brief mouseMoveEvent
     * @param mouseEvent
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    /**
     * @brief mouseReleaseEvent
     * @param mouseEvent
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    /**
     * @brief dragEnterEvent drag进入事件
     * @param event
     */
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event) Q_DECL_OVERRIDE;
    /**
     * @brief dragMoveEvent drag在场景中移动事件
     * @param event
     */
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    /**
     * @brief dropEvent  drag放下事件
     * @param event
     */
    void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    /**
     * @brief drawBackground 绘制背景
     * @param painter
     * @param rect
     */
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    /**
     * @brief 右键菜单
     * @param event
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;

private:
    /**
     * @brief reloadChildrenTreeData
     * @param itemParent
     * @param listTreeData
     */
    void reloadChildrenTreeData(BaseItem *itemParent,QList<TreeNode*> listTreeData);
    /**
     * @brief 加载拖拽出来的子节点数据
     * @param itemParent
     * @param listTreeData
     * @param adjustPointF
     */
    void loadDragChildrenTreeData(BaseItem *itemParent, QList<TreeNode*> listTreeData, QPointF adjustPointF,
                                  QList<QGraphicsObject*>& items);
    /**
     * @brief 连接信号与槽
     */
    void handleEvent();
    /**
     * @brief 约束节点之间的关系
     * @param startItem
     * @param endItem
     */
    bool constraintNode(BaseItem *startItem,BaseItem *endItem);

    /**
     * @brief 快捷键
     * @param event
     * @param keyPressEvent
     */
    void keyPressEvent(QKeyEvent *event);

    //!
    //! \brief ndo/redo 添加图元命令
    //! \param items
    //!
    void addItemsWithUndo(const QList<QGraphicsObject*> items);

    //!
    //! \brief undo/redo 移除图元命令
    //! \param items
    //!
    void removeItemsWithUndo(const QList<QGraphicsItem*> items);

    //!
    //! \brief ndo/redo 移动选中图元
    //! \param items
    //! \param movedPoint
    //!
    void moveItemsWithUndo(const QList<QGraphicsItem *>& items, const QPointF& movedPos);

private:
    Mode    m_mode;
    QGraphicsLineItem *m_line = nullptr;
    PublicData::FaultTreeData m_checkedTreeData;//当前选中的故障树属性
    QVariantHash m_checkedItemData;

    QStringList m_deleteNodeId;//已被删除的节点
    QList<PublicData::PelItemData> m_copyNodeDatas;//复制的节点数据

    QPointF m_dragPoint;//拖拽释放时的鼠标点(相对于场景的坐标系)
    QPointF m_oldMovePos; // 移动时所点击的item的点 add by zs
    int m_selectedPointCount; // 移动时选中的点数量 add by zs

    //所有函数名称
    QList<QString> m_allFunctionNameList;
    //根节点
    BaseItem * m_rootItem = nullptr;
    BaseItem * m_curItem = nullptr;
    RegionSelectItem * m_regionSelItem;

    // add by zs
    QUndoStack* m_undostak = nullptr; // 实现undo、redo
    EditMode m_editMode;

};

#endif // GRAPHICSSCENE_H
