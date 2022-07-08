#include "Config.h"
#include "Json.h"

#include <QString>
#include <QStringList>
#include <QApplication>

Config::Config() {
    json = new Json(qApp->applicationDirPath() + "/data/config.json", true); // 配置文件路径
}

Config::~Config() {
    destroy();
}

void Config::destroy() {
    delete json;
    json = Q_NULLPTR;
}

QStringList Config::getZmqTopics() const
{
    return json->getStringList("system_config.server.topics");
}

int Config::getZmqTransPort() const
{
    return json->getInt("system_config.server.zmq_trans_port");
}

int Config::getZmqSubPort() const
{
    return json->getInt("system_config.server.zmq_sub_port");
}

QString Config::getImsUrl() const
{
    return json->getString("ims_url");
}

QString Config::getServerIPAddress() const
{
    return json->getString("system_config.server.ip", "127.0.0.1");
}

int Config::getServerPort() const
{
    return json->getInt("system_config.server.port", 0);
}

QString Config::getLogDownloadDir() const
{
    return json->getString("system_config.download.log_dir", "C://");
}

int Config::getTableRows() const
{
    return json->getInt("system_config.basic.table_rows", 50);
}

int Config::getRollTime() const
{
    return json->getInt("system_config.basic.roll_time", 3000);
}

int Config::getUdpOverTimeTime() const
{
    return json->getInt("system_config.udpTimeOut");
}

QString Config::getLastLoginAccount() const
{
    return json->getString("save_login.last_account");
}

QString Config::getLastLoginPwd() const
{
    return json->getString("save_login.last_pwd");
}

bool Config::getIsSavePassword() const
{
    return json->getBool("save_login.save");
}

QStringList Config::getQssFiles() const
{
    return json->getStringList("qss_files");
}

QString Config::getPointsDir() const
{
    return json->getString("point_dir");
}

QString Config::getDevicesFileName() const
{
    return json->getString("devices_path");
}

int Config::getPreTime() const
{
    return json->getInt("system_config.server.pre_time");
}

int Config::getNextTime() const
{
    return json->getInt("system_config.server.next_time");
}

int Config::getRecommondPredictTime() const
{
    return json->getInt("system_config.server.recommond_predict_minutes");
}

void Config::setServerIPAddress(const QString &value)
{
    json->set("system_config.server.ip", QJsonValue(value));
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setServerPort(int value)
{
    json->set("system_config.server.port", QJsonValue(value));
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setLogDownloadDir(const QString &value) {
    json->set("system_config.download.log_dir", QJsonValue(value));
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setTableRows(int value)
{
    json->set("system_config.basic.table_rows", QJsonValue(value));
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setUdpOverTimeTime(int value)
{
    json->set("system_config.udpTimeOut", QJsonValue(value));
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setLastLoginAccount(const QString& account)
{
    json->set("save_login.last_account", account);
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setLastLoginPwd(const QString& pwd)
{
    json->set("save_login.last_pwd", pwd);
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setIsSavePassword(bool isSave)
{
    json->set("save_login.save", isSave);
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setPreTime(int secs)
{
    json->set("system_config.server.pre_time", secs);
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setNextTime(int secs)
{
    json->set("system_config.server.next_time", secs);
    json->save(qApp->applicationDirPath() + "/data/config.json");
}

void Config::setRecommondPredictTime(int minutes)
{
    json->set("system_config.server.recommond_predict_minutes", minutes);
    json->save(qApp->applicationDirPath() + "/data/config.json");
}
