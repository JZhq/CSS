#ifndef GRAPHICSITEMSHANDLE_H
#define GRAPHICSITEMSHANDLE_H

#include "common/commondata.h"
#include "common/configureglobal.h"
#include "common/treenode.h"
#include <QObject>


class GraphicsItemsHandle : public QObject
{
    Q_OBJECT
public:
    static GraphicsItemsHandle* getInstance()
    {
        static GraphicsItemsHandle instance;
        return &instance;
    }

signals:
    //-----------------------发送到模块外部消息-------------------------
    /**
     * @brief signalChangeProperty 发送图元属性改变消息
     * @param data
     */
    void signalChangeProperty(const QVariantHash& data);

    /**
     * @brief signalSelectItem  图元选中的消息
     */
    void signalSelectItem(const QVariantHash&);

    /**
     * @brief signalAddItem  添加图元消息
     */
    void signalAddItemProperty(const QVariantHash&);

    /**
     * @brief 保存故障树结构（送至服务端）
     */
    void signalSaveTreeNode(const QVariantHash&);
    /**
     * @brief 删除故障树节点
     */
    void signalDeleteTreeNode(const QVariantHash&);
    /**
     * @brief 加载树的节点
     */
    void signalLoadTreeNodes(const QVariantHash&);

    //展示故障树的属性
    void signalShowTreeProperty(const QVariantHash&);

    /**
     * @brief 清除编辑区的内容
     */
    void signalClearEditView();

    /**
     * @brief 当前选中的故障树
     */
    void signalPitchFaultTree(const QVariantHash&);
    /**
     * @brief 加载故障树节点响应
     */
    void signalLoadTreeNodesRes(TreeNode*);
    /**
     * @brief 加载显示故障树节点响应
     */
    void signalLoadDisplayNodesRes(TreeNode*);
    //拖拽展示故障树节点数据
    void signalDragShowNodesRes(TreeNode*);
    /**
     * @brief 设置节点属性到节点的数据域中
     */
    void signalSetNodesProperty(const QVariantHash&);
    void signalDrawItemName(const QString &id,const QString &name);
    void signalUpdateTreeProperty(const QVariantHash &);
    void signalUpadteFaultReq(const QVariantHash&);
    void signalSaveProperty();


public slots:
    /**
     * @brief onSelectItem 选中图元
     * @param itemName
     */
    void onSelectItem(const QString& itemName);
    /**
     * @brief 清除编辑区的内容
     * @param data
     */
    void onClearEditView(const QVariantHash& data);
    /**
     * @brief 选中当前故障树
     * @param hash
     */
    void onPitchFaultTree(const QVariantHash& hash);
    /**
     * @brief 加载故障树节点回应
     * @param hash
     */
    void onLoadTreeNodes(const QVariantHash& hash);
    /**
     * @brief 设置节点的属性
     * @param hash
     */
    void onSetNodeProperty(const QVariantHash& hash);
    /**
     * @brief 加载展示的故障树节点信息
     */
    void onLoadDisplayTreeNodes(const QVariantHash& hash);
    /**
     * @brief 绘制图元的名称
     * @param hash
     */
    void onDrawItemName(const QVariantHash& hash);

    /**
     * @brief 更新故障树属性
     * @param hash
     */
    void onUpdateTreeProperty(const QVariantHash& hash);
    /**
     * @brief 保存故障树及节点的属性
     * @param hash
     */
    void onSaveProperty(const QVariantHash& hash);

public:
    void sendChangeProperty(const QVariantHash& data);
    /**
     * @brief 选中某一节点
     * @param itemId
     * @param itemType
     */
    void sendSelectItem(const PublicData::PelItemData& nodeData);
    /**
     * @brief 选中某一节点
     * @param itemId
     * @param itemType
     */
    void sendSelectItemWithFriend(const PublicData::PelItemData& nodeData, const QList<PublicData::PelItemData> &nodeDatalist);
    /**
     * @brief 添加节点属性
     * @param type
     */
    void sendAddItemProperty(const QString &id, ItemAndPropertyType type, int treeType, const QString &name = QString("itemName"));
    /**
     * @brief 保存故障树结构（送至服务端）
     */
    void sendSaveTreeNode(TreeNode *root);
    /**
     * @brief 删除节点（送至服务端）
     * @param id
     */
    void sendDeleteTreeNode(const QStringList &id);
    /**
     * @brief 展示故障树的属性
     * @param treeData
     */
    void sendShowTreeProperty(const QVariantHash &itemData);
    /**
     * @brief 更新故障树属性（发送至服务端）
     * @param treeData
     */
    void sendUpdateProjectPropertyReq(const QVariantHash &itemHash, const PublicData::TreeOperationType type);
    void sendLoadTreeNodesReq(const QString &treeId);
    void sendLoadDragShowNodesReq(const QString &treeId);

private:
    void connectEventBus();
    void disconnectEventBus();

private:
    GraphicsItemsHandle(QObject *parent = 0);
    ~GraphicsItemsHandle();
    GraphicsItemsHandle(const GraphicsItemsHandle& rParam) = delete;
    const GraphicsItemsHandle& operator = (const GraphicsItemsHandle &rParam) = delete;
};
#endif // GRAPHICSITEMSHANDLE_H

