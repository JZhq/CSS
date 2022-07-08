#ifndef PUBLICDATA_H
#define PUBLICDATA_H

#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QMetaType>
#include <QObject>
#include <QPointF>
#include <QString>

#include "configureglobal.h"
#include "scene_global.h"

namespace PublicData {

const QString VERSION = "2.24";

// 插件类型集合
namespace PluginTypes {
    const QString AntennaTraceSatelliteDiag = "AntennaTraceSatelliteDiag";
    const QString TaskWorkFlowDiag          = "TaskWorkFlowDiag";
}

// 报告类型
namespace ReportType {
    const QString TaskQualityReport  = "TaskQualityReport";     // 接收质量报告
    const QString TaskAnalysisReport = "TaskAnalysisReport";    // 任务分析报告
    const QString TaskFDReport       = "TaskDianoseReport";          // 任务故障诊断报告
    const QString SystemFDReport     = "SystemDianoseReport";        // 系统故障诊断报告
    const QString AssessReport       = "AssessReport";               // 评估报告
    const QString MaintainReport     = "MaintainReport";             // 维护方案报告
}

// 上传的文件类型
namespace UploadType {
    const QString uploadDll = "uploadDll";
    const QString UploadPluginFlowChart    = "UploadPluginFlowChart"; // 插件流程图
    const QString UploadFunctionFlowChar   = "UploadFunctionFlowChar"; // 函数流程图
    const QString UploadArrangeTime        = "UploadArrangeTime";   // 服务器编排时间
}

// 下载的文件类型
namespace DownloadType {
    const QString DownloadPluginFlowChart    = "DownloadPluginFlowChart"; // 插件流程图
    const QString DownloadFunctionFlowChar   = "DownloadFunctionFlowChar"; // 函数流程图
    const QString DownloadTaskLog            = "DownloadTaskLog"; // 任务日志
    const QString ProcessData                = "ProcessData"; // 树过程数据
    const QString AssessReport               = "AssessReport";               // 评估报告
    const QString MaintainReport             = "MaintainReport";             // 维护方案报告
}

// 系统诊断的发起方式
enum SysDiagMethod {
    FromKnowledge, // 从知识管理发起
    FromHome, // 从首页发起
    FromDiag, // 从诊断界面发起
};

enum TreeStatus {
    Audit,          // 审核
    UnAudit,        // 未审核
    RejectAudit,    // 取消审核
};

enum TreeOperationType {
    ImportIn, //导入
    ImportOut,//导出
    Copy,     //复制
    Save,     //保存
    Edit,      //编辑
    New        //新建

};
// 诊断类型
enum DiagType {
    TaskDiag,   // 任务诊断
    SystemDiag, // 系统诊断
};

enum TreeType
{
    TaskAnalyTree = 0x01,      //任务分析树
    TaskDiagnoseTree = 0x02,   //任务诊断树
    SysDiagnoseTree = 0x03,     //系统诊断故障树
    BaseKnowledgeTree = 0x04
};

// 节点类型(2:算法节点  3:描述节点  4:检测节点 5:测试节点 6:插件节点)
enum NodeType {
    AlgorithmNode = 2,
    DescNode = 3,
    DetectionNode = 4,
    TestNode = 5,
    PluginNode = 6,
    BaseKnowledgeNode = 10
};


//  编辑场景： 0 : 故障树管理、1 : 基础知识管理
enum ScenesEnum {
    Fault_Tree_Mng = 0,
    Base_Know_Tree_Mng = 1
};

// 基础知识引用方式： 0 : 结构引用  1 : 结构和数据引用
enum ReferenceModeEnum{
    Struct_Reference,
    Data_Reference
};

namespace taskProcJsonKey {
enum NodeState {
    NotRun = 0,//未执行（初始状态）
    Run,        //执行状态
    End,       //结束状态
    Fault,     //故障
};

const QString NODE_TYPE = "NODE_TYPE";
const QString NODE_STATE = "NODE_STATE";
const QString NODE_NAME   = "NODE_NAME";    // 节点名称

// 树信息
namespace tree {
    const QString TREE_NAME        = "TREE.NAME";          // 名称
    const QString TREE_TYPE        = "TREE.TYPE";          // 树类型
    const QString TREE_PHASE       = "TREE.PHASE";         // 链路旋向
    const QString TREE_COUPLING    = "TREE.COUPLING";      // 链路耦合方式
    const QString TREE_FREQANGE    = "TREE.FREQANGE";      // 卫星通道频率
    const QString TREE_FREQUENCY   = "TREE.FREQUENCY";
    const QString TREE_CHANNELRATE = "TREE.CHANNELRATE";
    const QString TREE_STATE       = "TREE.STATE";         // 故障状态（正常、异常、未执行）

