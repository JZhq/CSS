#include "tablemodel.h"
#include <QDebug>
#include <QIcon>

const QString TableModel::KEY_CHECKED(QLatin1Literal("checked"));

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent),
      m_checkableColumn(-1)
{
}

TableModel::~TableModel()
{

}

QVariantHash TableModel::rowData(int row) const
{
    return m_data.value(row);
}

/*!
 * \brief returns the count of row, \a parent is not in use
 */
int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

/*!
 * \brief get the count of column, \a parent is not in use
 */
int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_header.size();
}

/*!
 * \brief TableModel::headerIndexOf
 * \param key
 * \return
 */
int TableModel::headerIndexOf(const QString &key) const
{
    for (int i=0; i<m_header.size(); ++i)
    {
        if (m_header.value(i).first.compare(key, Qt::CaseInsensitive) == 0)
            return i;
    }

    return -1;
}

QVariant TableModel::data(int row, const QString &key) const
{
    return m_data.value(row).value(key);
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        if (role == Qt::DisplayRole)
            return section + 1;

        return QVariant();
    }

    switch (role)
    {
    case HeaderKeyRole:     return m_header.value(section).first;
    case Qt::DisplayRole:   return m_header.value(section).second;
    }

    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractTableModel::flags(index);

    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == m_checkableColumn)
       flag |= Qt::ItemIsUserCheckable;

    if (m_editableColumns.contains(index.column()))
        flag |= Qt::ItemIsEditable;

    return flag;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.column() == m_checkableColumn && role == Qt::CheckStateRole) {
        return QVariant(static_cast<int>(m_data.value(index.row()).value(KEY_CHECKED).toBool() ? Qt::Checked : Qt::Unchecked));
    }

    QString key(headerData(index.column(), Qt::Horizontal, HeaderKeyRole).toString());
    if (role == Qt::TextAlignmentRole )
        return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (m_colValueTransMap.contains(index.column())) {
            return m_colValueTransMap.value(index.column()).value(m_data.value(index.row()).value(key).toString());
        } else {
            return m_data.value(index.row()).value(key);
        }
    } else if (m_roleMap.contains(role)) {
        return m_data.value(index.row()).value(m_roleMap.value(role));
    } else {
        QString roleKey(QStringLiteral("%1#%2").arg(index.column()).arg(role));
        if (m_data.value(index.row()).contains(roleKey)) {
            return m_data.value(index.row()).value(roleKey, QVariant());
        }
    }
    if (role == Qt::DecorationRole && m_colValueIconMap.contains(index.column())) {
        return m_colValueIconMap.value(index.column()).value(m_data.value(index.row()).value(key).toString());
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || data(index, role) == value)
        return false;

    QString col(headerData(index.column(), Qt::Horizontal, HeaderKeyRole).toString());
    if (index.column() == m_checkableColumn && role == Qt::CheckStateRole)
    {
        bool checked = value.canConvert(QVariant::Bool)
                ? value.toBool() : (value.toInt() > Qt::Unchecked ? true : false);
        m_data[index.row()][KEY_CHECKED] = checked;
        if (checked)
            m_checkedRows.insert(index.row());
        else
            m_checkedRows.remove(index.row());

        emit itemChecked(index.row(), checked);
        emit hasItemChecked(m_checkedRows.isEmpty() ? Qt::Unchecked : (m_checkedRows.size() >= rowCount() ? Qt::Checked : Qt::PartiallyChecked));
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole){
        m_data[index.row()][col] = value;
    }else if (m_roleMap.contains(role)){
        m_data[index.row()][QString::fromUtf8(m_roleMap.value(role))] = value;
    }else {
        QString roleKey(QStringLiteral("%1#%2").arg(index.column()).arg(role));
        m_data[index.row()][roleKey] = value;
    }
    if (m_dataIndexMap.contains(index.column()))
        m_dataIndexMap[index.column()][index.row()] = value;

    QVector<int> roles(1);
    roles << role;
    emit dataChanged(index, index, roles);
    return true;
}

QSet<int> TableModel::checkedItems() const
{
    return m_checkedRows;
}

int TableModel::findRow(const QString &key, const QVariant &value) const
{
    int col = headerIndexOf(key);
    if (m_dataIndexMap.contains(col))
        return m_dataIndexMap[col].indexOf(value);

    for (int i=0; i<rowCount(); ++i)
    {
        if (m_data[i].value(key) == value)
            return i;
    }

    return -1;
}

bool TableModel::removeCheckedItems()
{
    QList<int> rowList(m_checkedRows.toList());
    qSort(rowList.begin(), rowList.end());
    beginResetModel();
    for (int i=rowList.size()-1; i>=0; --i)
    {
        m_data.removeAt(rowList[i]);
        QMapIterator<int, QVariantList> it(m_dataIndexMap);
        while (it.hasNext())
        {
            it.next();
            QString col(headerData(it.key(), Qt::Horizontal, HeaderKeyRole).toString());
            m_dataIndexMap[it.key()].removeAt(i);
        }
    }
    endResetModel();

    m_checkedRows.clear();
    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row >= rowCount())
        return false;

    int rowCnt = rowCount();
    beginRemoveRows(parent, row, qMin(row+count, rowCount()) - 1);
    for (int i=qMin(row+count, rowCount()) - 1; i>=row; --i)
    {
        m_data.removeAt(i);
        m_checkedRows.remove(i);

        QMapIterator<int, QVariantList> it(m_dataIndexMap);
        while (it.hasNext())
        {
            it.next();
            QString col(headerData(it.key(), Qt::Horizontal, HeaderKeyRole).toString());
            m_dataIndexMap[it.key()].removeAt(i);
        }
    }
    endRemoveRows();

    int i=row;
    for (; i<rowCount(); ++i)
    {
        if (m_data[i][KEY_CHECKED].toBool())
            m_checkedRows.remove(i);
    }

    for (; i<rowCnt; ++i)
        m_checkedRows.remove(i);

    return true;
}

