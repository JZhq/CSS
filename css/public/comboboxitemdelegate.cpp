#include "comboboxitemdelegate.h"
#include <QComboBox>
#include <QIcon>
#include <QDebug>

ComboBoxItemDelegate::ComboBoxItemDelegate(int role, const QList<ComboBoxItemDelegate::Option> &options, QObject *parent)
    : QStyledItemDelegate(parent),
      m_role(role),
      m_options(options)
{
    foreach (const Option &d, m_options)
        m_valueList << d.value;
}

QWidget *ComboBoxItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QComboBox *comboBox = new QComboBox(parent);
    foreach (const Option &d, m_options)
        comboBox->addItem(d.icon, d.text, d.value);

    return comboBox;
}

void ComboBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *> (editor);
    if (comboBox == NULL)
        return ;

    comboBox->setCurrentIndex(m_valueList.indexOf(index.model()->data(index, m_role)));
}

void ComboBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *> (editor);
    model->setData(index, comboBox->currentData(), m_role);
}

void ComboBoxItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}
