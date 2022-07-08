#ifndef ITEMGLOBAL
#define ITEMGLOBAL
#include <QGraphicsItem>
#include "common/configureglobal.h"

const float G_SCENEWIDTH_PERCENT    = 1366.0;       // 场景大小单位为百分比时的默认宽度
const float G_SCENEHEIGHT_PERCENT   = 768.0;        // 场景大小单位为百分比时的默认高度

static const float ITEM_Z_COMMON       = 5.0;

class ItemGlobal
{
public:  
    enum Location   //区域位置
    {
        Location_Invalid=0X00,
        TopLeft         =0X01,
        Location_p1     =TopLeft,
        Location_start  =TopLeft,
        TopRigth        =0X02,
        Location_p2     =TopRigth,
        Location_end    =TopRigth,
        TopMiddle       =0X04,
        Location_p3     =TopMiddle,
        BottomLeft      =0X08,
        BottomRight     =0X10,
        BottomMiddle    =0X20,
        LeftMiddle      =0X40,
        RightMiddle     =0X80
    };

    enum CommondType //命令类型
    {
        AddItem,        // 增加
        CopyItem,       // 拷贝
        CutItem,        // 剪切
        DeleteItem,     // 删除
        GroupItem,      // 成组
        ModifyItem,     // 修改属性
        ModifyWidthHeight,  // 修改长宽
        ModifyGroupItem,    // 修改组合图元属性
        PasteItem,      // 粘贴
        RedoItem,       // 重做
        UndoItem,       // 撤销
        UnGroupItem,    // 解组
        RespondExec,    // 回复运行时参数
        AcceptExec,     // 接受运行时参数
        ItemDataGetvalue,
        SetAnimationPro,
    };

    enum FillType   // 填充类型
    {
        Solid,      // 实心填充
        Hollow      // 空心填充（不填充）
    };

    ~ItemGlobal();

    static ItemGlobal *getInstance();

    void setCurrentDrawType(ItemAndPropertyType type);
    ItemAndPropertyType getCurrentDrawType() const;

private:
    ItemGlobal();

private:
    ItemAndPropertyType m_currentDrawType;
};


#endif // ITEMGLOBAL

