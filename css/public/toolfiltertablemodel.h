#ifndef TOOLFILTERTABLEMODEL_H
#define TOOLFILTERTABLEMODEL_H

#include "tablemodel.h"

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#include "public_global.h"
#include <QSortFilterProxyModel>

class GLOBAL_EXPORT ToolFilterTableModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ToolFilterTableModel(TableModel *source = nullptr);

    enum CompareType {
        Equal,
        NotEqual,
        Contains,
        LessThen,
        EqualOrLessThen,
        MoreThen,
        EqualOrMoreThen,
        Between,
        EqualInt,
    };

    void filter(int role, CompareType compare, const QString &val1, const QString &val2 = QString());
    void unfilter(int role);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

signals:

private:
    TableModel *srcModel = nullptr;
    struct FilterItem {
        int role = 0;
        CompareType compare = Equal;
        QString value1;
        QString value2;
    };

    QMap<int, FilterItem> filters;
};

#endif // TOOLFILTERTABLEMODEL_H
