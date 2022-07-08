#ifndef COMBOBOXITEMDELEGATE_H
#define COMBOBOXITEMDELEGATE_H
#include "public_global.h"
#include <QStyledItemDelegate>

class GLOBAL_EXPORT ComboBoxItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    struct Option
    {
        Option() {}
        Option(const QIcon &_icon, const QString &_text, const QVariant &_value)
            : icon (_icon ),
              text (_text ),
              value(_value)
        { }
        Option(const Option &_other)
            : icon (_other.icon ),
              text (_other.text ),
              value(_other.value)
        { }
        Option &operator = (const Option &_other)
        {
            if (this != &_other)
            {
                icon  = _other.icon ;
                text  = _other.text ;
                value = _other.value;
            }
            return *this;
        }

        QIcon    icon;
        QString  text;
        QVariant value;
    };

    ComboBoxItemDelegate(int role, const QList<Option> &options, QObject *parent = 0);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int m_role;
    QList<Option> m_options;
    QVariantList m_valueList;
};

#endif // COMBOBOXITEMDELEGATE_H
