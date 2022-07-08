/***************************************
 * CopyRight: sinux
 * Description:
 * Authors: zs
 * Date: 2018-05-14
 * History:no
 ***************************************/
#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include "public_global.h"
#include <QStyledItemDelegate>

class GLOBAL_EXPORT CheckBoxDelegate : public QStyledItemDelegate
{
public:
    //!
    //! \brief checkbox 的所在列
    //!
    static const int CHECK_BOX_COLUMN = 0;

public:
    CheckBoxDelegate(QObject *parent = 0);

    //!
    //! \brief 绘制复选框
    //! \param painter
    //! \param option
    //! \param index
    //!
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:

    //!
    //! \brief 响应鼠标事件，更新数据
    //! \param event
    //! \param model
    //! \param option
    //! \param index
    //! \return
    //!
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;


};

#endif // CHECKBOXDELEGATE_H
