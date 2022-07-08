/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         封装树结构(用于保存)
*@author        luox
*@date          2018-05-07
**/
#ifndef TREEDATA_H
#define TREEDATA_H

#include <QList>
#include "publicdata.h"
#include <QObject>

class TreeData
{

public:
    explicit  TreeData(TreeData *parentItem = Q_NULLPTR);
    ~TreeData();
    /**
     * @brief appendChild 添加子节点
     * @param child
     */
    void appendChild(TreeData *child);
    /**
     * @brief removeChild 移除子节点
     * @param child
     */
    void removeChild(TreeData *child);
    /**
     * @brief getChildren 返回孩子节点
     * @return
     */
    QList<TreeData*> getChildren() { return m_childItems;}

    /**
     * @brief setParentItem 设置父节点
     * @param parentItem
     */
    void setParentItem(TreeData *parentItem);
    /**
     * @brief getParentItem 获取父节点
     * @return
     */
    TreeData* getParentItem();
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
private:
    QList<TreeData*> m_childItems;
    TreeData *m_parentItem;
    PublicData::PelItemData m_itemData;//节点数据
};

Q_DECLARE_METATYPE(::TreeData)

#endif // TREEDATA_H