    const QString TREE_ANTENNA_ID   = "TREE.antennaId";      // 天线
    const QString TREE_CHANNEL_TYPE = "TREE.channelType";    // 通道类型    ：系统诊断故障树 <->  设备名称
    const QString TREE_CHANNEL_ID   = "TREE.channelId";      // 通道名字   ： 系统诊断故障树 <->   设备ID
}

// 描述节点过程
namespace desc {
    const QString IS_ROOT     = "IS_ROOT";
    const QString VAR_NAME    = "VAR_NAME";     // 变量名称
    const QString VAR_VAL     = "VAR_VAL";      // 节点参数值
    const QString VAR_RESULT  = "VAR_RESULT";   // 结果
    const QString RESULT_INFO = "RESULT_INFO";  // 描述信息
    const QString HANDLE_METHOD  = "HANDLE_METHOD"; // 处置措施
    const QString DEV_NAME        = QStringLiteral("设备名称");
}

// 检测节点过程
namespace detc {
    const QString VAR_NAME       = "VAR_NAME";      // 参数变量名称
    const QString DEV_NAME       = "DEV_NAME";      // 设备名称
    const QString DEV_PARAM      = "DEV_PARAM";     // 参数名称
    const QString VAR_VAL        = "VAR_VAL";       // 参数值
    const QString VAR_MEANS      = "VAR_MEANS";     // 参数值含义
    const QString INTEGRITY      = "INTEGRITY";     // 完整性
}

// 算法节点执行过程
namespace algorithm {
    const QString EXPRESSION  = "EXPRESSION";
    const QString RESULT      = "RESULT";
    const QString VAR_INFO    = "VAR_INFO";
    const QString DESC_INFO   = "DESC_INFO";
}

// 测试节点
namespace test {
    const QString TEST_PLAN_ID          = "TEST_PLAN_ID";
    const QString TEST_TYPE             = "TEST_TYPE";
    const QString TEST_START_TIME       = "TEST_START_TIME";
    const QString TEST_END_TIME         = "TEST_END_TIME";
    const QString PREDICT_TEST_TIME     = "PREDICT_TEST_TIME";
    const QString FAILD_REASON          = "FAILD_REASON";
    const QString VAR_NAME_FREQ         = "VAR_NAME_FREQ";
    const QString VAR_NAME_SIGNAL       = "VAR_NAME_SIGNAL";
    const QString VAR_VAL_FREQ          = "VAR_VAL_FREQ";
    const QString VAR_VAL_SIGNAL        = "VAR_VAL_SIGNAL";
    const QString GRAPHIC_PATH          = "GRAPHIC_PATH";      // 测任务框图路径

    const QString UP_FREQ_DEV_NAME          = "UP_FREQ_DEV_NAME";
    const QString UP_FREQ_VAL               = "UP_FREQ_VAL";
    const QString UP_FREQ_ATTENUATION       = "UP_FREQ_ATTENUATION";
    const QString DOWN_FREQ_DEV_NAME        = "DOWN_FREQ_DEV_NAME";
    const QString DOWN_FREQ_VAL             = "DOWN_FREQ_VAL";
    const QString DOWN_FREQ_ATTENUATION     = "DOWN_FREQ_ATTENUATION";
    const QString DATA_INDOOR_DEV_NAME      = "DATA_INDOOR_DEV_NAME";
    const QString DATA_INDOOR_ATTENUATION   = "DATA_INDOOR_ATTENUATION";
    const QString TEST_INDOOR_DEV_NAME      = "TEST_INDOOR_DEV_NAME";
    const QString TEST_INDOOR_ATTENUATION   = "TEST_INDOOR_ATTENUATION";
    const QString MODULATOR_NAME            = "MODULATOR_NAME";
    const QString MODULATOR_LEVEL           = "MODULATOR_LEVEL";
}
// 插件节点
namespace plugin {
    const QString ERRORS      = "errors";
    const QString PLUGIN_NAME = "pluginName";
    const QString PLUGIN_TYPE = "pluginType";
    const QString RATE        = "rate";
    const QString RESULT      = "result";
}
}


struct  _EnvirnmentalFactorInfo{
public:
    _EnvirnmentalFactorInfo(){}

int id;
QString serialNumber;
QString jobTaskId;
QString startTime;
QString endTime;
QString dataJson;
};

// 跟踪精度因素
struct  _TrackingAccurayInfo{
public:
    _TrackingAccurayInfo(){}

int id;
QString trackType;
QString data;
QString startTime;
QString endTime;
QString satelliteName;
QString antennaId;
};

struct  _AnaylsisToolFilter{
public:
    _AnaylsisToolFilter(){}

