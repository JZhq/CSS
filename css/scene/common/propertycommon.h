#ifndef PROPERTYCOMMON_H
#define PROPERTYCOMMON_H

#include <QString>
#include <QObject>


//属性类型
namespace PrimitivePropertyConstants {

//----------------------------------------------
static const QString INPUT = "输入值";//输入
static const QString OUTPUT = QStringLiteral("输出值");//输出
static const QString ITEMNAME = QStringLiteral("图元名称");//图元名称
static const QString ITEMID   = QStringLiteral("图元标识");//图元id
static const QString VARNAME  = QStringLiteral("变量名");//变量名
static const QString ITEMTYPE = "itemType";//图元类型
static const QString ITEMDATA = "itemData";//图元数据域
static const QString ITEMDATAFRIEND = "itemDatafriend";//图元数据域
//----------------------------------------------

} // end namespace PrimitivePropertyConstants

#endif // PROPERTYCOMMON_H

