/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         封装树结构(用于保存)
*@author        luox
*@date          2018-05-07
**/
#include "treedata.h"
#include <QMetaType>

TreeData::TreeData(TreeData *parentItem)
{
    m_parentItem = parentItem;
    qRegisterMetaType<TreeData>("TreeData");
}

TreeData::~TreeData()
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
void TreeData::appendChild(TreeData *child)
{
    m_childItems.append(child);
}

/**
*@brief     移除孩子图元
*@parameter TreeData *:孩子图元
*@return
**/
void TreeData::removeChild(TreeData *child)
{
    if(m_childItems.contains(child))
    {
        int index = m_childItems.indexOf(child);
        m_childItems.removeAt(index);
    }
}

/**
*@brief
*@parameter
*@return
**/
void TreeData::setParentItem(TreeData *parentItem)
{
    m_parentItem = parentItem;
}

/**
*@brief
*@parameter
*@return
**/
TreeData *TreeData::getParentItem()
{
    return m_parentItem;
}

void TreeData::setItemData(const PublicData::PelItemData &data)
{
    m_itemData = data;
}

PublicData::PelItemData TreeData::getItemData()
{
    return m_itemData;
}
