/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         封装树结构(用于保存)
*@author        luox
*@date          2018-05-07
**/
#include "treenode.h"
#include <QMetaType>
#include <QDebug>

TreeNode::TreeNode(TreeNode *parentItem)
{
    m_parentItem = parentItem;
    qRegisterMetaType<TreeNode>("TreeNode");
    m_childItems.clear();
}

TreeNode::TreeNode(const PublicData::PelItemData &data)
{
    m_itemData = data;
    m_parentItem = Q_NULLPTR;
    qRegisterMetaType<TreeNode>("TreeNode");
    m_childItems.clear();
}

TreeNode::~TreeNode()
{
    //qDeleteAll(m_childItems);
    //    if (!this->m_childItems.isEmpty())
    //    {
    //        const int count = this->m_childItems.count();
    //        for (int i = count -1; i >= 0; i--)
    //        {
    //            TreeData *childNode = this->m_childItems.at(i);

    //            if ((Q_NULLPTR != childNode))
    //            {
    //                delete childNode;
    //                childNode = Q_NULLPTR;
    //            }
    //        }
    //    }
}



/**
*@brief     添加孩子图元
*@parameter TreeData* :孩子图元
*@return
**/
void TreeNode::appendChild(TreeNode *child)
{
    m_childItems.append(child);
}

/**
*@brief     移除孩子图元
*@parameter TreeData *:孩子图元
*@return
**/
void TreeNode::removeChild(TreeNode *child)
{
    if(m_childItems.contains(child))
    {
        int index = m_childItems.indexOf(child);
        m_childItems.removeAt(index);
    }
}

bool TreeNode::isChildNode(TreeNode *child)
{
    if(m_childItems.contains(child))
    {
        return true;
    }

    else
    {
        return false;
    }
}

/**
*@brief
*@parameter
*@return
**/
void TreeNode::setParentItem(TreeNode *parentItem)
{
    m_parentItem = parentItem;
}

/**
*@brief
*@parameter
*@return
**/
TreeNode *TreeNode::getParentItem()
{
    return m_parentItem;
}

void TreeNode::setItemData(const PublicData::PelItemData &data)
{
    m_itemData = data;
}

void TreeNode::setNodePos(const QPointF &point)
{
    m_itemData.pos.setX(point.x());
    m_itemData.pos.setY(point.y());
}

void TreeNode::setNodeId(const QString &id)
{
    m_itemData.id = id;
}

QString TreeNode::getNodeId()
{
    return m_itemData.id;
}

QString TreeNode::getNodeTreeId()
{
    return m_itemData.treeId;
}

void TreeNode::setNodeType(ItemAndPropertyType type)
{
    m_itemData.itemType = type;
}

ItemAndPropertyType TreeNode::getNodeType()
{
    return m_itemData.itemType;
}

void TreeNode::setNodeName(const QString &name)
{
    m_itemData.name = name;
}

QString TreeNode::getNodeName()
{
    return m_itemData.name;
}

void TreeNode::setNodeProperty(const QString &strProperty)
{
    m_itemData.nodeProperty = strProperty;
}

QString TreeNode::getNodeProperty()

{
    return  m_itemData.nodeProperty;

}

PublicData::PelItemData TreeNode::getItemData()
{
    return m_itemData;
}

