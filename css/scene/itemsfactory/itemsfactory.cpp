﻿#include "itemsfactory.h"
#include "baseitem.h"

#include "plugnodeitem.h"

ItemsFactory *ItemsFactory::m_pItemsFactory = 0;

ItemsFactory *ItemsFactory::instance()
{
    if (0 == ItemsFactory::m_pItemsFactory)
    {
        ItemsFactory::m_pItemsFactory = new ItemsFactory();
    }

    return ItemsFactory::m_pItemsFactory;
}

BaseItem *ItemsFactory::creatItem(int itemType)
{
    BaseItem *pRetItem = 0;
    switch (itemType)
    {
    default:
        break;
    }

    return pRetItem;
}


QRectF ItemsFactory::itemsBoundingRect(QList<BaseItem *> items)
{
    QRectF retRect(0, 0, 0, 0);

    QPolygonF polygon;
    BaseItem *pCurItem = 0;
    for (int i=0; i<items.size(); i++)
    {
        pCurItem = items.at(i);
        if (pCurItem)
        {
            QRectF curRect = pCurItem->boundingRect();
            polygon.push_back(curRect.topLeft());
            polygon.push_back(curRect.topRight());
            polygon.push_back(curRect.bottomRight());
            polygon.push_back(curRect.bottomLeft());
        }
    }

    retRect = polygon.boundingRect();

    return retRect;
}