    QString serialNumber;
    QString jobTaskId;
    QString startTime;
    QString endTime;
    QString type;
    QString satelliteName;
    QString antennaId;
    QString fw_range;     // 引导方位角度范围 格式： a~b
    QString fy_range;     // 引导俯仰角度范围 格式： a~b
};


struct  _AntennaDirectInfo{
public:
    _AntennaDirectInfo(){}
    QString time;
    QString fy_val;
    QString fw_val;
};

//故障树属性
struct FaultTreeData
{

    QString id;
    QString treeName;      // 树的名字
    int treeType;          // 树类型(1:任务分析故障树 2:任务诊断故障树 3:系统诊断故障树 4:基础知识故障树)
    int treePriority;      // 树的优先级
    QString linkPhase;     // 链路旋向
    QString couplingType;  // 链路耦合方式
    QString createTime;    // 创建时间
    QString freqRange;     // 链路频段
    QString freqUency;	   // 卫星通道频率
    QString channelRate;   // 卫星通道码速率
    int  auditStatus;        // 审核状态
    bool  deleteFlag;      // 删除标识
    int   userId;		   // 用户id
    QString versionNumber; // 知识版本
    QString satelliteCode; // 卫星代号
    QString antennaId;     // 天线类型
    QString channelType;   // 通道类型 : 系统故障诊断树 ->设备名称
    QString channelId;     // 通道编号 : 系统诊断故障树 ->设备ID

    FaultTreeData()
    {
        id    = "";
        treeType = 0;
        treePriority = 0;
        auditStatus = TreeStatus::UnAudit;
        deleteFlag = false;
    }

    FaultTreeData& operator= ( const FaultTreeData &other )
    {
        if(&other == this)
            return *this;
        id = other.id;
        treeName = other.treeName;
        treeType = other.treeType;
        treePriority = other.treePriority;
        linkPhase = other.linkPhase;
        satelliteCode = other.satelliteCode;
        couplingType = other.couplingType;
        createTime = other.createTime;
        freqRange = other.freqRange;
        freqUency = other.freqUency;
        channelRate = other.channelRate;
        auditStatus = other.auditStatus;
        deleteFlag = other.deleteFlag;
        userId = other.userId;
        versionNumber = other.versionNumber;
        antennaId = other.antennaId;
        channelType = other.channelType;
        channelId = other.channelId;

        return *this;
    }
    FaultTreeData(const FaultTreeData &other)
    {
        id = other.id;
        treeName = other.treeName;
        treeType = other.treeType;
        treePriority = other.treePriority;
        satelliteCode = other.satelliteCode;
        linkPhase = other.linkPhase;
        couplingType = other.couplingType;
        createTime   = other.createTime;
        freqRange = other.freqRange;
        freqUency = other.freqUency;
        channelRate = other.channelRate;
        auditStatus = other.auditStatus;
        deleteFlag  = other.deleteFlag;
        userId = other.userId;
        versionNumber = other.versionNumber;
        antennaId = other.antennaId;
        channelType = other.channelType;
        channelId = other.channelId;
    }

};


struct FaultIndexData
{
public:
    FaultIndexData():times(0){
    }

    int times;              //触发次数
    QString indexId;	    // 索引编号
    QString indexName;      //索引名字
    QString faultType;       //故障类型
    QString description; 	//故障描述
    QString suggest;     	//处置建议
    QString guide;  			//处置向导
    QString state;    		//状态
};

struct FaultIndexRefData{
public:
    FaultIndexRefData(){}

    int id;
    QString indexId;
    QString refId;
};

//图元基本属性（用于保存）
struct PelItemData
{
    QString id;       //图元标识号(UUID)
    QString parentId; //父节点id
    QString name;     //图元名称
    QPointF pos;      //坐标系中的位置
    ItemAndPropertyType itemType;//图元类型
    QString treeId;      //所属树的id
    QString nodeProperty;//节点属性（json）