void TableModel::updateData(const QList<QVariantHash> &data)
{
    beginResetModel();
    m_data.clear();
    m_data = data;
    m_checkedRows.clear();
    endResetModel();
    emit clearSpan();

    int row = 0;
    foreach (const QVariantHash &d, m_data)
    {
        if (d.value(KEY_CHECKED).toBool())
            m_checkedRows.insert(row);
        row++;
    }

    emit hasItemChecked(m_checkedRows.isEmpty() ? Qt::Unchecked : (m_checkedRows.size() >= rowCount() ? Qt::Checked : Qt::PartiallyChecked));
    QList<int> indexCols(m_dataIndexMap.keys());
    m_dataIndexMap.clear();
    foreach (int col, indexCols)
        setDataIndexForColumn(col);

    emit itemAdded(0, rowCount());
}

void TableModel::updateHeaderData(const QList<QPair<QString, QString> > &data)
{
    beginResetModel();
    m_header.clear();
    m_header = data;
    endResetModel();
}

void TableModel::setHeaderData(const QPair<QString, QString> &data)
{
    for(int i = 0;i<m_header.size();i++)
    {
        QPair<QString, QString> pData = m_header[i];
        if(pData.first == data.first)
        {
            beginResetModel();
            m_header[i] = data;
            endResetModel();
            break;
        }
    }
}

void TableModel::appendData(const QVariantHash &data)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_data.append(data);
    if (data.value(KEY_CHECKED).toBool())
    {
        m_checkedRows.insert(row);
        emit itemChecked(row, true);
        emit hasItemChecked(m_checkedRows.isEmpty() ? Qt::Unchecked : (m_checkedRows.size() >= rowCount() ? Qt::Checked : Qt::PartiallyChecked));
    }
    endInsertRows();
    QMapIterator<int, QVariantList> it(m_dataIndexMap);
    while (it.hasNext())
    {
        it.next();
        QString col(headerData(it.key(), Qt::Horizontal, HeaderKeyRole).toString());
        m_dataIndexMap[it.key()].append(data.value(col));
    }

    emit itemAdded(row);
}

void TableModel::insertData( int row, const QVariantHash &data)
{
    int rowNum = rowCount();
    if(row>rowNum)
        row = rowNum;
    beginInsertRows(QModelIndex(), row, row);
    m_data.insert(row,data);
    if (data.value(KEY_CHECKED).toBool())
    {
        m_checkedRows.insert(row);
        emit itemChecked(row, true);
        emit hasItemChecked(m_checkedRows.isEmpty() ? Qt::Unchecked : (m_checkedRows.size() >= rowCount() ? Qt::Checked : Qt::PartiallyChecked));
    }
    endInsertRows();
    QMapIterator<int, QVariantList> it(m_dataIndexMap);
    while (it.hasNext())
    {
        it.next();
        QString col(headerData(it.key(), Qt::Horizontal, HeaderKeyRole).toString());
        m_dataIndexMap[it.key()].append(data.value(col));
    }

    emit itemAdded(row);
}

void TableModel::clearData()
{
    emit clearSpan();
    beginResetModel();
    m_data.clear();
    m_checkedRows.clear();
    m_dataIndexMap.clear();
    emit hasItemChecked(m_checkedRows.isEmpty() ? Qt::Unchecked : (m_checkedRows.size() >= rowCount() ? Qt::Checked : Qt::PartiallyChecked));
    endResetModel();
}

QSet<int> TableModel::editableColumns() const
{
    return m_editableColumns;
}

void TableModel::setEditableColumns(const QSet<int> &editableColumns)
{
    m_editableColumns = editableColumns;
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    return m_roleMap;
}

void TableModel::setRoleNames(const QHash<int, QByteArray> &roleMap)
{
    m_roleMap = roleMap;
}

void TableModel::setColumnValueTranslation(int column, const QVariantHash &transMap)
{
    m_colValueTransMap.insert(column, transMap);
}

void TableModel::setColumnValueIcon(int column, const QHash<QString, QIcon> &iconMap)
{
    m_colValueIconMap.insert(column, iconMap);
}

void TableModel::setDataIndexForColumn(int column)
{
    if (m_dataIndexMap.contains(column))
        return ;

    QString col(headerData(column, Qt::Horizontal, HeaderKeyRole).toString());
    foreach (const QVariantHash &d, m_data)
        m_dataIndexMap[column].append(d.value(col));
}

int TableModel::checkableColumn() const
{
    return m_checkableColumn;
}

void TableModel::setCheckableColumn(int checkableColumn)
{
    m_checkableColumn = checkableColumn;
}

