#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include <QVariantList>
#include <QVariantHash>
#include <QString>
#include <QSet>
#include <QList>
#include <QIcon>
#include <QModelIndex>
#include "public_global.h"
class GLOBAL_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(QObject *parent = 0);

    ~TableModel();

    enum HeaderRole
    {
        HeaderKeyRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    int headerIndexOf(const QString &key) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariantHash rowData(int row) const;
    QVariant data(int row, const QString &key) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QSet<int> checkedItems() const;

    int findRow(const QString &key, const QVariant &value) const;

    bool removeCheckedItems();
    bool removeRows(int row, int count, const QModelIndex &parent);

    int checkableColumn() const;
    void setCheckableColumn(int checkableColumn);

    QSet<int> editableColumns() const;
    void setEditableColumns(const QSet<int> &editableColumns);

    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray> &roleMap);

    void setColumnValueTranslation(int column, const QVariantHash &transMap);
    void setColumnValueIcon(int column, const QHash<QString, QIcon> &iconMap);

    void setDataIndexForColumn(int column);

signals:
    void clearSpan();
    void settingSpan(int row, int colStart, int colSize);
    void itemChecked(int row, bool checked);
    void hasItemChecked(int checkState);
    void itemAdded(int row, int count = 1);

public slots:
    void updateData(const QList<QVariantHash> &data);
    void updateRowData(const QModelIndex &index, const QVariantHash &d);
    void updateHeaderData(const QList<QPair<QString, QString> > &data);
    void setHeaderData(const QPair<QString, QString> &data);
    void appendData(const QVariantHash &data);
    void insertData(int row, const QVariantHash &data);
    void clearData();

private:
    QList<QVariantHash> m_data;
    QList<QPair<QString, QString> > m_header;
    QSet<int> m_checkedRows;
    int m_checkableColumn;
    QSet<int> m_editableColumns;
    QHash<int, QByteArray> m_roleMap;
    // <column, <value, display> >
    QHash<int, QVariantHash> m_colValueTransMap;
    // <column, <value, icon path> >
    QHash<int, QHash<QString, QIcon> > m_colValueIconMap;

    // column, value list -> row
    QMap<int, QVariantList> m_dataIndexMap;

public:
    static const QString KEY_CHECKED;
};
#endif // TABLEMODEL_H
