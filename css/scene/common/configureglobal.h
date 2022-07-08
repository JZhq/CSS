#ifndef CONFIGUREGLOBAL
#define CONFIGUREGLOBAL

#include <QString>
#include <qobject.h>

static const QString DescribeNodeStr    = QStringLiteral("描述节点");
static const QString ParamNodeStr       = QStringLiteral("监测节点");
static const QString TestNodeStr        = QStringLiteral("测试节点");
static const QString PluginNodeStr      = QStringLiteral("插件节点");
static const QString BasicNodeStr       = QStringLiteral("基础知识节点");
static const QString AndArithmeticStr   = QStringLiteral("与算法");
static const QString OrArithmeticStr    = QStringLiteral("或算法");
static const QString WrongArithmeticStr = QStringLiteral("非算法");
static const QString EditArithmeticStr  = QStringLiteral("可编辑算法");
static const QString ArithmeticStr      = QStringLiteral("算法节点");
static const QString FaultTreeDatas     = QStringLiteral("FaultTreeDatas");

static const QString TaskAnalyTree = QStringLiteral("任务分析故障树");
static const QString TaskDiagnoseTree = QStringLiteral("任务诊断故障树");
static const QString SysDiagnoseTree = QStringLiteral("系统诊断故障树");
static const QString BaseKnowledgeTree = QStringLiteral("基础知识管理树");
static const QString BaseReferenceTree = QStringLiteral("基础知识引用树");


static const QString FaultTreeProperty = QStringLiteral("FaultTreeProperty");
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
    Arithmetic = 2,         //  算法节点
    BranchNode = 3,         //  描述节点
    LeafNode = 4,           //  参数节点
    TestNode = 5,           //  测试节点
    PlugNode = 6,           //  插件节点
    PolyLine = 7,           //  折线段
    FaultTree = 8,          //  故障树
    RegionSelect = 9,       //  框选
    BaseKnowledgeNode = 10  //  基础知识节点
}; 
//-------------------------------------------------------------
Q_DECLARE_METATYPE(ItemAndPropertyType)

#endif // CONFIGUREGLOBAL

