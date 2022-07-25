#ifndef CONFIGURATIONGLOBAL_H
#define CONFIGURATIONGLOBAL_H

#include "singleton.h"
#include <QString>
#include <json.h>

#define ConfigGlobalInterface Singleton<ConfigurationGlobal>::getInstance()
class ConfigurationGlobal
{
    SINGLETON(ConfigurationGlobal)
public:
    QString serverAddr() const;
    int serverPort() const;

private:
    Json* m_confJs;
};


namespace FrameWork{
    const QString Name = "name";
    const QString Version = "ver";
    const QString Sys_ver = "systemver";
    const QString Sys_name = "systemname";
    const QString Cpu_name = "cpuname";
    const QString Type = "type";
};

namespace Mudule{
    const QString Name = "modu_name";
    const QString Version = "ver";
    const QString Sys_ver = "systemver";
    const QString Sys_name = "systemname";
    const QString Cpu_name = "cpuname";
    const QString Type = "type";
};

namespace System{
    const QString Name = "systemname";
    const QString Version = "systemver";
    const QString Desc = "systemdesc";
};

namespace Cpu{
    const QString Name = "cpuname";
    const QString Desc = "cpudesc";
};
#endif // CONFIGURATIONGLOBAL_H