    PelItemData()
        : id(QString()), parentId(QString()), name(QString()), pos(QPointF())
        , itemType(ItemAndPropertyType::NoneType),treeId(QString()), nodeProperty(QString())
    {

    }
    PelItemData& operator= ( const PelItemData &other )
    {
        if(&other == this)
            return *this;
        id = other.id;
        name = other.name;
        pos = other.pos;
        itemType = other.itemType;
        parentId = other.parentId;
        treeId = other.treeId;
        nodeProperty = other.nodeProperty;
        return *this;
    }

    bool operator== ( const PelItemData &other )
    {
        return id == other.id;
    }

    PelItemData(const PelItemData &other)
    {
        id = other.id;
        name  = other.name;
        pos   = other.pos;
        itemType   = other.itemType;
        parentId = other.parentId;
        treeId = other.treeId;
        nodeProperty = other.nodeProperty;
    }

};

//描述节点属性
struct DescribeNodeProperty
{
    DescribeNodeProperty()
    {
        id = "";
        trueResult = true;
        falseResult = false;
    }

    QString id;
    QString name;//描述节点名称
    QString faultInfo;//故障描述信息
    QString normalInfo;//正常描述信息
    QString variableName;//变量名
    int     trueValue;//正常对应值
    int     fasleValue;//故障对应值
    QString normalDisposalMea;//正常处置措施
    QString falutDisposalMea;//异常处置措施
    bool trueResult; // 正常描述结果
    bool falseResult; // 异常描述结果
};

//参数节点属性
struct ParamNodeProperty
{
    ParamNodeProperty()
    {
        id = "";
    }

    QString id;
    QString name;
    QString variableName;//变量名
    QString deviceParam;//设备参数
    QString deviceType;//设备类型
    bool    isEnumValue;//是否存在枚举值
    int     beginValue;//begin
    int     endValue;  //end
    QString setValue;  //设置值
    QString minValue; //最小值
    QString maxValue; //最大值
    //add by led for 20190330
    QString enmuTypeKey;//枚举类型名称----------add
    QString enmuValue;//枚举值-----------------add

//    QString disposalMea;//处置措施

};

//插件节点属性
struct PluginNodeProperty
{
    PluginNodeProperty()
    {
        id = "";
    }

    QString id;
    QString name;//节点名称
    QString pluginName;//插件名称
    QString varName;//变量名
};

//测试节点属性
struct TestNodeProperty
{
    TestNodeProperty()
    {
        id = "";
    }
    QString id;
    QString name;         //节点名称
    QString taskType;     //测试类型
    QString taskName;     //测试名称
    QString testFreq;     //测试频率
    QString test_tsqscdp;//调试器输出电平
    QString test_cesbpqsj;//测试上变频器衰减
    QString test_cegdfsjsj;//测试光端发射机衰减（或可调增益）
    QString test_gzgdfsjsj;//跟踪光端发射机衰减（或可调增益）
    QString test_gzxbpqsj; //跟踪下变频器衰减（或可调增益）
    QString test_zxplcs;   //中心频率参数
    QString test_zxplbl;   //中心频率变量
    QString test_xhqucs;   //信号强度参数
    QString test_xhqdbl;   //信号强度变量

    QString devLink;        //设备链路
    QString test_tzqsb;   //调制器设备
    QString test_sbpsb;     //上变频设备名称
    QString test_sncegdj;  //室内测试光端机
    QString test_swcegdj;   //室外测试光端机
    QString test_snsjgdj;   //室内数据光端机
    QString test_swsjgdj;   //室外数据光端机
    QString test_xbpsb;        //下变频设备
    double test_yjcesj;    //预计测试时间
    QString test_cskssj;    //测试开始时间
    double test_zxcssj;   //执行超时时间
    double test_xycssj;   //响应超时时间
};

//算法节点属性
struct ArithmeticNodeProperty
{
    ArithmeticNodeProperty()
    {
        id = "";
    }
    QString id;
    QString name;    //名称
    QString algExp;  //表达式
    QString info;
};

struct BaseKnowNodeProperty{
    BaseKnowNodeProperty(){

    }
    QString id;
    QString name;
    QString varName;
    QString deviceName;
};

// 软件界面
struct Page
{
    // 当前所有界面
    enum AllPage {
        Home,

        RecvTask,
        DiagTask,

        FaultTreeMng,
        TestTemplateMng,
        FunctionMng,
        PluginMng,
        AnaylsisToolMng,
        MonitorPointMng,
        StandardResMng,
        AssessModelMng,

        LogMng,
        AssessMng,
        ToolMng,

        UserMng,
        FaultInfoMng
    };
    Page()
    {
        currentPage = Home;
    }

