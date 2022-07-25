#ifndef GROUPBOXPROPERTYBROWSER_H
#define GROUPBOXPROPERTYBROWSER_H

#include <QWidget>
#include <QDate>
#include "src/qtgroupboxpropertybrowser.h"
#include "src/qtpropertymanager.h"
#include "src/qteditorfactory.h"
#include "propertybrowser_global.h"
#include "propertyconstants.h"


class PROPERTYBROWSERSHARED_EXPORT GroupBoxPropertyBrowser : public QtGroupBoxPropertyBrowser
{
    Q_OBJECT
public:
    explicit GroupBoxPropertyBrowser(QWidget *parent = 0);
    ~GroupBoxPropertyBrowser();

protected:
    void initData();
    void onPropertyChanged(const QString &property, const QVariant &value);
    QtAbstractPropertyManager *propertyManager(PropertyType type) const;

signals:
    void propertyChanged(const QString &property, const QVariant &value, const QVariant &oldValue = QVariant());

public slots:
    virtual void showProperties(const QVariantList &properties = QVariantList());
    bool updatePropertiesValue(const QVariantMap &values);

    void onValueChanged(QtProperty *property, bool         value);
    void onValueChanged(QtProperty *property, int          value);
    void onValueChanged(QtProperty *property, double       value);
    void onValueChanged(QtProperty *property, QString      value);
    void onValueChanged(QtProperty *property, QColor       value);
    void onValueChanged(QtProperty *property, QFont        value);
    void onValueChanged(QtProperty *property, QPoint       value);
    void onValueChanged(QtProperty *property, QSize        value);
    void onValueChanged(QtProperty *property, QDate        value);
    void onValueChanged(QtProperty *property, QVariant     value);
    void onValueChanged(QtProperty *property, QKeySequence value);
    void onValueChanged(QtProperty *property, QRect        value);
    void onValueChanged(QtProperty *property, const QVariantList &value);
    void onValueChanged(QtProperty *property, const Range &value);
    void onValueChanged(QtProperty *property, const RangeF &value);
    void onValueChanged(QtProperty *property, const CheckableEnumValue & value);
    void onValueEditFinished(QtProperty *property, const QString &value);

private:
    void initManager();
    void initEditFactory();
    void setManagerEditor();

       //create property function
    QtProperty *createProperty(const QMap<QString, QVariant> &property);
    QtProperty *addAssociateProperty(QtProperty *item, const QList<QVariant> &subItems);
    void setSubItemEditable(QtProperty *item, const bool &editable);
    void addSubProperty(QtProperty *parentItem, const QList<QVariant> &subItems);
    QtProperty *addGroupProperty(const QString &propertyName, const QList<QVariant> &groupProperty);

private:
    //property manager
    QtDoublePropertyManager      *m_doubleManager = nullptr;
    QtStringPropertyManager      *m_stringManager = nullptr;
    QtColorPropertyManager       *m_colorManager = nullptr;
    QtFontPropertyManager        *m_fontManager = nullptr;
    QtPointPropertyManager       *m_pointManager = nullptr;
    QtSizePropertyManager        *m_sizeManager = nullptr;
    QtIntPropertyManager         *m_intManager = nullptr;
    QtBoolPropertyManager        *m_boolManager = nullptr;
    QtDatePropertyManager        *m_dateManager = nullptr;
    QtDateTimePropertyManager    *m_dateTimeManager = nullptr;
    QtTimePropertyManager        *m_timeManager = nullptr;
    QtEnumPropertyManager        *m_enumManager = nullptr;
    QtCheckableEnumPropertyManager *m_checkableEnumManager = nullptr;
    QtGroupPropertyManager       *m_groupManager = nullptr;
    QtKeySequencePropertyManager *m_keySequenceManager = nullptr;
    QtRectPropertyManager        *m_rectManager = nullptr;
    QtFlagPropertyManager        *m_flagManager = nullptr;
    QtCharPropertyManager        *m_charManager = nullptr;
    QtSizePolicyPropertyManager  *m_sizePolicyManager = nullptr;
    QtFilePropertyManager        *m_filepathManager = nullptr;
    QtDialogPropertyManager      *m_dialogManager = nullptr;
    QtRangePropertyManager       *m_rangeManager = nullptr;
    QtRangeFPropertyManager      *m_rangeFManager = nullptr;

       //property editor factory
    QtDoubleSpinBoxFactory       *m_doubleSpinBoxFactory = nullptr;
    QtCheckBoxFactory            *m_checkBoxFactory = nullptr;
    QtSpinBoxFactory             *m_spinBoxFactory = nullptr;
    QtLineEditFactory            *m_lineEditFactory = nullptr;
    QtEnumEditorFactory          *m_comboBoxFactory = nullptr;
    QtCheckableEnumEditorFactory *m_checkableComboBoxFactory = nullptr;
    QtDateEditFactory            *m_dateFactory = nullptr;
    QtDateTimeEditFactory        *m_dateTimeFactory = nullptr;
    QtTimeEditFactory            *m_timeFactory = nullptr;
    QtColorEditorFactory         *m_colorFactory = nullptr;
    QtKeySequenceEditorFactory   *m_keySequenceFactory = nullptr;
    QtCharEditorFactory          *m_charEditFactory = nullptr;
    QtFileEditFactory            *m_fileEditFactory = nullptr;
    QtDialogFactory              *m_dialogFactory = nullptr;
    QtRangeFactory               *m_rangeFactory = nullptr;
    QtRangeFFactory              *m_rangeFFactory = nullptr;

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

};


#endif // GROUPBOXPROPERTYBROWSER_H
