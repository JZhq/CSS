/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         说明
*@author        luox
*@date          2018-05-24
**/
#include "graphicsitmshandle.h"
#include "common/propertycommon.h"
#include <QDebug>
#include <QMessageBox>
#include <QVariantHash>



void GraphicsItemsHandle::onSelectItem(const QString &itemName)
{
    qDebug()<< itemName;
}


void GraphicsItemsHandle::onClearEditView(const QVariantHash &data)
{
    emit signalClearEditView();
}

void GraphicsItemsHandle::onPitchFaultTree(const QVariantHash &hash)
{
//    PublicData::FaultTreeData treeData = hash[FAULTTREE_INFO].value<PublicData::FaultTreeData>();
//    emit signalPitchFaultTree(treeData);
}

void GraphicsItemsHandle::onLoadTreeNodes(const QVariantHash &hash)
{
//    if(hash[OPERATE_METHOD] == Operate::Respond)
//    {
//        TreeNode treeNode = hash[FAULTTREE_NODE].value<TreeNode>();
//        emit signalLoadTreeNodesRes(&treeNode);
//    }
}

void GraphicsItemsHandle::onSetNodeProperty(const QVariantHash &hash)
{
    //移除消息类型项
//    QVariantHash hashValue = hash;
//    hashValue.remove("event.topic");

//    emit signalSetNodesProperty(hashValue);
}

void GraphicsItemsHandle::onLoadDisplayTreeNodes(const QVariantHash &hash)
{
//    if(hash[OPERATE_METHOD] == Operate::Respond && hash[LOADNODES_DATA_USE] == NodesDataUse::DISPLAY)
//    {
//        //展示所用的故障树节点
//        TreeNode treeNode = hash[FAULTTREE_NODE].value<TreeNode>();
//        emit signalLoadDisplayNodesRes(&treeNode);
//    }
//    else if(hash[OPERATE_METHOD] == Operate::Respond && hash[LOADNODES_DATA_USE] == NodesDataUse::DRAGSHOW)
//    {
//        //拖拽故障树所用的节点
//        TreeNode treeNode = hash[FAULTTREE_NODE].value<TreeNode>();
//        emit signalDragShowNodesRes(&treeNode);
//    }
}

void GraphicsItemsHandle::onDrawItemName(const QVariantHash &hash)
{
//    QString id = hash[FAULTTREE_NODE_ID].toString();

//    if(hash[FAULTTREE_NODE_NAME_TYPE].toString() == "name")
//    {
//        QString name = hash[FAULTTREE_NODE_NAME].toString();
//        emit signalDrawItemName(id,name);
//    }
//    else if(hash[FAULTTREE_NODE_NAME_TYPE].toString() == "varName")
//    {
//        QString varName = hash[FAULTTREE_NODE_VARNAME].toString();
//        emit signalDrawItemVarName(id,varName);
//    }
}


void GraphicsItemsHandle::onUpdateTreeProperty(const QVariantHash &hash)
{
    //emit signalUpdateTreeProperty(hash);
}

void GraphicsItemsHandle::onSaveProperty(const QVariantHash &hash)
{
    Q_UNUSED(hash);

    emit signalSaveProperty();
}


void GraphicsItemsHandle::sendLoadTreeNodesReq(const QString &treeId)
{
//    QVariantHash hash;
//    hash[OPERATE_METHOD] = Operate::Requset;
//    hash[LOADNODES_DATA_USE] = NodesDataUse::DISPLAY;
//    hash[FAULTTREE_ID] = treeId;

//    emit signalLoadTreeNodes(hash);
}

void GraphicsItemsHandle::sendLoadDragShowNodesReq(const QString &treeId)
{
//    QVariantHash hash;
//    hash[OPERATE_METHOD] = Operate::Requset;
//    hash[LOADNODES_DATA_USE] = NodesDataUse::DRAGSHOW;
//    hash[FAULTTREE_ID] = treeId;

//    emit signalLoadTreeNodes(hash);
}

void GraphicsItemsHandle::sendChangeProperty(const QVariantHash &data)
{
    if(!data.isEmpty())
    {
        emit signalChangeProperty(data);
    }
}

void GraphicsItemsHandle::sendSelectItem(const PublicData::PelItemData &nodeData)
{
    QVariantHash hashValue;
    hashValue.insert(PrimitivePropertyConstants::ITEMDATA,QVariant::fromValue(nodeData));
    emit signalSelectItem(hashValue);
}

void GraphicsItemsHandle::sendSelectItemWithFriend(const PublicData::PelItemData &nodeData, const QList<PublicData::PelItemData> &nodeDatalist)
{
    QVariantHash hashValue;
    hashValue.insert(PrimitivePropertyConstants::ITEMDATA,QVariant::fromValue(nodeData));
    hashValue.insert(PrimitivePropertyConstants::ITEMDATAFRIEND, QVariant::fromValue(nodeDatalist));
    emit signalSelectItem(hashValue);
}

void GraphicsItemsHandle::sendAddItemProperty(const QString &id,ItemAndPropertyType type,int treeType, const QString &name)
{
    QVariantHash hashValue;
    hashValue.insert(PrimitivePropertyConstants::ITEMTYPE,type);
    hashValue.insert(PrimitivePropertyConstants::ITEMID,id);
    hashValue.insert(PrimitivePropertyConstants::ITEMNAME, name);

    emit signalAddItemProperty(hashValue);
}

void GraphicsItemsHandle::sendSaveTreeNode(TreeNode *root)
{
    QVariantHash hashValue;

    emit signalSaveTreeNode(hashValue);
}

void GraphicsItemsHandle::sendDeleteTreeNode(const QStringList &id)
{
//    QVariantHash hashValue;
//    hashValue[OPERATE_METHOD] = Operate::Requset;
//    hashValue[FAULTTREE_NODE_ID] = id;

//    emit signalDeleteTreeNode(hashValue);
}

void GraphicsItemsHandle::sendShowTreeProperty(const QVariantHash &itemData)
{
    //映射故障树信息
    QVariantHash hash;

    // emit signalShowFaultTreeProperty(hash);
}


void GraphicsItemsHandle::sendUpdateProjectPropertyReq(const QVariantHash &itemHash,const PublicData::TreeOperationType type)
{
//    hash[OPERATE_METHOD] = Operate::Requset;
//    hash[OPERATE_TYPE] = type;
//    hash[FAULTTREE_INFO] = QVariant::fromValue(treeData);
    emit signalUpadteFaultReq(itemHash);
}



void GraphicsItemsHandle::connectEventBus()
{

}

void GraphicsItemsHandle::disconnectEventBus()
{
}

GraphicsItemsHandle::GraphicsItemsHandle(QObject *parent):QObject(parent)
{
    connectEventBus();
}

GraphicsItemsHandle::~GraphicsItemsHandle()
{
    disconnectEventBus();
}
