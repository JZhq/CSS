#include "configurationglobal.h"

#include <QApplication>

ConfigurationGlobal::ConfigurationGlobal():
    m_confJs(NULL)
{
    m_confJs = new Json(QApplication::applicationDirPath() + "/data/config.json", true);
}


ConfigurationGlobal::~ConfigurationGlobal()
{

}

QString ConfigurationGlobal::serverAddr() const
{
    return m_confJs->getString("server.host");
}

int ConfigurationGlobal::serverPort() const
{
    return m_confJs->getString("server.port").toInt();
}
