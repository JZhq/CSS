#ifndef CONFIGUREGLOBAL
#define CONFIGUREGLOBAL

#include <QString>
#include <qobject.h>

const static QString FrameworkNode = "FrameworkNode";
const static QString ModuleNode = "ModuleNode";
const static QString ProjectTree = "ProjectTree";

//图元的执行状态
enum ItemStatus
{  
    NotRun = 0,//未执行（初始状态）
    Run,        //执行状态
    End,       //结束状态
    Fault,     //故障
};

//-----------------------图元类型------------------------------
enum ItemAndPropertyType
{
    NoneType = 0,           //  没有图元
    BaseItemType = 1,       //  图元基类类型
    ModuleType = 2,         //  模块节点
    FrameworkType = 3,      //  框架节点
    PolyLine = 4,           //  折线段
    RegionSelect = 5        //  框选
}; 
//-------------------------------------------------------------
Q_DECLARE_METATYPE(ItemAndPropertyType)

#endif // CONFIGUREGLOBAL

