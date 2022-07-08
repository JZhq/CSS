/***************************************
 * CopyRight: sinux
 * Description: 工具类的接口类
 * Authors: zs
 * Date: 2018-04-27
 * History:no
 ***************************************/
#ifndef UTIL_H
#define UTIL_H

#include "util_global.h"

class UiUtil;
class UTILSHARED_EXPORT Util
{
public:
    static Util* getInstance()
    {
        static Util instance;
        return &instance;
    }

    //UiUtil* getUiUtil();

private:
    Util() {}
    ~Util() {}
    Util(const Util& rParam) = delete;
    const Util& operator = (const Util& rParam) = delete;

};

#endif // UTIL_H
