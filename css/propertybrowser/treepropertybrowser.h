#ifndef TREEPROPERTYBROWSER_H
#define TREEPROPERTYBROWSER_H

#include <QWidget>
#include <QDate>
#include <QMenu>
#include <QAction>
#include "src/qttreepropertybrowser.h"
#include "src/qtpropertymanager.h"
#include "src/qteditorfactory.h"
#include "propertybrowser_global.h"
#include "propertyconstants.h"

class PROPERTYBROWSERSHARED_EXPORT TreePropertyBrowser : public QtTreePropertyBrowser
{
    Q_OBJECT
public:
    explicit TreePropertyBrowser(QWidget *parent = 0);
    ~TreePropertyBrowser();

    QtAbstractPropertyManager *propertyManager(PropertyType type) const;
    QtProperty *property(const QString &name) const;

protected:
    void onPropertyChanged(const QString &property, const QVariant &value);


signals:
    void propertyChanged(const QString &property, const QVariant &value);
    void showPropertiesSignal();

    void errorVariableName(const QVariant &value);

    void selectItemProperty(const QString &propertyKey, const QString &propertyValue);

    void selectTreeProperty(const QString &propertyKey, const QString &propertyValue);

    //void biaoDashiChanged(const QString &property, const QString &msg);
public slots:
    void showProperties(const QVariantList &properties);
	void updatePropertiesValue(const QVariantMap &values);
	
    //添加属性，并返回添加的属性项(不清空tree)
    QList<QtProperty*> addProperties(const QVariantList &properties);
    //更新多个属性
    void updateProperties(const QVariantList &properties);
    //更新一个属性
    void updateOneProperty(const QVariantHash &property);
    //更新一个属性
    void setFriendVariableName(const QStringList &variableName);
    //设置所有函数列表
    void setAllFuncList(const QList<QString> &funcs);
private:
    void initManager();
    void initEditFactory();
    void setManagerEditor();
    void expandAll(QtBrowserItem *item, bool expand);

    //create property function
    QtProperty *createProperty(const QVariantHash &property);
    QtProperty *createProperty(const QVariantMap &property);

    void updateProperty(QtProperty *parent, const QVariantHash &property);
    void updateProperty(QtProperty *parent, const QVariantMap &property);
    QtProperty *addAssociateProperty(QtProperty *item, const QList<QVariant> &subItems);
    void updateAssociateProperty(QtProperty *parentItem, const QList<QVariant> &subItems);
    void setSubItemEditable(QtProperty *item, const bool &editable);
    void addSubProperty(QtProperty *parentItem, const QList<QVariant> &subItems);
    QtProperty *addGroupProperty(const QString &propertyName, const QList<QVariant> &groupProperty);
    void updateGroupProperty(QtProperty *parentItem, const QList<QVariant> &groupProperty);

private slots:
    void onValueChanged(QtProperty *property, bool value);
    void onValueChanged(QtProperty *property, int value);
    void onValueChanged(QtProperty *property, double value);
    void onValueChanged(QtProperty *property, QString value);
    void onValueChanged(QtProperty *property, QColor value);
    void onValueChanged(QtProperty *property, QFont value);
    void onValueChanged(QtProperty *property, QPoint value);
    void onValueChanged(QtProperty *property, QSize value);
    void onValueChanged(QtProperty *property, QDate value);
    void onValueChanged(QtProperty *property, QVariant value);
    void onValueChanged(QtProperty *property, const QKeySequence &value);
    void onVlaueChanged(QtProperty *property, QRect value);
    void onValueChanged(QtProperty *property, const QVariantList &value);
    void onValueChanged(QtProperty *property, const Range &value);
    void onValueChanged(QtProperty *property, const RangeF &value);
    void onValueChanged(QtProperty *property, const CheckableEnumValue & value);
    void showEditWindows();
    void showContextMenu(const QPoint &pos);
    void setBiaoDaShi(const QString &msg);
    void slotSelectCurrentItem(QtBrowserItem *item);
    /**
      *@brief 接收所有函数信息
      * @param count,funcs
      */
    void onReceiveAllFunction(const QList<QString>&funcs);
private:
    //property manager
    QtDoublePropertyManager *m_doubleManager;
    QtStringPropertyManager *m_stringManager;
    QtColorPropertyManager *m_colorManager;
    QtFontPropertyManager *m_fontManager;
    QtPointPropertyManager *m_pointManager;
    QtSizePropertyManager *m_sizeManager;
    QtIntPropertyManager *m_intManager;
    QtBoolPropertyManager *m_boolManager;
    QtDatePropertyManager *m_dateManager;
    QtEnumPropertyManager *m_enumManager;
    QtGroupPropertyManager *m_groupManager;
    QtKeySequencePropertyManager *m_keySequenceManager;
    QtFilePropertyManager *m_filepathManager;
    QtRectPropertyManager *m_rectManager;
    QtCheckableEnumPropertyManager *m_checkableEnumManager = nullptr;
    QtDialogPropertyManager      *m_dialogManager = nullptr;
    CustomStringTextPropertyManager *m_textManager;
    QtRangePropertyManager       *m_rangeManager = nullptr;
    QtRangeFPropertyManager      *m_rangeFManager = nullptr;

