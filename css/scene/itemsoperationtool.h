#ifndef ITEMSOPERATIONTOOL_H
#define ITEMSOPERATIONTOOL_H

#include <QObject>
#include <QMap>
#include "singleton.h"
#include "itemsoperationsuspendwidget.h"

class QGraphicsScene;
class QGraphicsItem;

#define ItemsOperationToolInstance Singleton<ItemsOperationTool>::getInstance()
class ItemsOperationTool : public QObject
{
    Q_OBJECT
    SINGLETON(ItemsOperationTool)

public:
    //!
    //! \brief 图元对齐方式
    //!
    enum Operate {
        Align_Left,
        Align_Right,
        Align_Top,
        Align_Bottom,
        Align_Ver_AvrSpacing,
        Align_Hor_AvrSpacing
    };

    //!
    //! \brief 处理图元对齐方式
    //! \param scene 场景
    //! \param align 图元对齐方式
    //! \return 处理结果
    //!
    bool processItemAlign(QGraphicsScene *scene, Operate align);

    //!
    //! \brief 处理图元间距对齐方式
    //! \param scene 场景
    //! \param align 图元对齐方式
    //! \return
    //!
    bool processItemAlignSpacing(QGraphicsScene *scene, Operate align);

signals:
    //!
    //! \brief 图元对齐
    //!
    void signalItemAlign(QMap<QGraphicsItem*, QPair<QPointF, QPointF>>);

    //!
    //! \brief 图元间距对齐
    //!
    void signalItemAlignSpacing(QMap<QGraphicsItem*, QPair<QPointF, QPointF>>);

};

#endif // ITEMSOPERATIONTOOL_H
