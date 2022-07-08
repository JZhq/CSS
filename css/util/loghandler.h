/***************************************
 * CopyRight: sinux
 * Description: 实现日志记录功能
 * Authors: zs
 * Date: 2018-05-15
 * History:no
 ***************************************/
#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "util_global.h"

class LogHandlerPrivate;
class UTILSHARED_EXPORT LogHandler
{
public:
    //!
    //! \brief 获取此单例的实例
    //! \return
    //!
    static LogHandler *getInstance()
    {
        static LogHandler instance;
        return &instance;
    }

    //!
    //! \brief 释放资源
    //!
    void uninstallMessageHandler();

    //!
    //! \brief 给 Qt 安装消息处理函数
    //!
    void installMessageHandler();

private:
    LogHandler();
    ~LogHandler();
    LogHandler(const LogHandler& rParam) = delete;
    const LogHandler& operator = (const LogHandler& rParam) = delete;

private:
    LogHandlerPrivate *d;
};

#endif // LOGHANDLER_H
