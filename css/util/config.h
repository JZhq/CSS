#ifndef CONFIG_H
#define CONFIG_H

#include "util_global.h"

class QString;
class QStringList;
class Json;
class QJsonValue;

/**
 * 用于读写配置文件:
 * 1. 配置文件位于: data/config.json，存储配置的信息，例如数据库信息，QSS 文件的路径
 * 2. 读取配置，如 Singleton<Config>::getInstance().getDatabaseName();
 */
class UTILSHARED_EXPORT Config {

public:
    static Config* getInstance()
    {
        static Config instance;
        return &instance;
    }

    //!
    //! \brief 销毁 Config 的资源，如有必要，在 main 函数结束前调用，例如保存配置文件
    //!
    void destroy();
// 读
public:

    // 系统配置信息
    QStringList getZmqTopics() const;
    int getZmqTransPort() const;
    int getZmqSubPort() const;
    QString getImsUrl() const;                  // 获取信息管理系统 URL 地址
    QString getServerIPAddress() const;         // 获取服务端 IP 地址
    int     getServerPort() const;              // 获取端口号
    QString getLogDownloadDir() const;          // 获取日志下载路径
    int     getTableRows() const;               // 获取分页表行数
    int     getRollTime() const;                // 获取轮询时间
    int     getUdpOverTimeTime() const;         // 获取udp超时时间,单位ms,1000ms = 1s
    int     getPreTime() const;                 // 上一个任务开始时间间隔
    int     getNextTime() const;                // 下一个任务开始时间间隔
    int     getRecommondPredictTime() const;    // 推荐的预计测试时间

    // 登陆界面使用的信息
    QString getLastLoginAccount() const;
    QString getLastLoginPwd() const;
    bool    getIsSavePassword() const;

    // 其它
    QStringList getQssFiles() const;            // QSS 样式表文件, 可以是多个
    QString getPointsDir() const;
    QString getDevicesFileName() const;

// 写
public:
    void setServerIPAddress(const QString &value); // 设置服务器 IP 地址
    void setServerPort(int value);                 // 设置服务器端口号
    void setLogDownloadDir(const QString &value);  // 设置日志下载路径
    void setTableRows(int value);                  // 设置表行数
    void setUdpOverTimeTime(int value);            //设置udp超时时间,单位ms，1000ms = 1s
    void setLastLoginAccount(const QString& account);
    void setLastLoginPwd(const QString& pwd);
    void setIsSavePassword(bool isSave);
    void setPreTime(int secs);                 // 上一个任务开始时间间隔
    void setNextTime(int secs);                // 下一个任务开始时间间隔
    void setRecommondPredictTime(int minutes); // 推荐的预计测试时间
private:
    Config();
    ~Config();
    Config(const Config& rParam) = delete;
    const Config& operator = (const Config& rParam) = delete;
    Json *json;
};

#endif // CONFIG_H
