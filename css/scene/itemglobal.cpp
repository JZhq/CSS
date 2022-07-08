#include "itemglobal.h"

ItemGlobal::ItemGlobal()
    : m_currentDrawType(NoneType)
{
}
ItemGlobal::~ItemGlobal()
{

}
ItemGlobal *ItemGlobal::getInstance()
{
    static ItemGlobal itemGlobal;
    return &itemGlobal;
}
void ItemGlobal::setCurrentDrawType(ItemAndPropertyType type)
{
    this->m_currentDrawType = type;
}

ItemAndPropertyType ItemGlobal::getCurrentDrawType() const
{
    return m_currentDrawType;
}
