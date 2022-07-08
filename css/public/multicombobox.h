#ifndef MULTICOMBOBOX_H
#define MULTICOMBOBOX_H

#include <QComboBox>
#include "public_global.h"

class MultiComboBoxPrivate;

class GLOBAL_EXPORT MultiComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MultiComboBox(QWidget *parent = Q_NULLPTR);
    ~MultiComboBox();

    // QObject interface
public:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;

public:
    //!
    //! \brief 重定义基类 additem
    //! \param text
    //! \param userData
    //!
    void addItem(const QString &text, const QVariant &userData = QVariant());
    QStringList allItem();

    void clearAllData();

    //!
    //! \brief 根据使用者添加的 userrole 获取 item 值
    //! \param role
    //! \return
    //!
    QVariant currentData(int role = Qt::UserRole) const;

    //!
    //! \brief 控制 item 是否被选中
    //! \param row
    //! \param checked
    //!
    void setItemChecked(int row, bool checked = true);
signals:
    void checkStateChanged(const QVariant &userData,bool checked,const QVariant &currentData);
private slots:
    void onStateChanged(int);
    void onLineTextChanged(const QString&);

private:
    void init();

private:
    MultiComboBoxPrivate* d;
};

#endif // MULTICOMBOBOX_H
