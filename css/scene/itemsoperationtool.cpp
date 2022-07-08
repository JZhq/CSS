#include "itemsoperationtool.h"
#include "items/baseitem.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QMultiMap>
#include <QRectF>
#include <QPointF>

ItemsOperationTool::ItemsOperationTool()
{

}

ItemsOperationTool::~ItemsOperationTool()
{

}

bool ItemsOperationTool::processItemAlign(QGraphicsScene *scene, ItemsOperationTool::Operate align)
{
    bool ret = false;
    QMap<QGraphicsItem*, QPair<QPointF, QPointF>> itemsPos;
    if (scene)
    {
        QPolygonF polygon;
        QList<QGraphicsItem*> list = scene->selectedItems();
        if (!list.isEmpty())
        {
            // 计算包含所有item的最小矩形框
            BaseItem *pBaseItem = 0;
            for (int i=0; i < list.size(); i++)
            {
                pBaseItem = dynamic_cast<BaseItem*>(list.at(i));
                if (pBaseItem)
                {
                    QRectF rect = pBaseItem->boundingRect();
                    polygon.push_back(pBaseItem->mapToScene(rect.topLeft()));
                    polygon.push_back(pBaseItem->mapToScene(rect.topRight()));
                    polygon.push_back(pBaseItem->mapToScene(rect.bottomLeft()));
                    polygon.push_back(pBaseItem->mapToScene(rect.bottomRight()));
                }
            }

            // 开始移动图元
            QRectF boundRect = polygon.boundingRect();
            for (int i=0; i<list.size(); i++)
            {
                pBaseItem = dynamic_cast<BaseItem*>(list.at(i));
                if (pBaseItem)
                {
                    // 先计算移动点
                    QRectF rect = pBaseItem->boundingRect();
                    QPointF movePoint(0, 0);
                    switch (align)
                    {
                        case Align_Left:
                        {
                            qreal x = QPointF(pBaseItem->mapToScene(rect.topLeft())).x();
                            movePoint = QPointF(boundRect.left() - x, 0);
                            break;
                        }
                        case Align_Right:
                        {
                            qreal x = QPointF(pBaseItem->mapToScene(rect.topRight())).x();
                            movePoint = QPointF(boundRect.right() - x, 0);
                            break;
                        }
                        case Align_Top:
                        {
                            qreal y = QPointF(pBaseItem->mapToScene(rect.topLeft())).y();
                            movePoint = QPointF(0, boundRect.top() - y);
                            break;
                        }
                        case Align_Bottom:
                        {
                            qreal y = QPointF(pBaseItem->mapToScene(rect.bottomLeft())).y();
                            movePoint = QPointF(0, boundRect.bottom() - y);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }

                    // 图元移动
                    if (!movePoint.isNull() && !(movePoint.x() == 0 && movePoint.y() == 0))
                    {
                        QPair<QPointF, QPointF> newOldPos;
                        newOldPos.first  = pBaseItem->pos() + movePoint; // 新位置
                        newOldPos.second = pBaseItem->pos(); // 老位置
                        itemsPos[pBaseItem] = newOldPos;

//                        pBaseItem->setPos(pBaseItem->pos() + movePoint);
//                        pBaseItem->update();
                    }
                }
            }
        }

        ret = true;
    }

    emit signalItemAlign(itemsPos);
    return ret;
}

bool ItemsOperationTool::processItemAlignSpacing(QGraphicsScene *scene, ItemsOperationTool::Operate align)
{
    bool ret = false;
    QMap<QGraphicsItem*, QPair<QPointF, QPointF>> itemsPos;

    if (scene)
    {
        QList<QGraphicsItem*> list = scene->selectedItems();
        if (!list.isEmpty())
        {
            // 利用QMultiMap对图元位置进行排序
            QMultiMap<int,BaseItem*> mmpItems;
            BaseItem *pBaseItem = 0;
            if (Align_Hor_AvrSpacing == align)
            {
                for (int i=0; i < list.size(); i++)
                {
                    pBaseItem = dynamic_cast<BaseItem*>(list.at(i));
                    if (pBaseItem)
                    {
                        QRectF rect = pBaseItem->mapToScene(pBaseItem->boundingRect()).boundingRect();
                        int iCenter = (int)rect.center().x();
                        mmpItems.insert(iCenter, pBaseItem);
                    }
                }
            }
            else if (Align_Ver_AvrSpacing == align)
            {
                for (int i=0; i<list.size(); i++)
                {
                    pBaseItem = dynamic_cast<BaseItem*>(list.at(i));
                    if (pBaseItem)
                    {
                        QRectF rect = pBaseItem->mapToScene(pBaseItem->boundingRect()).boundingRect();
                        int iCenter = (int)rect.center().y();
                        mmpItems.insert(iCenter, pBaseItem);
                    }
                }
            }

            // 提取两端的图元
            BaseItem *pMinBaseItem = 0;
            BaseItem *pMaxBaseItem = 0;
            if (mmpItems.size() < 2)
            {
                return ret;
            }
            pMaxBaseItem = (--mmpItems.end()).value();
            pMinBaseItem = mmpItems.begin().value();
            mmpItems.erase(--mmpItems.end());
            mmpItems.erase(mmpItems.begin());

            // 计算图元之间的间距
            float fAllWidth = 0.0;
            float fAllHeight = 0.0;
            float fSpacingWidth = 0.0;
            float fSpacingHeight = 0.0;
            QList<int> keys = mmpItems.keys();
            for (int i=0; i<keys.size(); i++)
            {
                int iKey = keys[i];
                QList<BaseItem*> items = mmpItems.values(iKey);
                BaseItem *pTmpItem = 0;
                for (int j=0; j<items.size(); j++)
                {
                    pTmpItem = items[j];
                    if (pTmpItem)
                    {
                        QRectF rect = pTmpItem->mapToScene(pTmpItem->boundingRect()).boundingRect();
                        fAllWidth += rect.width();
                        fAllHeight += rect.height();
                    }
                }
            }
            fSpacingWidth = pMaxBaseItem->mapToScene(pMaxBaseItem->boundingRect()).boundingRect().left()
                    - pMinBaseItem->mapToScene(pMinBaseItem->boundingRect()).boundingRect().right() - fAllWidth;
            fSpacingHeight = pMaxBaseItem->mapToScene(pMaxBaseItem->boundingRect()).boundingRect().top()
                    - pMinBaseItem->mapToScene(pMinBaseItem->boundingRect()).boundingRect().bottom() - fAllHeight;
            fSpacingWidth = fSpacingWidth / (mmpItems.size() + 1);
            fSpacingHeight = fSpacingHeight / (mmpItems.size() + 1);

            // 开始移动图元
            BaseItem *pLastBaseItem = pMinBaseItem;
            for (int i=0; i<keys.size(); i++)
            {
                int iKey = keys[i];
                QList<BaseItem*> items = mmpItems.values(iKey);
                BaseItem *pTmpItem = 0;
                for (int j=0; j<items.size(); j++)
                {
                    pTmpItem = items[j];
                    if (pTmpItem)
                    {
                        QRectF lastRect = pLastBaseItem->mapToScene(pLastBaseItem->boundingRect()).boundingRect();
                        QRectF rect = pTmpItem->mapToScene(pTmpItem->boundingRect()).boundingRect();
                        float fXpos = lastRect.right() + fSpacingWidth;
                        float fYpos = lastRect.bottom() + fSpacingHeight;
                        QPointF movePoint(0, 0);
                        if (Align_Hor_AvrSpacing == align)
                        {
                            movePoint = QPointF(fXpos-rect.left(), 0);
                        }
                        else
                        {
                            movePoint = QPointF(0, fYpos-rect.top());
                        }
                        // 图元移动
                        if (!movePoint.isNull() && !(movePoint.x() == 0 && movePoint.y() == 0))
                        {
//                            qDebug() << "move points" << movePoint;
//                            qDebug() << "src pos:" << pBaseItem->pos();
//                            qDebug() << "dest pos:" << pBaseItem->pos() + movePoint;
//                            pTmpItem->setPos(pTmpItem->pos() + movePoint);
//                            pTmpItem->update();
                            QPair<QPointF, QPointF> newOldPos;
                            newOldPos.first  = pTmpItem->pos() + movePoint; // 新位置
                            newOldPos.second = pTmpItem->pos(); // 老位置
                            itemsPos[pTmpItem] = newOldPos;
                        }
                        pLastBaseItem = pTmpItem;
                    }
                }
            }
        }

        ret = true;
    }
    emit signalItemAlignSpacing(itemsPos);
    return ret;
}
