#include "flashengine.h"
#include "items/baseitem.h"

FlashEngine::FlashEngine(QObject *parent, BaseItem *pItem) :
    QObject(parent),
    m_bEngineStatus(false),
    m_pItem(pItem)
{

}

void FlashEngine::startEngine()
{
    m_bEngineStatus = true;

    QTimer::singleShot(500, this, SLOT(slotFlash()));
}

bool FlashEngine::engineStatus()
{
    return m_bEngineStatus;
}

void FlashEngine::slotFlash()
{
//    if (m_pItem)
//    {
//        FlashParam flashParam = m_pItem->getFlashParam();

//        // 背景色
//        if (FObj_Background & flashParam.flashObj)
//        {
//            flashParam.bFlashBack = !flashParam.bFlashBack;
//        }
//        else
//        {
//            flashParam.bFlashBack = false;
//        }

//        // 边线颜色
//        if (FObj_Border & flashParam.flashObj)
//        {
//            flashParam.bFlashBorder = !flashParam.bFlashBorder;
//        }
//        else
//        {
//            flashParam.bFlashBorder = false;
//        }

//        // 前景色
//        if (FObj_Foreground & flashParam.flashObj)
//        {
//            flashParam.bFlashFore = !flashParam.bFlashFore;
//        }
//        else
//        {
//            flashParam.bFlashFore = false;
//        }

//        m_pItem->setFlashParam(flashParam);
//        m_pItem->update();
//    }

    QTimer::singleShot(500, this, SLOT(slotFlash()));
}