    AllPage currentPage;
};

// 用户结构
struct User
{
    enum Role
    {
        MANANGER,  // 管理员
        OPERATOR,   // 操作员
        SUPER_MANAGER // 高级管理员
    };

    int id;
    int role;
    QString account;
    QString password;
    QString name;
    QString phone;
    QString email;
    QDateTime createTime;
    bool isActive;
    QString comments;

    User()
    {
        id = 0;
        role = OPERATOR;
        isActive = false;
    }
};


struct ThresholdGroupUserRef{
    int id;
    QString userName;
    QString groupIDs;
    ThresholdGroupUserRef(){
        id=0;
    }
};

//监测点组信息
struct ThresholdGroupInfo{
    int id;
    QString groupName;
    QString groupDescripe;
    int groupType;
    QString childIDs;
    int groupShow;
    ThresholdGroupInfo(){
        id=0;
    }
};
//复杂监测点信息
struct ComplexThreshold{
    int id;
    QString complex_name;
    QString complex_child_list;
    QString complex_avrName_list;
    QString simple_child_list;
    QString complex_descripe;
    QString complex_advise;
    QString complex_failure;
    QString complex_relation;
    QString complex_range;
    QString fault_IndexId;
    ComplexThreshold(){
        id = 0;
    }
};
//add by led
struct Threshold
{
    enum Compare
    {
        MIN,//最小值
        MAX,//最大值
        EQUAL,//等值
        MIN_MAX//最大最小值
    };
    int id;
    QString createdTime;
    int compare;
    QString compareWay;
    QString thresholdValue;
    //QString fault_reason;
    //补充数据
    QString monitorPointName;
    QString monitorPointId;
    QString deviceName;
    QString deviceId;
    QString faultIndexId;
    QString deviceType;
    QString maxThreshold;
    QString minThreshold;
    QString enumType;
    QString enumValue;
    QString faultReason;
    QString advice;
    QString indicentDescrip;
    bool isComplex;
    int relationID;
    QString variableName;
    Threshold() {
        id=0;
        compare=MIN;
        isComplex = false;
    }
};

//函数管理信息
struct FunctionInfo{
    int id;
    QString functionName;
    QString functionType;
    QString createTime;
    QString functionPath;
    QString chartPath;
    QString paramNumber;
    QString functionDescripe;
    FunctionInfo(){
        id=0;
    }
};

//插件管理信息
struct PluginInfo{
    int id;
    QString pluginName;
    QString pluginType;
    QString createTime;
    QString pluginPath;
    QString chartPath;
    QString inputJson;
    QString pluginDescripe;
    QString exp;
    QByteArray byteData;
    PluginInfo(){
        id=0;
    }
};

//故障信息
struct FaultInfo
{
    QString id;
    int faultSource;
    QString faultId;
    QString faultReason;
    QString jobTaskId;
    QString diangnosisTaskId;
    QString ClosedLoopState;
    QString deviceId;
    QString deviceName;
    QString monitorId;
    QString monitorName;
    QString faultTime;
    QString CloseLoopTime;
    QString statusInfo;
    QString faultAdvice;
    QString faultIndex;
    QString taskSerialNumber;
    FaultInfo() {}
};

//查找故障信息条件
//故障信息
struct FindFaultInfoFilter
{
    QString deviceName;
    QString startTime;
    QString monitorName;
    QString endTime;
    QString faultReason;
    int faultSource;
    QString faultIndex;
    QString faultId;
    QString taskSerialNumber;
    QString jobTaskId;
    int closedLoopState;
    FindFaultInfoFilter() {}
};

//测试模板时间结构体
struct TestModeTime
{
    QString test_zxcssj;
    QString test_xycssj;
    QString test_csrwzxsjmx;
    QString flow_chart_path;
    QString duration;
    TestModeTime() {
        test_zxcssj="";
        test_xycssj="";
        test_csrwzxsjmx="";
        flow_chart_path="";
        duration="";
    }
};

//测试模板设备参数结构体
struct TestModeDeviceParams
{
    QString testName;//测试模板名称
    QString modDeviceID;//调制器类型
    QString modOutPutLevel;//调制器输出电平
    QString ucDeviceID;//下变频器类型
    QString ucTestFrq;//上变频器测试频率
    QString ucTestConAtt;//测试上变频器衰减
    QString dcDeviceID;//下变频器类型
    QString dcTestFrq;//下变频器测试频率
    QString dcFrqConAtt;//数据/跟踪下变频器衰减
    QString iottDeviceID;//室内光端机类型
    QString iottTestLightAtt;//室内测试光端机发射衰减
    QString oottDeviceID;//未使用
    QString iotdDeviceID;//未使用
    QString ootdLightTranAtt;//室外数据发射光端机衰减
    QString ootdDeviceID;//室外数据发射光端机类型
//    TestModeDeviceParams() {
//        memset(this,0,sizeof(TestModeDeviceParams));
//    }
};

struct TestModeData {
    int id;
    QString testType;
    QString testName;
    QString parmsJson;
    TestModeData() {
        id=0;
        testType="";
        testName="";
        parmsJson="";
    }
};

// 状态
struct ChannelState
{
    enum Monitor {
        Server,
        RemoteDB,
        ShareDisk
    };

