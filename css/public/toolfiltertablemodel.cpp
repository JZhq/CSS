#include "toolfiltertablemodel.h"

ToolFilterTableModel::ToolFilterTableModel(TableModel *parentModel)
    : QSortFilterProxyModel(parentModel)
    , srcModel(parentModel)
{

}

void ToolFilterTableModel::filter(int role, CompareType compare, const QString &val1, const QString &val2)
{
    FilterItem filter;
    filter.role = role;
    filter.compare = compare;
    filter.value1 = val1;
    filter.value2 = val2;
    filters[role] = filter;
    invalidateFilter();
}

void ToolFilterTableModel::unfilter(int role)
{
    filters.remove(role);
    invalidateFilter();
}

bool ToolFilterTableModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex idx = sourceModel()->index(source_row, 0);
    for (auto filter : filters) {
        QString content = sourceModel()->data(idx, filter.role).toString();
        switch (filter.compare) {
        case EqualInt:        if (filter.value1.toInt() != content.toInt()) return false; break;
        case Equal:           if (filter.value1.compare(content, Qt::CaseInsensitive) != 0) return false; break;
        case NotEqual:        if (filter.value1.compare(content, Qt::CaseInsensitive) == 0) return false; break;
        case Contains:        if (!content.contains(filter.value1, Qt::CaseInsensitive)) return false; break;
        case LessThen:        if (content >= filter.value1) return false; break;
        case EqualOrLessThen: if (content > filter.value1) return false; break;
        case MoreThen:        if (content <= filter.value1) return false; break;
        case EqualOrMoreThen: if (content < filter.value1) return false; break;
        case Between:         if (content < filter.value1 || content > filter.value2) return false; break;
        }
    }

    return true;
}
