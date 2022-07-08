#ifndef EVENT_INTERFACE
#define EVENT_INTERFACE


#include <QString>

namespace eventinterface {

//!
//! \brief *Handler 类必须实现的接口
//!
class EventImp {
    virtual void connectEventBus() = 0;
    virtual void disconnectEventBus()  = 0;
};

/*----------------------------------------------------------------------------|
 |                                主题                                        |
 |----------------------------------------------------------------------------*/
// 主界面
static const QString AllMonitorStateTopic = "allMonitorStateTopic";  // 所有的监视状态
static const QString SysTreeInfoTopic = "SysTreeInfoTopic";

//门限管理
static const QString ThresholdGroupAddTopic = "ThresholdGroupAddTopic";//增加监测点组名
static const QString ThresholdGroupModifyTopic = "ThresholdGroupModifyTopic";//修改监测点组名
static const QString ThresholdGroupDeleteTopic = "ThresholdGroupDeleteTopic";//删除监测点组名
static const QString ThresholdAddTopic = "ThresholdAddTopic";//增加门限
static const QString ThresholdDeleteTopic = "ThresholdDeleteTopic";//删除门限
static const QString ThresholdModifyTopic = "ThresholdModifyTopic";//修改门限
static const QString ThresholdFindByFilterTopic = "ThresholdFindByFilterTopic";//查找门限
static const QString ThresholdSaveTopic = "ThresholdSaveTopic";
static const QString ThresholdFindAllTopic ="ThresholdFindAllTopic";//查找所有门限
static const QString ThresholdGroupFindTopic ="ThresholdGroupFindTopic";//查找监测点组
//复杂监测点
static const QString ComplexThresholdFindTopic = "ComplexThresholdFindTopic";//查找复杂监测点
static const QString ComplexThresholdAddTopic = "ComplexThresholdAddTopic";//增加复杂监测点
static const QString ComplexThresholdDeleteTopic = "ComplexThresholdDeleteTopic";//删除复杂监测点
static const QString ComplexThresholdModifyTopic = "ComplexThresholdModifyTopic";//修改复杂监测点
//测试模板管理
static const QString TestmodeAddTopic ="TestmodeAddTopic";
static const QString TestmodeDeleteTopic ="TestmodeDeleteTopic";
static const QString TestmodeModifyTopic ="TestmodeModifyTopic";
static const QString TestmodeFindAllTopic ="TestmodeFindAllTopic";
static const QString TestmodeFindByNameTopic ="TestmodeFindByNameTopic";
static const QString TestmodeFindByTypeOrNameTopic ="TestmodeFindByTypeOrNameTopic";

//首页故障信息显示
static const QString FaultInfoFindTopic ="FaultInfoFindTopic";
static const QString BatchCloseloopFaultTopic = "BatchCloseloopFaultTopic";
static const QString FaultTreeIdsTopic ="FaultTreeIdsTopic";
static const QString FaultInfoFindByFilterTopic ="FaultInfoFindByFilterTopic";
static const QString FaultTreeFindTopic ="FaultTreeFindTopic";
static const QString DeviceStatusTopic ="DeviceStatusTopic";
static const QString ModeChangeTopic ="ModeChangeTopic";
static const QString LogInfoTopic ="LogInfoTopic";
static const QString DeviceAbnormalInfoTopic = "DeviceAbnormalInfoTopic";
static const QString DevicePictureShowTopic = "DevicePictureShowTopic";

//函数管理
static const QString FunctionAllTopic ="FunctionAllTopic";
static const QString FunctionAddTopic ="FunctionAddTopic";
static const QString FunctionFindByFilterTopic = "FunctionFindByFilterTopic";
static const QString FunctionModifyTopic ="FunctionModifyTopic";
static const QString FunctionDeleteTopic ="FunctionDeleteTopic";
static const QString FunctionDirTopic ="FunctionDirTopic";
static const QString FunctionChartPathTopic ="FunctionChartPathTopic";
static const QString FunctionChartDownloadTopic ="FunctionChartDownloadTopic";
static const QString FunctionChartDownloadCompleteTopic ="FunctionChartDownloadCompleteTopic";
//插件管理
static const QString PluginAddTopic ="PluginAddTopic";
static const QString PluginAllTopic ="PluginAllTopic";
static const QString PluginFindByFilterTopic = "PluginFindByFilterTopic";
static const QString PluginModifyTopic ="PluginModifyTopic";
static const QString PluginDeleteTopic ="PluginDeleteTopic";
static const QString PluginDirTopic ="PluginDirTopic";
static const QString PluginChartPathTopic ="PluginChartPathTopic";
static const QString PluginChartDownloadTopic ="PluginChartDownloadTopic";
static const QString PluginChartDownloadCompleteTopic ="PluginChartDownloadCompleteTopic";
static const QString highOrderFuncChangedTopic ="highOrderFuncChangedTopic";
// 用户管理
static const QString UserLoginTopic = "UserLoginTopic"; // 用户登陆
static const QString UserLogoutTopic = "UserLogoutTopic"; // 用户注销
static const QString UserAddTopic = "UserAddTopic"; // 添加用户
static const QString UserInfoTopic = "UserInfoTopic"; // 用户信息
static const QString UserDelTopic = "UserDelTopic"; // 用户删除
static const QString UserUpdateTopic = "UserUpdateTopic"; // 用户更新
static const QString UserPasswordReset = "UserPasswordReset"; // 密码重置
static const QString RangeUserInfoTopic = "RangeUserInfoTopic"; // 一个范围的用户数据信息
//故障树管理模块
const static QString JumpSysDiagTopic  = "JumpSysDiagTopic"; // 跳转到系统诊断
const static QString AddItemPropertyTopic    = "AddItemProperty"; //增加图元属性主题
const static QString DeleteTreeNodeTopic  = "DeleteTreeNode";//删除故障树节点
const static QString SelectItemTopic = "SelectItem"; //选中图元主题
const static QString ChangeSceneModeTopic = "ChangeSceneMode"; //改变场景的模式
const static QString ChangeSceneEditModeTic = "ChangeSceneEditMode";  // 改变场景编辑的模式
const static QString ClearEditViewTopic = "ClearEditView"; //清除编辑区的内容
const static QString FindAllFunctionTopic = "FindAllFunctionTopic"; //查找所有函数信息
const static QString FindFaultTreeByFilterTopic = "FindFaultTreeByFilterTopic"; //按条件查找故障树

const static QString UpdateFaultTreeTopic = "UpdateFaultTree"; //更新故障树属性
const static QString SaveAuditFaultTreeTopic = "SaveAuditFaultTree"; //更新故障树属性
const static QString DeleteFaultTreeTopic = "DeleteFaultTree"; //删除故障树
const static QString FaultTreeInfoTopic = "FaultTreeInfo";      //获取故障树属性
const static QString AllFaultTreeInfoTopic = "AllFaultTreeInfo"; //获取所有的故障树属性
const static QString ShowFaultTreePropertyTopic = "ShowFaultTreeProperty"; //展示当前选中的故障树属性
const static QString AllBasicTreeInfoTopic = "AllBasicTreeInfoTopic";   // 获取所有的基础知识树属性
const static QString IsRefBaseTreeTopic = "IsRefBaseTreeTopic";   // 获取所有的基础知识树属性

//const static QString SaveFaultTreeNodeTopic = "SaveFaultTreeNode";//保存故障树结构
const static QString SaveTreeNodeTopic = "SaveTreeNode";//保存故障树结构
//const static QString AddFaultTreeNodeTopic = "AddFaultTreeNode";//添加故障树节点
const static QString PitchFaultTreeTopic = "PitchFaultTree";//选中当前故障树
const static QString BatchExportTreesTopic = "BatchExportTreesTopic"; // 批量导出树

//获取展示用的节点数据
const static QString LoadDisplayNodesDataTopic = "LoadDisplayNodesData";

const static QString LoadFaultTreeNodesTopic = "LoadFaultTreeNodes";//加载当前故障树所有节点信息
const static QString SetNodesPropertyTopic = "SetNodesProperty";//设置故障树节点属性到绘图模块中
const static QString SetTreePropertyTopic = "SetTreeProperty"; //设置故障树属性到故障树模块中
const static QString LoadTestModeDataFromPropertyTopic = "LoadTestModeDataFromProperty";//加载测试模板数据到属性模块
const static QString LoadAllPluginInfoFromPropertyTopic = "LoadAllPluginInfoFromProperty";//加载所有的插件数据到属性模块
//属性模块
const static QString DrawItemNameTopic = "DrawItemName";//绘制输入的图元名称
const static QString DrawItemVariableErrorTopic= "DrawItemVariableError";//绘制输入的图元名称
const static QString SaveNodesPropertyTopic = "SaveNodesProperty";//保存故障树节点属性
//const static QString LoadNodesPropertyTopic = "LoadNodesProperty";//加载故障树节点属性

// 日志管理
const static QString LogDelTopic = "LogDelTopic"; // 日志删除
const static QString RangeLogInfoTopic = "RangeLogInfoTopic"; // 日志数据
const static QString logDownloadTopic = "logDownloadTopic";

// 任务管理
const static QString RangeRTasksTopic = "RangeRTasksTopic"; // 接收任务数据
const static QString RangeSysDiagTasksTopic = "RangeSysDiagTasksTopic"; // 系统诊断任务数据
const static QString RangeDiagTasksTopic = "RangeDiagTasksTopic"; // 诊断分析任务数据
const static QString AlDiagProcessTasksTopic = "AlDiagProcessTasksTopic"; // 分析诊断任务过程信息
const static QString SysDiagProcessTasksTopic = "SysDiagProcessTasksTopic"; // 系统诊断任务过程信息
const static QString ManuAlTaskTopic = "ManuAlTaskTopic"; // 手动任务分析
const static QString ManuTaskDiagTreeTopic = "ManuTaskDiagTreeTopic"; // 手动任务诊断
const static QString SysDiagWithTaskMngTopic = "SysDiagWithTaskMngTopic"; // 任务管理的系统诊断
const static QString SysDiagWithFaultTreeTopic = "SysDiagWithFaultTreeTopic"; // 故障树管理的系统诊断
const static QString ExamineFaultTreeTopic = "ExamineFaultTreeTopic";//审核故障树
const static QString ProcessTreesTopic = "ProcessTreesTopic"; // 过程数据的树
const static QString ReportDownloadTopic = "ReportDownload"; // 报告查看
const static QString ReportFindOutTopic = "ReportFindOutTopic"; // 报告查看
const static QString LogLookUpTopic = "LogLookUpTopic"; // 日志查看
const static QString DiagCancelTopic = "DiagCancel"; // 诊断取消
const static QString ManualTaskExcuteTopic = "ManualTaskExcuteTopic"; // 手动任务执行

const static QString CreateStandardResultTableTopic = "CreateStandardResultTableTopic";//创建标准结果表
const static QString GetStandardResultTableTopic = "GetStandardResultTableTopic";//获取标准结果表
const static QString SaveStandardResultTableTopic = "SaveStandardResultTableTopic";//保存标准结果表
const static QString SaveCustSystemTargetTableTopic = "SaveCustSystemTargetTableTopic";//保存自定义系统指标表
const static QString DeleteCustSystemTargetTableTopic = "DeleteCustSystemTargetTableTopic";//删除自定义系统指标表
const static QString StandardResultRepsonseTopic = "StandardResultRepsonseTopic";//处理表返回值
const static QString StandardResultTableDataTopic = "StandardResultTableDataTopic";//处理表返回值

//故障索引
static const QString FaultIndexUpdateTopic ="FaultIndexUpdateTopic";//批量更新故障索引
static const QString FaultIndexFindTopic ="FaultIndexFindTopic";//查找故障索引
static const QString FaultIndexFindByIDTopic ="FaultIndexFindByIDTopic";//根据id查找故障索引
static const QString FaultIndexAddTopic ="FaultIndexAddTopic";//增加故障索引
static const QString FaultIndexmodifyTopic ="FaultIndexmodifyTopic";//修改故障索引
static const QString FaultIndexDeleteTopic ="FaultIndexDeleteTopic";//删除故障索引
static const QString FaultIndexRefFindByIdTopic = "FaultIndexRefFindbyIdTopic";  // 索引表引用查询
static const QString FaultIndexRefInsertTopic = "FaultIndexRefInsertTopic";      // 索引表引用数据添加

// 分析工具
static const QString SetAnaylsisToolTopic ="SetAnaylsisToolTopic";
static const QString GetAnaylsisToolTopic ="GetAnaylsisToolTopic";
static const QString FindEnvFactorTopic   ="FindEnvFactorTopic";
static const QString FindTrackAccuTopic   = "FindTrackAccuTopic";
static const QString AntanenDirectAccuTopic   = "AntanenDirectAccuTopic";
static const QString Anaylsis_tool_DATA   = "Anaylsis_tool_DATA";

//评估模型
static const QString AssessTopic = "AssessTopic"; //评估与决策
static const QString AssessRepsonseTopic = "AssessRepsonseTopic"; //评估与决策返回值

//故障评估
static const QString FaultAssessTopic = "FaultAssessTopic"; //评估与决策
static const QString FaultAssessRepsonseTopic = "FaultAssessRepsonseTopic"; //评估与决策返回值

/*-----------------------------------------------------------------------------|
 |                                数据类型                                      |
 |----------------------------------------------------------------------------*/
/* 通用 */
static const QString OPERATE_METHOD = "OPERATE_METHOD"; // 请求/响应
static const QString OPERATE_TYPE = "OPERATE_TYPE"; // 操作方式
static const QString NODE_PROPERTY_TYPE = "NODE_PROPERTY_TYPE"; // 节点属性类型
static const QString LOADNODES_DATA_USE = "LOADNODES_DATA_USE";//加载节点数据的用途

/* 主界面 */
static const QString MONITITOR_TYPE = "MONITITOR_TYPE"; // 监视类型
static const QString STATE_VALUE = "STATE_VALUE"; // 监视状态
static const QString DEVICE_STATUS ="DEVICE_STATUS";//设备状态
static const QString LOG_TYPE ="LOG_TYPE"; //日志类别
static const QString LOG_CONTENT ="LOG_CONTENT";//日志内容
static const QString TIME ="TIME";//日志时间
static const QString DEVICE_ID="DEVICE_ID";
static const QString DEVICE_NAME="DEVICE_NAME";
static const QString MONITOR_ID="MONITOR_ID";
static const QString MONITOR_NAME="MONITOR_NAME";
static const QString FAULT_TIME="FAULT_TIME";
static const QString FAULT_REASON="FAULT_REASON";
static const QString FAULT_SOURCE= "FAULT_SOURCE";
static const QString PICTURE="PICTURE";
static const QString CHANGE_MODE="CHANGE_MODE";
static const QString ALL_SYS_TREEID = "ALL_SYS_TREEID"; // 系统诊断树的ID

/* 用户管理 */
static const QString USER_ID = "USER_ID"; // 用户ID
static const QString USER_ACCOUNT = "USER_ACCOUNT"; // 用户账号
static const QString USER_PWD = "USER_PWD"; // 用户密码
static const QString RESPOND_MSG = "RESPOND_MSG"; // 响应文本消息
static const QString RESPOND_ID = "RESPOND_ID"; // 响应文本消息ID
static const QString USER_INFO = "USER_INFO"; // 用户数据
static const QString USER_LIST = "USER_LIST"; // 一批的用户数据
static const QString USER_INDEX = "USER_INDEX"; // 用户表起始索引
static const QString USER_OFFSET = "USER_OFFSET"; // 用户表偏移量
static const QString USER_FILTER = "USER_FILTER"; // 用户筛选条件
static const QString USER_COUNT = "USER_COUNT"; //  用户数量
static const QString RANGE_USDER_INFO = "RANGE_USDER_INFO"; // 范围的用户数据

/* 故障树管理 */
static const QString FAULTTREE_ID = "FAULTTREE_ID"; // 故障树ID
static const QString FAULTTREE_STATUS = "FAULTTREE_STATUS"; // 故障树状态
static const QString FAULTTREE_TYPE = "FAULTTREE_TYPE"; // 故障树类型
static const QString FAULTTREE_NAME = "FAULTTREE_NAME"; // 故障树名称
static const QString FAULTTREE_INFO = "FAULTTREE_INFO"; // 故障树属性数据
static const QString FAULTTREE_LIST = "FAULTTREE_LIST"; // 一批的故障树属性数据
static const QString FAULTTREE_NODE = "FAULTTREE_NODE"; // 故障树节点结构数据
static const QString FAULTTREE_NODE_ID = "FAULTTREE_NODE_ID"; // 故障树节点id
static const QString FAULTTREE_NODE_NAME = "FAULTTREE_NODE_NAME";// 故障树节点的名称
static const QString FAULTTREE_NODE_PROPERTY = "FAULTTREE_NODE_PROPERTY"; // 节点属性
static const QString FAULTTREE_SYSDIAG_STARTTIME = "FAULTTREE_SYSDIAG_STARTTIME";//系统诊断开始时间
static const QString FAULTTREE_SYSDIAG_ENDTIME = "FAULTTREE_SYSDIAG_ENDTIME";//系统诊断结束时间
static const QString FAULTTREE_NODE_VARNAME = "FAULTTREE_NODE_VARNAME";//节点的变量名
static const QString FAULTTREE_NODE_NAME_TYPE = "FAULTTREE_NODE_NAME_TYPE";//绘制图元的名称的类型（名称、变量名）
static const QString FAULTTREE_PARAM_TIME = "FAULTTREE_PARAM_TIME"; // 参数节点的取值时间
static const QString FAULTTREE_DATA_START_TIME = "FAULTTREE_DATA_START_TIME"; // 系统诊断开始取值时间
static const QString FAULTTREE_DATA_END_TIME = "FAULTTREE_DATA_END_TIME"; // 系统诊断结束取值时间

/* 日志管理 */
static const QString LOG_INDEX = "LOG_INDEX"; // 日志表起始索引
static const QString LOG_OFFSET = "LOG_OFFSET"; // 日志表偏移量
static const QString LOG_IDLIST = "LOG_IDLIST"; // 多条日志ID
static const QString LOG_FILTER = "LOG_FILTER"; // 日志筛选条件
static const QString LOG_COUNT = "LOG_COUNT"; // 日志数量
static const QString LOG_LIST = "LOG_LIST"; // 日志列表
static const QString LOG_START_DATE = "LOG_START_DATE"; // 日志下载开始时间
static const QString LOG_END_DATE = "LOG_END_DATE"; // 日志下载结束时间

/* 任务管理 */
static const QString RTASK_INDEX  = "RTASK_INDEX"; // 接收任务表起始索引
static const QString RTASK_OFFSET = "RTASK_OFFSET"; // 接收任务表偏移量
static const QString RTASK_FILTER = "RTASK_FILTER"; // 接收任务筛选条件
static const QString RTASK_COUNT  = "RTASK_COUNT"; // 接收任务数量
static const QString RTASK_LIST   = "RTASK_LIST"; // 接收任务列表
static const QString DIAGTASK_INDEX  = "DIAGTASK_INDEX";  // 诊断任务表起始索引
static const QString DIAGTASK_OFFSET = "DIAGTASK_OFFSET"; // 诊断任务表偏移量
static const QString DIAGTASK_FILTER = "DIAGTASK_FILTER"; // 诊断任务筛选条件
static const QString DIAGTASK_COUNT  = "DIAGTASK_COUNT";  // 诊断任务数量
static const QString DIAGTASK_LIST   = "DIAGTASK_LIST";   // 诊断任务列表
static const QString ALDIAG_ALID = "ALDIAG_ALID"; // 分析任务ID
static const QString SYSDIAG_SYSID = "SYSDIAG_SYSID"; // 系统诊断任务ID
static const QString ALDIAG_PROCESS_LIST = "ALDIAG_PROCESS_LIST"; // 分析诊断任务过程信息列表
static const QString SYSDIAG_PROCESS_LIST = "SYSDIAG_PROCESS_LIST"; // 系统诊断任务过程信息列表
static const QString RTASK_TS = "RTASK_TS";    // TS
static const QString RTASK_JOB = "RTASK_JOB"; // JOB
static const QString TASK_ALID = "TASK_ALID"; // 诊断任务ID
static const QString DIAG_START_TIME = "DIAG_START_TIME"; // 编排起始时间
static const QString DIAG_END_TIME = "DIAG_END_TIME"; // 编排结束时间
static const QString DIAG_MSG_UUID = "DIAG_MSG_UUID"; // 故障树诊断消息ID
static const QString SYS_DIAG_TREE_ID = "SYS_DIAG_TREE_ID"; // 系统诊断树ID
static const QString TREE_BASICINFO_LIST = "TREE_BASICINFO_LIST"; // 树基本信息
static const QString FLAG_TYPE = "FLAG_TYPE"; // 报告类型
static const QString REPORT_CONTENT = "REPORT_CONTENT"; // 报告内容
static const QString DIAG_TYPE = "DIAG_TYPE"; // 诊断类型
static const QString CANCEL_MSG = "CANCEL_MSG"; // 取消的消息
static const QString MANUAL_DIAG_TREES = "MANUAL_DIAG_TREES"; // 手动的任务诊断树
static const QString MANUAL_DIAG_EXCUTE = "MANUAL_DIAG_EXCUTE"; // 手动的任务执行
static const QString TASK_ID = "TASK_ID"; // 任务ID
static const QString TASK_LOG = "TASK_LOG"; // 任务日志
static const QString SYS_TREE_INFO = "SYS_TREE_INFO"; // 系统诊断树信息
static const QString DEV_NAME = "DEV_NAME"; // 系统诊断的设备名

/* 复杂监测点 */
static const QString COMPLEX_THRESHOLD_ID = "COMPLEX_THRESHOLD_ID";
static const QString COMPLEX_THRESHOLD_INFO ="COMPLEX_THRESHOLD_INFO";
static const QString COMPLEX_THRESHOLD_LIST ="COMPLEX_THRESHOLD_LIST";
static const QString COMPLEX_THRESHOLD_COUNT ="COMPLEX_THRESHOLD_COUNT";
//add by led
/* 门限管理 */
static const QString THRESHOLD_ID = "THRESHOLD_ID";
static const QString THRESHOLD_MONITOR_POINTS_ID = "THRESHOLD_MONITOR_POINTS_ID";
static const QString THRESHOLD_MONITOR_POINTS_NAME = "THRESHOLD_MONITOR_POINTS_NAME";
static const QString THRESHOLD_DEVICE_ID = "THRESHOLD_DEVICE_ID";
static const QString THRESHOLD_DEVICE_NAME = "THRESHOLD_DEVICE_NAME";
static const QString THRESHOLD_DEVICE_TYPE = "THRESHOLD_DEVICE_TYPE";
static const QString THRESHOLD_MAX_VALUE ="THRESHOLD_MAX_VALUE";
static const QString THRESHOLD_MIN_VALUE = "THRESHOLD_MIN_VALUE";
static const QString THRESHOLD_ENMU_VALUE ="THRESHOLD_ENMU_VALUE";
static const QString THRESHOLD_ADVICE ="THRESHOLD_ADVICE";
static const QString THRESHOLD_DESCRIPE ="THRESHOLD_DESCRIPE";
static const QString THRESHOLD_CREATED_TIME = "THRESHOLD_CREATED_TIME";
static const QString THRESHOLD_COMPARE = "THRESHOLD_COMAPRE";
static const QString THRESHOLD_VALUE = "THRESHOLD_VALUE";
static const QString THRESHOLD_FAULT_REASON = "THRESHOLD_FAULT_REASON";
static const QString THRESHOLD_INFO ="THRESHOLD_INFO";
static const QString THRESHOLD_LIST ="THRESHOLD_LIST";
static const QString THRESHOLD_COUNT ="THRESHOLD_COUNT";
static const QString THRESHOLD_GROUP_INFO ="THRESHOLD_GROUP_INFO";
static const QString THRESHOLD_GROUP_NAME ="THRESHOLD_GROUP_NAME";
static const QString THRESHOLD_GROUP_COUNT ="THRESHOLD_GROUP_COUNT";
static const QString THRESHOLD_GROUP_LIST ="THRESHOLD_GROUP_LIST";
static const QString THRESHOLD_GROUP_SHOW ="THRESHOLD_GROUP_SHOW";
static const QString THRESHOLD_GROUP_ID ="THRESHOLD_GROUP_ID";
static const QString IS_COMPLEX_SUB ="IS_COMPLEX_SUB";
static const QString THRESHOLD_GROUP_USER_REF ="THRESHOLD_GROUP_USER_REF";
static const QString THRESHOLD_GROUP_USER_FILTER ="THRESHOLD_GROUP_USER_FILTER";
/* 测试模板管理 */
static const QString TESTMODE_ID ="TESTMODE_ID";
static const QString TESTMODE_REPLY ="TESTMODE_REPLY";
static const QString TESTMODE_TEST_TYPE ="TESTMODE_TEST_TYPE";
static const QString TESTMODE_TEST_NAME ="TESTMODE_TEST_NAME";
static const QString TESTMODE_PARAMS_JSON ="TESTMODE_TEST_PARAMS_JSON";
static const QString TESTMODE_COUNT = "TESTMODE_COUNT";
static const QString TESTMODE_LIST ="TESTMODE_LIST";
static const QString TESTMODE_INFO ="TESTMODE_INFO";

/* 函数管理 */
static const QString FUNCTION_ID ="FUNCTION_ID";
static const QString FUNCTION_DIR ="FUNCTION_DIR";
static const QString FUNCTION_CHART_PATH ="FUNCTION_CHART_PATH";
static const QString FUNCTION_CHART_NAME ="FUNCTION_CHART_NAME";
static const QString FUNCTION_LIST ="FUNCTION_LIST";
static const QString FUNCTION_INFO ="FUNCTION_INFO";
static const QString FUNCTION_COUNT ="FUNCTION_COUNT";
static const QString FUNCTION_NAME ="FUNCTION_NAME";
static const QString FUNCTION_TYPE ="FUNCTION_TYPE";
static const QString FUNCTION_CREATED_TIME ="FUNCTION_CREATED_TIME";
static const QString FUNCTION_START_TIME ="FUNCTION_START_TIME";
static const QString FUNCTION_END_TIME ="FUNCTION_END_TIME";

/* 故障信息管理 */
static const QString FAULT_INFO = "FAULT_INFO";
static const QString FAULT_INFO_COUNT ="FAULT_INFO_COUNT";
static const QString FAULT_INFO_LIST ="FAULT_INFO_LIST";
static const QString FAULT_DEVIEC_ID="FAULT_DEVIEC_ID";
static const QString FAULT_DEVIEC_NAME="FAULT_DEVIEC_NAME";
static const QString FAULT_MONITOR_NAME ="FAULT_MONITOR_NAME";
static const QString FAULT_START_TIME = "FAULT_START_TIME";
static const QString FAULT_END_TIME ="FAULT_END_TIME";
static const QString FAULT_TREE_IDS ="FAULT_TREE_IDS";
static const QString FAULT_INDEX = "FAULT_INDEX";
static const QString FAULT_OFFSET = "FAULT_OFFSET";
static const QString CLOSEDLOOP_STATE = "CLOSEDLOOP_STATE";
/* 插件管理 */
static const QString PLUGIN_ID ="PLUGIN_ID";
static const QString PLUGIN_DIR ="PLUGIN_DIR";
static const QString PLUGIN_CHART_PATH ="PLUGIN_CHART_PATH";
static const QString PLUGIN_CHART_NAME ="PLUGIN_CHART_NAME";
static const QString PLUGIN_LIST ="PLUGIN_LIST";
static const QString PLUGIN_INFO ="PLUGIN_INFO";
static const QString PLUGIN_COUNT ="PLUGIN_COUNT";
static const QString PLUGIN_NAME ="PLUGIN_NAME";
static const QString PLUGIN_TYPE ="PLUGIN_TYPE";
static const QString PLUGIN_CREATED_TIME ="PLUGIN_CREATED_TIME";
static const QString PLUGIN_START_TIME ="PLUGIN_START_TIME";
static const QString PLUGIN_END_TIME ="PLUGIN_END_TIME";

static const QString STANDARD_TABLE_TYPE = "STANDARD_CREATE_TYPE";
static const QString STANDARD_TABLE_NAME = "STANDARD_TABLE_NAME";
static const QString STANDARD_TABLE_HEADLIST = "STANDARD_TABLE_HEADLIST";
static const QString STANDARD_TABLE_DATALIST = "STANDARD_TABLE_DATALIST";
static const QString STANDARD_RESULT = "STANDARD_RESULT";
static const QString STANDARD_ERROR_STR = "STANDARD_ERROR_STR";

/* 故障索引 */
static const QString FAULT_INDEX_DATA = "FAULT_INDEX_DATA";
static const QString FAULT_INDEX_ID = "FAULT_INDEX_ID";
static const QString FAULT_INDEX_DATA_LIST = "FAULT_INDEX_DATA_LIST";
static const QString FAULT_REF_ID = "FAULT_REF_ID";
static const QString FAULT_INDEX_REF_DATA = "FAULT_INDEX_REF_DATA";
static const QString FAULT_ID = "FAULT_ID";
static const QString FAULT_ID_DATA = "FAULT_ID_DATA";
static const QString TASK_SERIAL_NUMBER = "TASK_SERIAL_NUMBER";
static const QString TASK_SERIAL_NUMBER_DATA = "TASK_SERIAL_NUMBER_DATA";
static const QString JOB_TASK_ID = "JOB_TASK_ID";
static const QString JOB_TASK_ID_DATA = "JOB_TASK_ID_DATA";

/* 评估模型 */
namespace assess_model {
    static const QString MAIN_MANAGE_TYPE = "MAIN_MANAGE_TYPE";
    static const QString SUB_MANAGE_TYPE = "SUB_MANAGE_TYPE";
    static const QString DATA = "DATA";
    static const QString ERRCODE = "ERRCODE";
    static const QString ERRSTR = "ERRSTR";
}

/* 故障评估 */
namespace fault_assess {
    static const QString MAIN_MANAGE_TYPE = "MAIN_MANAGE_TYPE";
    static const QString SUB_MANAGE_TYPE = "SUB_MANAGE_TYPE";
    static const QString DATA = "DATA";
    static const QString ERRCODE = "ERRCODE";
    static const QString ERRSTR = "ERRSTR";
}

/*-----------------------------------------------------------------------------|
 |                                数据内容                                      |
 |----------------------------------------------------------------------------*/
// C/S通信时请求还是响应数据
enum Operate
{
    Requset,
    Respond
};

//所加载的节点数据的用途
enum NodesDataUse
{
    EDIT,
    DISPLAY,
    DRAGSHOW
};

enum StandardTableType{
    GT_Table,
    Transmission_Table,
    LinkerPer_Table,
    ServerPer_Table,
    TelemetryError_Table,
    RemoteComp_Table,
    DirectSensit_Table,
    TrackAccuracy_Table,
    PointState_Table,
    EbNo_Table,
    SystemTarget_Table,
    SystemTarget_Tables
};

enum AssessModelManageType {
    EAMM_None = 0,
    EAMM_DeviceUnit,
    EAMM_ProtoModel,
    EAMM_AssessModel,
    EAMM_FaultAssessManage,
};

enum DeviceUnitManageType {
    EDUM_None = 0,
    EDUM_Update,
    EDUM_Query,
};

enum DeviceCategoryManageType {
    EDCM_None = 0,
    EDCM_UpdateCategotyGroup,
    EDCM_RemoveCategotyGroup,
    EDCM_UpdateCategoty,
    EDCM_RenameCategoty,
    EDCM_SetCategotySubsystem,
    EDCM_RemoveCategoty,
    EDCM_MoveCategoty,
    EDCM_QueryCategotyTree,
    EDCM_QueryCategotyInfo,
    EDCM_QueryFaultIndexInfo,
};

enum AssessCapabilityRequirementType {
    EACRT_None = 0,
    EACRT_UpdateInfo,
    EACRT_RemoveInfo,
    EACRT_QueryGeneralList,
    EACRT_QueryDetailInfo,
};

enum FaultAssessManageType {
    EFAMT_None = 0,
    EFAMT_QueryAllSatellite,
    EFAMT_QueryAllAntenna,
    EFAMT_UpdateFaultAssessmentModelInfo,
    EFAMT_QueryAllFaultAssessmentModelIds,
    EFAMT_QueryFaultAssessmentModelInfo,
    EFAMT_RemoveFaultAssessmentModelInfo,
    EFAMT_QueryActivedFaultInfos,
    EFAMT_QueryFaultInfos,
    EFAMT_QueryFaultRefInfos,
    EFAMT_UpdateFaultrAssessExecTime,
    EFAMT_UpdateFaultAssociateInfos,
    EFAMT_QueryFaultAssessConfigRsltLastestInfo,
    EFAMT_QueryFaultAssessConfigRsltInfos,
    EFAMT_GetNewTrendAnalysisTaskId,
    EFAMT_AddTrendAnalysisTask,
    EFAMT_RemoveTrendAnalusisTask,
    EFAMT_UpdateTrendAnalusisTaskRslt,
    EFAMT_QueryTrendAnalusisTaskRslt,
    EFAMT_QueryAllTrendAnalysisTaskIds,
    EFAMT_QueryTrendAnalusisTask,
};
}

#endif
