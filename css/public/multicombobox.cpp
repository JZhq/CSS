#include "multicombobox.h"
#include <QCheckBox>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>

class MultiComboBoxPrivate
{
public:
    MultiComboBoxPrivate() :
        selectedDataLineEdit(Q_NULLPTR),
        checkableListWidget(Q_NULLPTR),
        isChecked(false)
    {

    }

    QString selectedData;
    QLineEdit* selectedDataLineEdit;
    QListWidget* checkableListWidget;
    QStringList allTexts;
    bool isChecked;
};

MultiComboBox::MultiComboBox(QWidget *parent) :
    QComboBox(parent),
    d(new MultiComboBoxPrivate)
{
    init();
}

MultiComboBox::~MultiComboBox()
{
    if (d)
        delete d;
}

bool MultiComboBox::eventFilter(QObject* watched, QEvent* event)
{
    if (d->selectedDataLineEdit == watched) {
        if (event->type() == QEvent::MouseButtonPress) {
            showPopup();
            return true;
        }
        return false;
    }
    return QComboBox::eventFilter(watched, event);
}

void MultiComboBox::init()
{
    d->selectedDataLineEdit = new QLineEdit(this);
    d->checkableListWidget = new QListWidget(this);
    // d->selectedDataLineEdit->setMinimumWidth(300);

    d->checkableListWidget->setMinimumWidth(170);
    //this->setMaximumWidth(d->selectedDataLineEdit->width() + 100);
    this->setModel(d->checkableListWidget->model());
    this->setView(d->checkableListWidget);
    this->setLineEdit(d->selectedDataLineEdit);
    d->selectedDataLineEdit->setReadOnly(true);
    connect(d->selectedDataLineEdit, SIGNAL(textChanged(const QString&)), this,
        SLOT(onLineTextChanged(const QString&)));

    d->selectedDataLineEdit->installEventFilter(this);
}

void MultiComboBox::addItem(const QString &text, const QVariant &userData)
{
    QListWidgetItem* item = new QListWidgetItem(d->checkableListWidget);
    d->checkableListWidget->addItem(item);
    QCheckBox* checkBox = new QCheckBox(this);
    checkBox->setText(text);
    item->setData(Qt::UserRole, userData);
    d->checkableListWidget->addItem(item);
    d->checkableListWidget->setItemWidget(item, checkBox);
    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
    d->allTexts.append(text);
}

QStringList MultiComboBox::allItem()
{
    return d->allTexts;
}

void MultiComboBox::clearAllData()
{
    d->allTexts.clear();
    this->clear();
}

void MultiComboBox::onStateChanged(int)
{
    d->isChecked = true;
    int count = d->checkableListWidget->count();
    d->selectedData.clear();
    QCheckBox* sendCheckBox = qobject_cast<QCheckBox*>(sender());
    QListWidgetItem* curItem = nullptr;
    for (int i = 0; i != count; ++i) {
        QListWidgetItem* item = d->checkableListWidget->item(i);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(d->checkableListWidget->itemWidget(item));
        if (checkBox->isChecked()) {
            QString content = checkBox->text();
            //修改复选状态的分隔符号从“：”改为“&”
            d->selectedData.append(content).append("&");
        }
        if(sendCheckBox == checkBox)
        {
            curItem = item;
        }
    }
    if (d->selectedData.endsWith("&")) {
        d->selectedData.remove(d->selectedData.count() - 1, 1);
    }
    d->selectedDataLineEdit->setText(d->selectedData);
    d->selectedDataLineEdit->setToolTip(d->selectedData);
    d->isChecked = false;
    if(curItem&&sendCheckBox)
        emit checkStateChanged(curItem->data(Qt::UserRole),sendCheckBox->isChecked(),currentData());

}

void MultiComboBox::onLineTextChanged(const QString&)
{
    if (!d->isChecked) {
        d->selectedDataLineEdit->setText(d->selectedData);
    }
}

QVariant MultiComboBox::currentData(int role) const
{
    QStringList itemsData;
    int count = d->checkableListWidget->count();
    for (int i = 0; i != count; ++i) {
        QListWidgetItem* item = d->checkableListWidget->item(i);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(d->checkableListWidget->itemWidget(item));
        if (checkBox->isChecked()) {
           itemsData << item->data(role).toString();
        }
    }
    QVariant data(itemsData);
    return data;
}

void MultiComboBox::setItemChecked(int row, bool checked)
{
    QListWidgetItem* item = d->checkableListWidget->item(row);
    if (item) {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>
                    (d->checkableListWidget->itemWidget(item));
        checkBox->setChecked(checked);
    }
}