    enum State {
        Online,
        Offline
    };

    int curMonitor;
    int curState;

    ChannelState()
    {
        curMonitor = Server;
        curState = Offline;
    }
};

// 故障
struct Fault {
    int id;
    QString deviceID;
    QString deviceName;
    QString monitorID;
    QString monitorName;
    QString faultReason;
    //QDateTime faultTime;
    QString faultTime;
    QString closedLoopState;

    Fault()
    {
        id = -1;
    }
};

// 日志内容
struct LogInfo {
    enum LogLevel
    {
        Info = 0,
        Fatal = 1,
        Warning = 3
    };

    int id;
    int level;
    QString info;
    QDateTime createTime;
    int pri;
    int soft;
    QString taskID;
    QString desc;
};

// 用户筛选值
struct UserFilter {
    QString userName;
    bool isContainManager;
    bool isContainOperator;
    bool isContainSuperManager;

    UserFilter()
    {
        isContainManager = true;
        isContainOperator = true;
        isContainSuperManager = true;
    }
};

//门限筛选值
struct ThresholdFilter
{
    QString deviceType;
    QString deviceName;
    QString monitorName;
    ThresholdFilter() {
        deviceName="";
        deviceType="";
        monitorName="";
    }
};

//函数筛选
struct FunctionFilter
{
    QString functionName;
    QString functionType;
    QString startTime;
    QString endTime;
    FunctionFilter() {
        functionName="";
        functionType="";
        startTime="";
        endTime="";
    }
};

//函数筛选
struct PluginFilter
{
    QString pluginName;
    QString pluginType;
    QString startTime;
    QString endTime;
    PluginFilter() {
        pluginName="";
        pluginType="";
        startTime="";
        endTime="";
    }
};

// 日志筛选值
struct LogFilter
{
    QString startTime;
    QString endTime;
    bool isContainInfo;
    bool isContainWarning;
    bool isContainFatal;

    LogFilter()
    {
        isContainInfo = false;
        isContainWarning = false;
        isContainFatal = false;
    }
};

// 接收任务筛选值
struct RecvTaskFilter
{
    QString taskID;
    QString startTime;
    QString endTime;
    QString satellite;
    QString taskState;
    bool isAutoUpdate;
};

// 系统诊断任务筛选值
struct SysDiagnoseFilter
{
    QString devName;
    QString taskID;
    QString startTime;
    QString endTime;

    bool isNormal;
    bool isAbNormal;
    bool isAutoUpdate;

    SysDiagnoseFilter()
    {
        isNormal = false;
        isAbNormal = false;
        isAutoUpdate = false;
    }
};

// 任务下的分析任务和诊断任务筛选值
struct AlAndDiagTaskFilter
{
    QString ts;
    QString job;
};

// devices.xml 中的数据
struct Device
{
    QString id;
    QString name;
    QString alias;
    QString stationId;
    QString subSystemId;
    QString deviceType;
    QString deviceTypeName;
    QString dataTemplateId;
    QString receivePortId;
    QString receiveIP;
    QString remoteIP;
    QString remotePortId;
    QString protocolId;
    QString commType;
    QString address;
    QString queryTime;
    QString comment1;
};

// *Points.xml 中的 comBox 结构中的 item 结构
struct  ComBoxItem {
    QString text;
    QString textAlias;
    QString value;
};

// *Points.xml 中的 comBox 结构
struct ComBox {
    QString id;
    QString name;
    QString begin;
    QString end;
    QList<ComBoxItem> items;
};

// *Points.xml 的结构
struct Point {
    QString id;
    QString alias;
    QString name;
    QString deviceId;
    QString dataType;
    QString length;
    QString value;
    QString minValue;
    QString maxValue;
    QString commandId;
    QString offset;
    QString alarmNumber;
    QList<ComBox>  comBox;
    QString unit;
};

// 接收任务结构
struct RecvTask {
    enum TaskStatus{
        Run,
        Finish
    };

