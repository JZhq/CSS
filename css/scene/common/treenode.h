#ifndef TREENODE_H
#define TREENODE_H

#include <QList>
#include "publicdata.h"
#include "scene_global.h"
#include <QObject>

class  TreeNode
{

public:
    explicit  TreeNode(TreeNode *parentItem = Q_NULLPTR);
    explicit  TreeNode(const PublicData::PelItemData& data);
    ~TreeNode();

    /**
     * @brief appendChild 添加子节点
     * @param child
     */
    void appendChild(TreeNode *child);
    /**
     * @brief removeChild 移除子节点
     * @param child
     */
    void removeChild(TreeNode *child);
    /**
     * @brief removeChild 移除子节点
     * @param child
     */
    bool isChildNode(TreeNode *child);
    /**
     * @brief getChildren 返回孩子节点
     * @return
     */
    QList<TreeNode*> getChildren() { return m_childItems;}

    /**
     * @brief setParentItem 设置父节点
     * @param parentItem
     */
    void setParentItem(TreeNode *parentItem);
    /**
     * @brief getParentItem 获取父节点
     * @return
     */
    TreeNode* getParentItem();
    /**
     * @brief 获取节点数据
     * @return
     */
    PublicData::PelItemData getItemData();
    /**
     * @brief 设置节点数据
     * @param data
     */
    void setItemData(const PublicData::PelItemData& data);
    /**
     * @brief 设置节点的位置
     * @param point
     */
    void setNodePos(const QPointF& point);
    /**
     * @brief 设置节点的id
     * @param id
     */
    void setNodeId(const QString& id);
    /**
     * @brief 获取节点的id
     * @return QString
     */
    QString getNodeId();

    QString getNodeTreeId();
    /**
     * @brief 设置节点类型
     */
    void setNodeType(ItemAndPropertyType type);
    /**
     * @brief 获取节点类型
     */
    ItemAndPropertyType getNodeType();
    /**
     * @brief 设置节点名称
     * @param name
     */
    void setNodeName(const QString& name);
    /**
     * @brief 获取节点名称
     * @return
     */
    QString getNodeName();
    /**
     * @brief 设置节点的属性
     */
    void setNodeProperty(const QString& strProperty);
    /**
     * @brief 得到节点的属性
     */
    QString getNodeProperty();
private:
    QList<TreeNode*> m_childItems;
    TreeNode *m_parentItem = nullptr;
    //节点数据
    PublicData::PelItemData m_itemData;
};

Q_DECLARE_METATYPE(::TreeNode)

#endif // TREENODE_H
