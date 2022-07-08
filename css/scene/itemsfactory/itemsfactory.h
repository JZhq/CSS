/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         图元工厂类
*@author        luox
*@date          2018-05-23
**/
#ifndef ITEMSFACTORY_H
#define ITEMSFACTORY_H

#include <QRectF>

class BaseItem;

class ItemsFactory
{
protected:
    explicit ItemsFactory()
    {

    }

public:
    static ItemsFactory* instance();

public:
    /**********************************************************************
     * Function:    creatItem
     * Description: 根据图元类型创建图元
     * Input:       图元类型
     * Output       图元指针
     * Return:      图元指针
     **********************************************************************/
    BaseItem *creatItem(int itemType);

    /**********************************************************************
     * Function:    itemsBoundingRect
     * Description: 计算图元所在rect
     * Input:       void
     * Output       void
     * Return:      void
     **********************************************************************/
    QRectF itemsBoundingRect(QList<BaseItem*> items);

private:
    static ItemsFactory *m_pItemsFactory;
};

#endif // ITEMSFACTORY_H
