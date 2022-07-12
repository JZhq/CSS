#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>


//所有字符编码采用UTF-8
class HttpClient : public QObject
{
    Q_OBJECT
public:
    //const char *slot 参数为bool, QString
    HttpClient(const QString &ip, int port, QObject *reciver, const char *slot, QObject *parent = nullptr);

    //用户登录
    //示例：localhost::8080/usermanager/login?user=xxx&password=xxxx
    void userLogin(const QString &user, const QString &password);


//    操作系统信息录入接口（采用get方式）
//    localhost::8080/opsystemconfig/add?systemname=xxx&systemver=xxxx&systemdesc=xxx
    void opSystemConfigAdd(const QString &systemname, const QString &systemver, const QString &systemdesc);


//    操作系统信息删除接口（采用get方式）
//    localhost::8080/opsystemconfig/delete?systemname=xxx&systemver=xxxx
    void opSystemConfigDelete(const QString &systemname, const QString &systemver);

//    操作系统信息更新接口（采用get方式）
//    localhost::8080/opsystemconfig/update?oldsystemname=xxx&oldsystemver=xxx &systemname=xxx&systemver=xxxx&systemdesc=xxx
    void opSystemConfigUpdate(const QString &oldsystemname, const QString &oldsystemver,
                              const QString &systemname, const QString &systemver, const QString &systemdesc);

//    操作系统信息查询接口（采用get方式）
//    localhost::8080/opsystemconfig/query
    void opSystemConfigQueryList();

//    CPU信息录入接口（采用get方式）
//    localhost::8080/cpuconfig/add?cpuname=xxx&cpudesc=xxx
    void cpuConfigAdd(const QString &cpuname, const QString cpudesc);


//    CPU信息删除接口（采用get方式）
//    localhost::8080/cpuconfig/delete?cpuname=xxx
    void cpuConfigDelete(const QString &cpuname);


//    CPU信息更新接口（采用get方式）
//    localhost::8080/cpuconfig/update?oldcpuname=xxx&cpuname=xxx&cpudesc=xxx
    void cpuConfigUpdate(const QString &oldcpuname, const QString &cpuname, const QString &cpudesc);

//    CPU信息查询接口（采用get方式）
//    localhost::8080/cpuconfig/query
    void cpuConfigQueryList();

//    框架信息上传接口（采用post）
//    localhost::8080/frameworkinfo/add
//    需要将上传的文件或者文件夹进行zip压缩，然后在上传
//    附加参数包括以下内容：
//    name:框架名称
//    ver:框架版本
//    systemname:支持的操作系统名称
//    systemver:支持的操作系统版本
//    cpuname:支持的cpu名称
//    type:上传的资源是源码还是动态库（0：为源码  1：为动态库）
    void frameworkInfoAdd(const QString &name, const QString &ver, const QString &systemname
                          , const QString &systemver, const QString &cpuname, int type, const QString &fileName);


//    查询所有框架信息（采用get）
//    localhost::8080/frameworkinfo/querylist
    void frameworkInfoQueryList();

//    框架信息删除接口（采用get）
//    localhost::8080/frameworkinfo/delete?name=xxx&ver=xxx
    void frameworkInfoDelete(const QString &name, const QString &ver);



//    框架信息更新接口（采用post）
//    localhost::8080/frameworkinfo/update
//    需要将上传的文件或者文件夹进行zip压缩，然后在上传
//    附加参数包括以下内容：
//    oldname:更新前的框架名称
//    oldver:更新前的框架版本
//    name:框架名称
//    ver:框架版本
//    systemname:支持的操作系统名称
//    systemver:支持的操作系统版本
//    cpuname:支持的cpu名称
//    type:上传的资源是源码还是动态库（0：为源码  1：为动态库）
    //fileName为空是只更新基础信息不更新文件包
    void frameworkInfoUpdate(const QString &oldname, const QString &oldver, const QString &name, const QString &ver, const QString &systemname
                          , const QString &systemver, const QString &cpuname, int type, const QString &fileName = QString());


//    框架压缩包下载接口（采用Get）
//    localhost::8080/frameworkinfo/download?filename=xxxx
//    filename:下载的文件名（查询信息中会包括框架压缩包文件名）
    //下载完成的文件会存放至saveFilePath
    //最好在线程中调用
    void frameworkInfoDownload(const QString &fileName, const QString &saveFilePath);



//    模块信息上传接口（采用post）
//    localhost::8080/moduleinfo/add
//    需要将上传的文件或者文件夹进行zip压缩，然后在上传
//    附加参数包括以下内容：
//    name:模块名称
//    ver:模块版本
//    systemname:支持的操作系统名称
//    systemver:支持的操作系统版本
//    cpuname:支持的cpu名称
//    type:上传的资源是源码还是动态库（0：为源码  1：为动态库）
    void moduleInfoAdd(const QString &name, const QString &ver, const QString &systemname
                          , const QString &systemver, const QString &cpuname, int type, const QString &fileName);


    //    框架压缩包下载接口（采用Get）
    //    localhost::8080/moduleinfo/download?filename=xxxx
    //    filename:下载的文件名（查询信息中会包括模块压缩包文件名）
        //下载完成的文件会存放至saveFilePath
        //最好在线程中调用
    void moduleInfoDownload(const QString &fileName, const QString &saveFilePath);

//    查询所有模块信息（采用get）
//    localhost::8080/moduleinfo/querylist
    void moduleInfoQueryList();


//    模块信息删除接口（采用get）
//    localhost::8080/moduleinfo/delete?name=xxx&ver=xxx
//    name:模块名称
//    ver:模块版本
    void moduleInfoDelete(const QString &name, const QString &ver);


//    模块信息更新接口（采用post）
//    localhost::8080/moduleinfo/update
//    需要将上传的文件或者文件夹进行zip压缩，然后在上传
//    附加参数包括以下内容：
//    oldname:更新前的模块名称
//    oldver:更新前的模块版本
//    name:模块名称
//    ver:模块版本
//    systemname:支持的操作系统名称
//    systemver:支持的操作系统版本
//    cpuname:支持的cpu名称
    void moduleInfoUpdate(const QString &oldname, const QString &oldver, const QString &name, const QString &ver, const QString &systemname
                          , const QString &systemver, const QString &cpuname, int type, const QString &fileName = QString());


    //工程信息创建接口（get）
//    localhost::8080/project/add?projectname=xxx&value=xxx
    void addProject(const QString &projectname, const QString &value);

    //工程信息更新接口（get）
    //    localhost::8080/project/update?oldprojectname=xxx&projectname=xxx&value=xxx
    void updateProject(const QString &oldprojectname, const QString &projectname, const QString &value);



    //工程信息删除接口（get）
    //    localhost::8080/project/delete?projectname=xxx
    void deleteProject(const QString &projectname);

    //工程信息列表查询接口（get）
    //    localhost::8080/project/query
    void queryProjectList();



signals:
    void emitData(bool flag, const QString &data);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    QString m_ip;
    int m_port;
    const char *m_slot;
    QSharedPointer<QNetworkAccessManager> m_manager;
};

#endif // HTTPCLIENT_H
