#ifndef ANIMATIONSTRUCT
#define ANIMATIONSTRUCT

#include <QColor>

enum FlashObject
{
    FObj_None           = 0X00,
    FObj_Background     = 0X01,
    FObj_Border         = 0X02,
    FObj_Foreground     = 0X04,
    FObj_All            = FObj_None | FObj_Background | FObj_Border | FObj_Foreground,
};

// 闪烁属性
typedef struct FlashParam
{
    FlashObject flashObj;

    bool bFlashBack;
    bool bFlashBorder;
    bool bFlashFore;

    //
    bool bFlashBackEnable;
    QColor colorBackSilent;
    QColor colorBackFlash;
    //
    bool bFlashBorderEnable;
    QColor colorBorderSilent;
    QColor colorBorderFlash;
    //
    bool bFlashForeEnable;
    QColor colorForeSilent;
    QColor colorForeFlash;

    FlashParam() : flashObj(FObj_None),
        bFlashBack(false), bFlashBorder(false), bFlashFore(false),
        bFlashBackEnable(false), colorBackSilent(0,0,0), colorBackFlash(0,0,0),
        bFlashBorderEnable(false), colorBorderSilent(0,0,0), colorBorderFlash(0,0,0),
        bFlashForeEnable(false), colorForeSilent(0,0,0), colorForeFlash(0,0,0)
    {
    }
}stFlashParam;

#endif // ANIMATIONSTRUCT