    QString uuid;
    QString taskserialNumber;
    QString jobtaskID;
    QString satelliteName; // 卫星代号
    QDateTime firstDate;
    QDateTime lastDate;
    QString status; // 任务状态
    int qualityReport;
};

// 诊断任务分析信息
struct DiagnosistaskInfo {
public:
    // 诊断类型
    enum DiagnosisType {
        TaskDiagnosis,
        SystemDiagnosis,
        TaskAnaly
    };

    // 诊断状态 0正在执行，1执行完成，2取消执行，3等待执行, 4执行失败5, （拒绝）
    enum DiagnosisStatus {
        Run,
        Finish,
        Cancel,
        Wait,
        Failed,
        deny
    };

    // 诊断结果
    enum DiagnosisResult {
        Unknow,
        Fault,
        Success
    };

    int id;
    QString taskAlID;
    QString taskserialNumber;
    QString jobtaskID;
    QString devName;
    QString taskID;   // 诊断任务编号（MC年月日流水号，如：MC20180523001）
    int type;
    QDateTime startTime;
    QDateTime endTime;
    QDateTime expectEndTime;
    QDateTime createdTime;
    int status;
    int result;
    QString desc;
    int reportExist;
    int analyseReportExist;
    QDateTime dataStartTime;
    QDateTime dataEndTime;
};

// 分析诊断任务过程信息
class SCENE_SHARED_EXPORT AlDiagTaskProcessInfo
{
public:
    AlDiagTaskProcessInfo();

    // 执行状态
    enum state {
        Stop,   // 未执行
        Run,    // 执行
        Finish,  // 完成
        Failed   // 失败
    };

    int id;                     // COMMENT '自增长ID'
    QString nodeID;             // COMMENT '节点标识ID'
    int diagsisID;              // 分析诊断任务ID
    int channTreeID;            // COMMENT '通道与树关系ID'
    int runState;               // COMMENT '执行状态'
    QString result;             // COMMENT '结果（JSON）'
    QString taskSerialnumber;   // COMMENT '任务单流水号'
    QString jobtaskID;          // COMMENT '作业任务编号'
    int cchannID;               // COMMENT '通道ID'
    QString treeID; // COMMENT '树ID'

    //    friend COMMONDATASHARED_EXPORT QDataStream& operator>>(QDataStream &in, AlDiagTaskProcessInfo& data);
    //    friend COMMONDATASHARED_EXPORT QDataStream& operator<<(QDataStream &out, AlDiagTaskProcessInfo& data);
};

// 系统诊断任务过程信息
class SysDiagTaskProcessInfo
{
public:
    // 执行状态
    enum state {
        Stop,   // 未执行
        Run,    // 执行
        Finish,  // 完成
        Failed   // 失败
    };

    int id;
    int diagID;
    int systemDiagnosisRelationID;         // int(11) COMMENT '系统诊断任务与树关系ID(系统诊断任务与树关系表)'
    QString treeID;                        // varchar(255) COMMENT '树ID（系统诊断任务与树关系表中故障树ID）'
    QString nodeID;                        // int(11) COMMENT '节点标识ID'
    int runState;                          // decimal(5,0) COMMENT '执行状态'
    QString result;                        // ongtext COMMENT '结果（JSON）'
    QString dataStartTime;
    QString dateEndTime;
};

// 过程的基本树信息
class BasicProcessTreeInfos {

public:
    struct DescNode {
        QString id;
        QString attribute;
    };
    struct AlgorithmNode {
        QString id;
        QString attribute;
    };
    struct DetcNode {
        QString id;
        QString attribute;
    };
    struct PluginNode {
        QString id;
        QString attribute;
    };
    struct TestNode {
        QString id;
        QString attribute;
    };
    struct BaseKonwNode {
        QString id;
        QString attribute;
    };

public:
    int diagID;
    QString treeID;
    QString treeAttribute;

    QString dataStartTime;
    QString dataEndTime;

    QList<DescNode> descNode;
    QList<AlgorithmNode> algorithmNode;
    QList<DetcNode> detcNode;
    QList<PluginNode> pluginNode;
    QList<TestNode> testNode;
    QList<BaseKonwNode> baseNode;
};

//!
//! \brief 手动诊断树信息
//!
class ManualDiagTreeInfos {
public:
    struct Tree {
        QString treeID;
        QString treeName;
        bool isBelongToErrChann;
        bool isContainTestNode;
        QStringList testNodeProperties;
    };