    //property editor factory
    QtDoubleSpinBoxFactory *m_doubleSpinBoxFactory;
    QtCheckBoxFactory *m_checkBoxFactory;
    QtSpinBoxFactory *m_spinBoxFactory;
    QtLineEditFactory *m_lineEditFactory;
    QtEnumEditorFactory *m_comboBoxFactory;
    QtDateEditFactory *m_dateFactory;
    QtColorEditorFactory *m_colorFactory;
    QtKeySequenceEditorFactory *m_keySequenceFactory;
    QtFileEditFactory *m_fileEditFactory;
    QtDialogFactory *m_dialogFactory;
    QtCheckableEnumEditorFactory *m_checkableComboBoxFactory;
    QtRangeFactory               *m_rangeFactory = nullptr;
    QtRangeFFactory              *m_rangeFFactory = nullptr;

    QtTextEditFactory *m_textEditFactory;
    //add by led
    QMenu *menu;
    QAction *editAction;
    QString biaoDaShi;
    QStringList m_variableNamelist;
    QStringList m_currentVariableName;
    QString m_currentZxqdbl;
    QString m_currentXhqdbl;
    QVariantList m_properties;
    //所有函数名称
    QList<QString> m_allFunctionNameList;
    struct ConditionData {
        QString bindProperty;
        QString expression;
        QString targetProperty;
        QVariantMap attributes;

        bool isValid() const { return !bindProperty.isEmpty() && !targetProperty.isEmpty(); }
    };

    struct PropertyData {
        QVariantList properties;
        QStringList propertyOrder;
        QVariantMap propertyValues;
        QSet<QString> updateTriggerProperties;
        // <target property, ConditionData>
        QMultiMap<QString, ConditionData> targetPropertyConditions;
        QMultiHash<int, QtProperty *> propertyTypeToProperties;
        QHash<QString, QtProperty*> propertyNameToProperty;

        void clear() {
            properties.clear();
            propertyOrder.clear();
            propertyValues.clear();
            updateTriggerProperties.clear();
            targetPropertyConditions.clear();
            propertyTypeToProperties.clear();
            propertyNameToProperty.clear();
        }
    } propertyData;

    bool testExpression(const QString &expression, const QVariantMap &compareValues) const;
    bool testCompare(const QString &compare, const QVariantMap &compareValues) const;

    enum CompareType {
        Equal = 0,
        MoreThan,
        LessThan,
        MoreThanOrEqual,
        LessThanOrEqual,
        NotEqual
    };
    static const QMap<QString, CompareType> CompareNames;

    QHash<QString, QtProperty*> propertyNameToQtProperty;
};

#endif // TREEPROPERTYBROWSER_H