    struct InvalidTimes {
        QString startTime;
        QString endTime;
    };

    QList<Tree> allTrees;
    QList<InvalidTimes> invalidTimes;
    int preTime;
    int nextTime;
    QString msgId;
};


class ManualTaskDiagExcuteInfo {
public:
    QString ts;
    QString job;
    QString startTime;
    QString endTime;
    QStringList treeIDs;
};

class ManualTaskDiagMsgID {
public:
    QString msgId;
};

}

Q_DECLARE_METATYPE(PublicData::ParamNodeProperty)
Q_DECLARE_METATYPE(PublicData::DescribeNodeProperty)
Q_DECLARE_METATYPE(PublicData::PluginNodeProperty)
Q_DECLARE_METATYPE(PublicData::TestNodeProperty)
Q_DECLARE_METATYPE(PublicData::ArithmeticNodeProperty)
Q_DECLARE_METATYPE(PublicData::Page)
Q_DECLARE_METATYPE(PublicData::User)
Q_DECLARE_METATYPE(PublicData::ThresholdGroupInfo)
Q_DECLARE_METATYPE(PublicData::ComplexThreshold)
Q_DECLARE_METATYPE(PublicData::Threshold)
Q_DECLARE_METATYPE(PublicData::FunctionInfo)
Q_DECLARE_METATYPE(PublicData::PluginInfo)
Q_DECLARE_METATYPE(PublicData::FaultInfo)
Q_DECLARE_METATYPE(PublicData::FindFaultInfoFilter)
Q_DECLARE_METATYPE(PublicData::TestModeTime)
Q_DECLARE_METATYPE(PublicData::TestModeDeviceParams)
Q_DECLARE_METATYPE(PublicData::TestModeData)
Q_DECLARE_METATYPE(PublicData::ChannelState)
Q_DECLARE_METATYPE(PublicData::Fault)
Q_DECLARE_METATYPE(PublicData::LogInfo)
Q_DECLARE_METATYPE(PublicData::UserFilter)
Q_DECLARE_METATYPE(PublicData::ThresholdFilter)
Q_DECLARE_METATYPE(PublicData::FunctionFilter)
Q_DECLARE_METATYPE(PublicData::PluginFilter)
Q_DECLARE_METATYPE(PublicData::LogFilter)
Q_DECLARE_METATYPE(PublicData::RecvTaskFilter)
Q_DECLARE_METATYPE(PublicData::SysDiagnoseFilter)
Q_DECLARE_METATYPE(PublicData::AlAndDiagTaskFilter)
Q_DECLARE_METATYPE(PublicData::Device)
Q_DECLARE_METATYPE(PublicData::ComBox)
Q_DECLARE_METATYPE(PublicData::Point)
Q_DECLARE_METATYPE(PublicData::RecvTask)
Q_DECLARE_METATYPE(PublicData::DiagnosistaskInfo)
Q_DECLARE_METATYPE(PublicData::AlDiagTaskProcessInfo)
Q_DECLARE_METATYPE(PublicData::SysDiagTaskProcessInfo)
Q_DECLARE_METATYPE(PublicData::PelItemData)
Q_DECLARE_METATYPE(PublicData::FaultTreeData)
Q_DECLARE_METATYPE(PublicData::FaultIndexData)
Q_DECLARE_METATYPE(PublicData::FaultIndexRefData)
Q_DECLARE_METATYPE(PublicData::ManualDiagTreeInfos)
Q_DECLARE_METATYPE(PublicData::BasicProcessTreeInfos)
Q_DECLARE_METATYPE(PublicData::ManualTaskDiagExcuteInfo)
Q_DECLARE_METATYPE(PublicData::ManualTaskDiagMsgID)
Q_DECLARE_METATYPE(PublicData::_EnvirnmentalFactorInfo)
Q_DECLARE_METATYPE(PublicData::_AnaylsisToolFilter)
Q_DECLARE_METATYPE(PublicData::_TrackingAccurayInfo)
Q_DECLARE_METATYPE(PublicData::_AntennaDirectInfo)
Q_DECLARE_METATYPE(PublicData::ThresholdGroupUserRef)

SCENE_SHARED_EXPORT QDebug operator<<(QDebug debug, const PublicData::Device& dev);

#endif // PUBLICDATA_H
