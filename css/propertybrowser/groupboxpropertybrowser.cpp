#include "groupboxpropertybrowser.h"
#include <QTranslator>
#include <QVariant>
#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>
#include <QSet>
#include <QTimer>
#include <QDoubleValidator>
#include <QTimer>

const QMap<QString, GroupBoxPropertyBrowser::CompareType> GroupBoxPropertyBrowser::CompareNames = {
    { "=",  GroupBoxPropertyBrowser::Equal },
    { "==", GroupBoxPropertyBrowser::Equal },
    { ">",  GroupBoxPropertyBrowser::MoreThan },
    { ">=", GroupBoxPropertyBrowser::MoreThanOrEqual },
    { "<",  GroupBoxPropertyBrowser::LessThan },
    { "<=", GroupBoxPropertyBrowser::LessThanOrEqual },
    { "!=", GroupBoxPropertyBrowser::NotEqual },
};

GroupBoxPropertyBrowser::GroupBoxPropertyBrowser(QWidget *parent)
    : QtGroupBoxPropertyBrowser(parent)
{
    initData();
}

GroupBoxPropertyBrowser::~GroupBoxPropertyBrowser()
{
    unsetFactoryForManager(m_doubleManager);
    unsetFactoryForManager(m_stringManager);
    unsetFactoryForManager(m_colorManager->subIntPropertyManager());
    unsetFactoryForManager(m_colorManager);
    unsetFactoryForManager(m_fontManager->subIntPropertyManager());
    unsetFactoryForManager(m_fontManager->subBoolPropertyManager());
    unsetFactoryForManager(m_fontManager->subEnumPropertyManager());
    unsetFactoryForManager(m_pointManager->subIntPropertyManager());
    unsetFactoryForManager(m_sizeManager->subIntPropertyManager());
    unsetFactoryForManager(m_rectManager->subIntPropertyManager());
    unsetFactoryForManager(m_intManager);
    unsetFactoryForManager(m_boolManager);
    unsetFactoryForManager(m_dateManager);
    unsetFactoryForManager(m_enumManager);
    unsetFactoryForManager(m_checkableEnumManager);
    unsetFactoryForManager(m_keySequenceManager);
    unsetFactoryForManager(m_filepathManager);
    unsetFactoryForManager(m_dialogManager);
}

void GroupBoxPropertyBrowser::initData()
{
//    QTranslator qtTranslator;
//    if (qtTranslator.load(":/propertybrowser/zh_CN.qm"))
//        qApp->installTranslator(&qtTranslator);

//    QTranslator qtMissedTranslator;
//    if (qtMissedTranslator.load(":/propertybrowser/qt_missed_zh_CN.qm"))
//        qApp->installTranslator(&qtMissedTranslator);

    initEditFactory();
    initManager();
    setManagerEditor();
}

void GroupBoxPropertyBrowser::onPropertyChanged(const QString &property, const QVariant &value)
{
    propertyData.propertyValues[property] = value;

    if (signalsBlocked()) {
        return ;
    }

    emit propertyChanged(property, value, propertyData.propertyValues.value(property));

    // handle property conditions
    if (propertyData.updateTriggerProperties.contains(property)) {
        QTimer::singleShot(0, [=](){
            clear();
            for(auto propertyItem : propertyData.properties) {
                if(propertyItem.isNull()) {
                    continue;
                }
                QtProperty *item = createProperty(propertyItem.toMap());
                if (item != nullptr) {
                    addProperty(item);
                }
            }
        });
    }
}

QtAbstractPropertyManager *GroupBoxPropertyBrowser::propertyManager(PropertyType type) const
{
    switch (type) {
    case PROPERTY_TYPE_CHAR          : return m_charManager         ;
    case PROPERTY_TYPE_BOOL          : return m_boolManager         ;
    case PROPERTY_TYPE_INT           : return m_intManager          ;
    case PROPERTY_TYPE_DOUBLE        : return m_doubleManager       ;
    case PROPERTY_TYPE_STRING        : return m_stringManager       ;
    case PROPERTY_TYPE_TEXTSTRING    : return m_stringManager       ;
    case PROPERTY_TYPE_ENUM          : return m_enumManager         ;
    case PROPERTY_TYPE_CHECKABLE_ENUM: return m_checkableEnumManager;
    case PROPERTY_TYPE_ENUM_ICON     : return m_enumManager         ;
    case PROPERTY_TYPE_FLAG          : return m_flagManager         ;
    case PROPERTY_TYPE_COLOR         : return m_colorManager        ;
    case PROPERTY_TYPE_DATE          : return m_dateManager         ;
    case PROPERTY_TYPE_DATETIME      : return m_dateTimeManager     ;
    case PROPERTY_TYPE_TIME          : return m_timeManager         ;
    case PROPERTY_TYPE_POINT         : return m_pointManager        ;
    case PROPERTY_TYPE_SIZE          : return m_sizeManager         ;
    case PROPERTY_TYPE_RECT          : return m_rectManager         ;
    case PROPERTY_TYPE_FILESELECT    : return m_filepathManager     ;
    case PROPERTY_TYPE_FONT          : return m_fontManager         ;
    case PROPERTY_TYPE_KEYSEQUENCE   : return m_keySequenceManager  ;
    case PROPERTY_TYPE_SIZEPOLICY    : return m_sizePolicyManager   ;
    case PROPERTY_TYPE_RANGE         : return m_sizePolicyManager   ;
    case PROPERTY_TYPE_RANGEF        : return m_sizePolicyManager   ;
    case PROPERTY_TYPE_DIALOG        : return m_dialogManager       ;
    default: break;
    }
    return nullptr;
}

void GroupBoxPropertyBrowser::showProperties(const QVariantList &properties)
{
    propertyData.clear();

    for (auto property : properties) {
        QVariantMap propertyMap(property.toMap());
        QString propertyName(propertyMap.value(PropertyConstants::PROPERTY_NAME).toString());
        propertyData.propertyValues[propertyName] = propertyMap.value(PropertyConstants::PROPERTY_VALUE);
        if (propertyMap.contains(PropertyConstants::PROPERTY_CONDITION)) {
            for (auto condition : propertyMap.value(PropertyConstants::PROPERTY_CONDITION).toList()) {
                QVariantMap conditionMap = condition.toMap();
                QString whenString = conditionMap.take(PropertyConstants::PROPERTY_CONDITION_WHEN).toString();
                QSet<QString> bindProperties;
                QRegularExpression re("'(?<property>\\w+)'.+'\\w+'");
                for (auto sub : whenString.split(QRegExp("(\\|\\||&&)"), QString::SkipEmptyParts)) {
                    QRegularExpressionMatch match = re.match(sub);
                    if (match.hasMatch()) {
                        QString bindProp(match.captured("property"));
                        if (!bindProperties.contains(bindProp)) {
                            bindProperties << bindProp;
                            propertyData.updateTriggerProperties.insert(bindProp);
                            propertyData.targetPropertyConditions.insert(propertyName, {bindProp, whenString, propertyName, conditionMap});
                        }
                    }
                }
            }
        }
    }

    propertyData.properties = properties;

    clear();

    for(auto property : propertyData.properties) {
        if(property.isNull() || property.toMap().isEmpty()) {
            continue;
        }

        QtProperty *item = createProperty(property.toMap());
        if (item != nullptr) {
            addProperty(item);
        }
    }
}

bool GroupBoxPropertyBrowser::updatePropertiesValue(const QVariantMap &values)
{
    if (values.isEmpty()) {
        return false;
    }
    bool needReloadProperties = false;
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        QtProperty *property = propertyData.propertyNameToProperty.value(it.key(), nullptr);

        if (property == nullptr) {
            continue;
        }

        qDebug() << "\t" << it.key() << "?" << property->propertyKey() << "\t" << property->propertyName() << "\t" << it.value();
        if (!needReloadProperties && propertyData.updateTriggerProperties.contains(it.key())) {
            needReloadProperties = true;
        }

        switch (propertyData.propertyTypeToProperties.key(property, PROPERTY_TYPE_INVALID)) {
        case PROPERTY_TYPE_CHAR          : m_charManager->setValue(property, it.value().toChar()); break;
        case PROPERTY_TYPE_BOOL          : m_boolManager->setValue(property, it.value().toBool()); break;
        case PROPERTY_TYPE_INT           : m_intManager->setValue(property, it.value().toInt()); break;
        case PROPERTY_TYPE_DOUBLE        : m_doubleManager->setValue(property, it.value().toDouble()); break;
        case PROPERTY_TYPE_STRING        : m_stringManager->setValue(property, it.value().toString()); break;
        case PROPERTY_TYPE_TEXTSTRING    : m_stringManager->setValue(property, it.value().toString()); break;
        case PROPERTY_TYPE_ENUM          : m_enumManager->setValue(property, it.value().toInt()); break;
        case PROPERTY_TYPE_CHECKABLE_ENUM: {
            if (it.value().canConvert<CheckableEnumValue>()) {
                m_checkableEnumManager->setValue(property, it.value().value<CheckableEnumValue>());
            } else if (it.value().isNull()) {
                m_checkableEnumManager->setValue(property, CheckableEnumValue());
            } else {
                QList<int> indexes;
                QList<int> keys;
                QVariantList enumKeys = m_checkableEnumManager->enumKeys(property);
                QStringList enumNames = m_checkableEnumManager->enumNames(property);
                bool hasKeys = enumKeys.size() == enumNames.size();
                bool isIndex = true;
                if (it.value().type() == QVariant::List) {
                    for (auto idx : it.value().toList()) {
                        if (idx.type() == QVariant::Int) {
                            indexes << idx.toInt();
                            keys << (hasKeys ? enumKeys.indexOf(idx) : enumNames.indexOf(idx.toString()));
                            if (idx.toInt() < 0 || idx >= enumKeys.size()) {
                                isIndex = false;
                            }
                        } else {
                            indexes << (hasKeys ? enumKeys.indexOf(idx) : enumNames.indexOf(idx.toString()));
                        }
                    }
                }
                m_checkableEnumManager->setValue(property, isIndex ? indexes : keys);
            }
        } break;
        case PROPERTY_TYPE_ENUM_ICON     : m_enumManager->setValue(property, it.value().toInt()); break;
        case PROPERTY_TYPE_FLAG          : m_flagManager->setValue(property, it.value().toInt()); break;
        case PROPERTY_TYPE_COLOR         : m_colorManager->setValue(property, it.value().value<QColor>()); break;
        case PROPERTY_TYPE_DATE          : m_dateManager->setValue(property, it.value().toDate()); break;
        case PROPERTY_TYPE_DATETIME      : m_dateTimeManager->setValue(property, it.value().toDateTime()); break;
        case PROPERTY_TYPE_TIME          : m_timeManager->setValue(property, it.value().toTime()); break;
        case PROPERTY_TYPE_POINT         : m_pointManager->setValue(property, it.value().toPoint()); break;
        case PROPERTY_TYPE_SIZE          : m_sizeManager->setValue(property, it.value().toSize()); break;
        case PROPERTY_TYPE_RECT          : m_rectManager->setValue(property, it.value().toRect()); break;
        case PROPERTY_TYPE_FILESELECT    : m_filepathManager->setValue(property, it.value().toString()); break;
        case PROPERTY_TYPE_FONT          : m_fontManager->setValue(property, it.value().value<QFont>()); break;
        case PROPERTY_TYPE_KEYSEQUENCE   : m_keySequenceManager->setValue(property, it.value().value<QKeySequence>()); break;
        case PROPERTY_TYPE_SIZEPOLICY    : m_sizePolicyManager->setValue(property, it.value().value<QSizePolicy>()); break;
        case PROPERTY_TYPE_DIALOG        : m_dialogManager->setValue(property, it.value()); break;
        }
    }

    return needReloadProperties;
}

void GroupBoxPropertyBrowser::initManager()
{
    m_doubleManager      = new QtDoublePropertyManager     (this);
    m_stringManager      = new QtStringPropertyManager     (this);
    m_colorManager       = new QtColorPropertyManager      (this);
    m_fontManager        = new QtFontPropertyManager       (this);
    m_pointManager       = new QtPointPropertyManager      (this);
    m_sizeManager        = new QtSizePropertyManager       (this);
    m_intManager         = new QtIntPropertyManager        (this);
    m_boolManager        = new QtBoolPropertyManager       (this);
    m_dateManager        = new QtDatePropertyManager       (this);
    m_enumManager        = new QtEnumPropertyManager       (this);
    m_checkableEnumManager = new QtCheckableEnumPropertyManager(this);
    m_groupManager       = new QtGroupPropertyManager      (this);
    m_keySequenceManager = new QtKeySequencePropertyManager(this);
    m_filepathManager    = new QtFilePropertyManager       (this);
    m_rectManager        = new QtRectPropertyManager       (this);
    m_flagManager        = new QtFlagPropertyManager       (this);
    m_dialogManager      = new QtDialogPropertyManager     (this);
    m_rangeManager       = new QtRangePropertyManager      (this);
    m_rangeFManager      = new QtRangeFPropertyManager     (this);

    QObject::connect(m_doubleManager,      SIGNAL(valueChanged(QtProperty *, double)),
        this,    SLOT(onValueChanged(QtProperty *, double)));
    QObject::connect(m_stringManager,      SIGNAL(valueChanged(QtProperty *, QString)),
        this,    SLOT(onValueChanged(QtProperty *, QString)));
//    QObject::connect(m_stringManager,      SIGNAL(valueChangedFinished(QtProperty *, QString)),
//        this,    SLOT(onValueEditFinished(QtProperty*,QString)));
    QObject::connect(m_colorManager,       SIGNAL(valueChanged(QtProperty *, QColor)),
        this,    SLOT(onValueChanged(QtProperty *, QColor)));
    QObject::connect(m_fontManager,        SIGNAL(valueChanged(QtProperty *, QFont)),
        this,    SLOT(onValueChanged(QtProperty *, QFont)));
    QObject::connect(m_pointManager,       SIGNAL(valueChanged(QtProperty *, QPoint)),
        this,    SLOT(onValueChanged(QtProperty *, QPoint)));
    QObject::connect(m_sizeManager,        SIGNAL(valueChanged(QtProperty *, QSize)),
        this,    SLOT(onValueChanged(QtProperty *, QSize)));
    QObject::connect(m_intManager,         SIGNAL(valueChanged(QtProperty *, int)),
        this,    SLOT(onValueChanged(QtProperty *, int)));
    QObject::connect(m_boolManager,        SIGNAL(valueChanged(QtProperty *, bool)),
        this,    SLOT(onValueChanged(QtProperty *, bool)));
    QObject::connect(m_dateManager,        SIGNAL(valueChanged(QtProperty *, QDate)),
        this,    SLOT(onValueChanged(QtProperty *, QDate)));
    QObject::connect(m_enumManager,        SIGNAL(valueChanged(QtProperty *, int)),
        this,    SLOT(onValueChanged(QtProperty *, int)));
    QObject::connect(m_checkableEnumManager, SIGNAL(valueChanged(QtProperty *, const CheckableEnumValue &)),
        this,    SLOT(onValueChanged(QtProperty *, const CheckableEnumValue &)));
    QObject::connect(m_keySequenceManager, SIGNAL(valueChanged(QtProperty *, QKeySequence)),
        this,    SLOT(onValueChanged(QtProperty *, QKeySequence)));
    QObject::connect(m_filepathManager,    SIGNAL(valueChanged(QtProperty *, QString)),
        this,    SLOT(onValueChanged(QtProperty *, QString)));
    QObject::connect(m_rectManager,        SIGNAL(valueChanged(QtProperty *, QRect)),
        this,    SLOT(onValueChanged(QtProperty *, QRect)));
    QObject::connect(m_flagManager,        SIGNAL(valueChanged(QtProperty *, int)),
        this,    SLOT(onValueChanged(QtProperty *, int)));
    QObject::connect(m_dialogManager,        SIGNAL(valueChanged(QtProperty *, QVariant)),
        this,    SLOT(onValueChanged(QtProperty *, QVariant)));
    QObject::connect(m_rangeManager,        SIGNAL(valueChanged(QtProperty *, const Range &)),
        this,    SLOT(onValueChanged(QtProperty *, const Range &)));
    QObject::connect(m_rangeFManager,        SIGNAL(valueChanged(QtProperty *, const RangeF &)),
        this,    SLOT(onValueChanged(QtProperty *, const RangeF &)));
}

void GroupBoxPropertyBrowser::initEditFactory()
{
    m_doubleSpinBoxFactory  = new QtDoubleSpinBoxFactory    (this);
    m_checkBoxFactory       = new QtCheckBoxFactory         (this);
    m_spinBoxFactory        = new QtSpinBoxFactory          (this);
    m_lineEditFactory       = new QtLineEditFactory         (this);
    m_comboBoxFactory       = new QtEnumEditorFactory       (this);
    m_checkableComboBoxFactory = new QtCheckableEnumEditorFactory       (this);
    m_dateFactory           = new QtDateEditFactory         (this);
    m_colorFactory          = new QtColorEditorFactory      (this);
    m_keySequenceFactory    = new QtKeySequenceEditorFactory(this);
    m_fileEditFactory       = new QtFileEditFactory         (this);
    m_dialogFactory         = new QtDialogFactory           (this);
    m_rangeFactory          = new QtRangeFactory           (this);
    m_rangeFFactory         = new QtRangeFFactory           (this);
}

void GroupBoxPropertyBrowser::setManagerEditor()
{
    setFactoryForManager(m_doubleManager,                               m_doubleSpinBoxFactory);
    setFactoryForManager(m_stringManager,                               m_lineEditFactory);
    setFactoryForManager(m_colorManager->subIntPropertyManager(),       m_spinBoxFactory);
    setFactoryForManager(m_colorManager,                                m_colorFactory);
    setFactoryForManager(m_fontManager->subIntPropertyManager(),        m_spinBoxFactory);
    setFactoryForManager(m_fontManager->subBoolPropertyManager(),       m_checkBoxFactory);
    setFactoryForManager(m_fontManager->subEnumPropertyManager(),       m_comboBoxFactory);
    setFactoryForManager(m_pointManager->subIntPropertyManager(),       m_spinBoxFactory);
    setFactoryForManager(m_sizeManager->subIntPropertyManager(),        m_spinBoxFactory);
    setFactoryForManager(m_rectManager->subIntPropertyManager(),        m_spinBoxFactory);
    setFactoryForManager(m_intManager,                                  m_spinBoxFactory);
    setFactoryForManager(m_boolManager,                                 m_checkBoxFactory);
    setFactoryForManager(m_dateManager,                                 m_dateFactory);
    setFactoryForManager(m_enumManager,                                 m_comboBoxFactory);
    setFactoryForManager(m_checkableEnumManager,                        m_checkableComboBoxFactory);
    setFactoryForManager(m_keySequenceManager,                          m_keySequenceFactory);
    setFactoryForManager(m_filepathManager,                             m_fileEditFactory);
    setFactoryForManager(m_flagManager->subBoolPropertyManager(),       m_checkBoxFactory);
    setFactoryForManager(m_dialogManager,                               m_dialogFactory);
    setFactoryForManager(m_rangeManager,                                m_rangeFactory);
    setFactoryForManager(m_rangeFManager,                               m_rangeFFactory);
}

QtProperty *GroupBoxPropertyBrowser::createProperty(const QMap<QString, QVariant> &property)
{
    QtProperty *item = nullptr;
    if (property.isEmpty()) {
        return item;
    }
    QString propertyKey(property.value(PropertyConstants::PROPERTY_NAME).toString());
    QVariant value = propertyData.propertyValues.value(propertyKey, property.value(PropertyConstants::PROPERTY_VALUE));

    QVariantMap attributes(property);
    for (ConditionData condition : propertyData.targetPropertyConditions.values(propertyKey)) {
        if (testExpression(condition.expression, propertyData.propertyValues)) {
            bool visible = condition.attributes.value(PropertyConstants::PROPERTY_VISIBLE, true).toBool();
            if (!visible) {
                return nullptr;
            }

            for (auto it = condition.attributes.constBegin(); it != condition.attributes.constEnd(); ++it) {
                attributes.insert(it.key(), it.value());
            }
        }
    }

    QString showText(attributes.value(PropertyConstants::PROPERTY_SHOW_TEXT).toString());
    int type = attributes.value(PropertyConstants::PROPERTY_VALUE_TYPE).toInt();
    if (attributes.value(PropertyConstants::PROPERTY_VALUE_TYPE).type() != QVariant::Int) {
        type = PropertyConstants::PropertyNameMap.value(attributes.value(PropertyConstants::PROPERTY_VALUE_TYPE).toString());
    }

    switch(type)
    {
    case PROPERTY_TYPE_BOOL:
    {
        item = m_boolManager->addProperty(showText);
        m_boolManager->setValue(item, value.toBool());
        item = addAssociateProperty(item, attributes.value(PropertyConstants::PROPERTY_ASSOCIATE).toList());
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        if(!value.toBool())
            setSubItemEditable(item, false);
        break;
    }
    case PROPERTY_TYPE_INT:
    {
        item = m_intManager->addProperty(showText);
        m_intManager->setValue(item, value.toInt());

        QVariant min(attributes.value(PropertyConstants::PROPERTY_VALUE_MIN));
        QVariant max(attributes.value(PropertyConstants::PROPERTY_VALUE_MAX));
        m_intManager->setMinimum(item, min.isNull() ? -2147483648 : min.toInt());
        m_intManager->setMaximum(item, max.isNull() ? 2147483647 : max.toInt());
        m_intManager->setIntegerBase(item, attributes.value(PropertyConstants::PROPERTY_INTEGERBASE, 10).toInt());
        m_intManager->setReadOnly(item, !attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_intManager->setSingleStep(item, attributes.value(PropertyConstants::PROPERTY_SINGLESTEP, QVariant::fromValue(1)).toInt());
        m_intManager->setSuffix(item, attributes.value(PropertyConstants::PROPERTY_SUFFIX).toString());
        break;
    }
    case PROPERTY_TYPE_DOUBLE:
    {
        item = m_doubleManager->addProperty(showText);
        m_doubleManager->setDecimals(item, attributes.value(PropertyConstants::PROPERTY_DECIMALS, QVariant::fromValue(2)).toInt());

        m_doubleManager->setValue(item, value.toDouble());
        QVariant min(attributes.value(PropertyConstants::PROPERTY_VALUE_MIN));
        QVariant max(attributes.value(PropertyConstants::PROPERTY_VALUE_MAX));

        m_doubleManager->setMinimum(item, min.isNull() ? FLT_MIN : min.toDouble());
        m_doubleManager->setMaximum(item, max.isNull() ? FLT_MAX : max.toDouble());

        m_doubleManager->setSuffix(item, attributes.value(PropertyConstants::PROPERTY_SUFFIX).toString());
        m_doubleManager->setReadOnly(item, !attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_doubleManager->setSingleStep(item, attributes.value(PropertyConstants::PROPERTY_SINGLESTEP, QVariant::fromValue(1.0)).toDouble());
        break;
    }
    case PROPERTY_TYPE_STRING:
    {
        item = m_stringManager->addProperty(showText);
        m_stringManager->setValue(item, value.toString());
        QString regexp(attributes.value(PropertyConstants::PROPERTY_STRING_REGEXP).toString().trimmed());
        QRegExp rx(regexp);
        if (!regexp.isEmpty() && rx.isValid())
            m_stringManager->setRegExp(item, rx);
        m_stringManager->setEchoMode(item, static_cast<QLineEdit::EchoMode> (attributes.value(PropertyConstants::PROPERTY_STRING_ECHOMODE).toInt()));
        m_stringManager->setReadOnly(item, !attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        break;
    }
    case PROPERTY_TYPE_FLAG:
    {
        item = m_flagManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        QVariantList values(attributes.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList flagNames;
        if (values.value(0).canConvert(QVariant::Hash))
        {
            for(int idx = 0; idx < values.size(); ++idx)
            {
                QVariantMap d(values.at(idx).toMap());
                flagNames.append(d.value("name").toString());
            }
        }
        else
        {
            for(int idx = 0; idx < values.size(); ++idx)
                flagNames.append(values.at(idx).toString());
        }

        m_flagManager->setFlagNames(item, flagNames);
        m_flagManager->setValue(item, value.toInt());
        break;
    }
    case PROPERTY_TYPE_ENUM:
    {
        item = m_enumManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        QVariantList values(attributes.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList  enumNames;
        QVariantList enumKeys;
        QMap<int, QIcon> enumIcons;

        int index = 0;
        if (values.value(0).type() == QVariant::Map)
        {
            for(int idx = 0; idx < values.size(); ++idx)
            {
                QVariantMap d(values.at(idx).toMap());
                enumNames.append(d.value("name").toString());
                enumKeys.append(d.value("key"));
                if (d.contains("icon"))
                    enumIcons.insert(idx, QIcon(d.value("icon").toString()));
            }
            index = enumKeys.indexOf(value.toString());
        }
        else
        {
            for(int idx = 0; idx < values.size(); ++idx) {
                enumNames.append(values.at(idx).toString());
            }
            index = value.toInt();
        }

        m_enumManager->setEnumkeys(item, enumKeys);
        m_enumManager->setEnumNames(item, enumNames);

        if (enumIcons.count() == enumNames.count() && enumIcons.count() > 0)
            m_enumManager->setEnumIcons(item, enumIcons);

        m_enumManager->setValue(item, index);
        break;
    }
#if 1
    case PROPERTY_TYPE_CHECKABLE_ENUM:
    {
        item = m_checkableEnumManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        QVariantList values(attributes.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList  enumNames;
        QVariantList enumKeys;

        bool hasKeys = values.value(0).type() == QVariant::Map;
        if (hasKeys) {
            for(int idx = 0; idx < values.size(); ++idx) {
                QVariantMap d(values.at(idx).toMap());
                enumNames.append(d.value("name").toString());
                enumKeys.append(d.value("key"));
            }
        } else {
            for(int idx = 0; idx < values.size(); ++idx) {
                enumKeys.append(values.at(idx).toString());
                enumNames.append(values.at(idx).toString());
            }
        }

        m_checkableEnumManager->setEnumkeys(item, enumKeys);
        m_checkableEnumManager->setEnumNames(item, enumNames);

        if (value.type() == qMetaTypeId<CheckableEnumValue>()) {
            m_checkableEnumManager->setValue(item, value.value<CheckableEnumValue>());
        } else {
            QList<int> indexes;
            QList<int> keys;
            bool isIndex = true;
            if (value.type() == QVariant::List) {
                for (auto idx : value.toList()) {
                    if (idx.type() == QVariant::Int) {
                        indexes << idx.toInt();
                        keys << (hasKeys ? enumKeys.indexOf(idx) : enumNames.indexOf(idx.toString()));
                        if (idx.toInt() < 0 || idx >= enumKeys.size()) {
                            isIndex = false;
                        }
                    } else {
                        indexes << (hasKeys ? enumKeys.indexOf(idx) : enumNames.indexOf(idx.toString()));
                    }
                }
            }
            m_checkableEnumManager->setValue(item, isIndex ? indexes : keys);
        }
        break;
    }
    case PROPERTY_TYPE_RANGE: {
        item = m_rangeManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        Range range = value.value<Range>();
        if(value.canConvert(QMetaType::QString))
        {
            range = Range::fromString(value.toString());
        }
        m_rangeManager->setValue(item, range);
        break;
    }
    case PROPERTY_TYPE_RANGEF: {
        item = m_rangeFManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        RangeF range = value.value<RangeF>();
        if(value.canConvert(QMetaType::QString))
        {
            range = RangeF::fromString(value.toString());
        }
        m_rangeFManager->setValue(item, range);
        break;
    }
#endif
    case PROPERTY_TYPE_COLOR:
    {
        item = m_colorManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_colorManager->setValue(item, value.value<QColor>());
        break;
    }
    case PROPERTY_TYPE_DATE:
    {
        item = m_dateManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_dateManager->setValue(item, value.toDate());
        break;
    }
    case PROPERTY_TYPE_POINT:
    {
        item = m_pointManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_pointManager->setValue(item, value.toPoint());
        break;
    }
    case PROPERTY_TYPE_SIZE:
    {
        item = m_sizeManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_sizeManager->setValue(item, value.toSize());
        m_sizeManager->setMinimum(item, attributes.value(PropertyConstants::PROPERTY_VALUE_MIN).value<QSize>());
        m_sizeManager->setMaximum(item, attributes.value(PropertyConstants::PROPERTY_VALUE_MAX).value<QSize>());
        break;
    }
    case PROPERTY_TYPE_RECT:
    {
        item = m_rectManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_rectManager->setValue(item, value.toRect());
        break;
    }
    case PROPERTY_TYPE_FONT:
    {
        item = m_fontManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_fontManager->setValue(item, value.value<QFont>());
        break;
    }
    case PROPERTY_TYPE_GROUP:
    {
        QList<QVariant> groupProperty(value.toList());
        item = addGroupProperty(showText, groupProperty);
        break;
    }
    case PROPERTY_TYPE_KEYSEQUENCE:
    {
        item = m_keySequenceManager->addProperty(showText);
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_keySequenceManager->setValue(item, value.value<QString>());
        break;
    }
    case PROPERTY_TYPE_ENUM_ICON:
    {
        item = m_enumManager->addProperty(showText);
        QVariantList values(attributes.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList enumNames;
        QVariantList enumKeys;
        QMap<int, QIcon> enumIcons;
        int cnter = 0;
        for (const QVariant &enumItem : values) {
            if (enumItem.canConvert(QVariant::Map)) {
                QVariantMap map(enumItem.toMap());
                enumIcons[cnter++] = QIcon(map.value("icon").toString());
                enumNames << map.value("name").toString();
                enumKeys << map.value("key");
            }
        }
        item->setEnabled(attributes.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_enumManager->setEnumkeys(item,  enumKeys);
        m_enumManager->setEnumNames(item, enumNames);
        m_enumManager->setEnumIcons(item, enumIcons);
        m_enumManager->setValue(item, value.toInt());
        break;
    }
    case PROPERTY_TYPE_DIALOG: {
        item = m_dialogManager->addProperty(showText);
        m_dialogManager->setClassName(item,attributes.value(PropertyConstants::PROPERTY_DIALOG_CLASSNAME).toString());
        if(value.canConvert(QMetaType::QString))
        {
            value = value.toString().split(",");
        }
        m_dialogManager->setValue(item, value);
    }
    default:
        break;
    }

    if(item == nullptr)
        return item;

    propertyData.propertyNameToProperty.insert(propertyKey, item);
    propertyData.propertyTypeToProperties.insert(type, item);

    item->setPropertyKey(propertyKey);
    item->setToolTip(attributes.value(PropertyConstants::PROPERTY_TOOLTIP).toString());
    item->setStatusTip(attributes.value(PropertyConstants::PROPERTY_STATUSTIP).toString());
    item->setWhatsThis(attributes.value(PropertyConstants::PROPERTY_WHATSTHIS).toString());

    if (attributes.value(PropertyConstants::PROPERTY_VALUE_TYPE).toInt() == PROPERTY_TYPE_GROUP)
    {
        for (const QVariant &child : attributes.value(PropertyConstants::PROPERTY_SUB_ITEMS).toList())
        {
            QtProperty *childItem = createProperty(child.toMap());
            if (childItem != nullptr)
                item->addSubProperty(childItem);
        }
    }

    return item;
}

QtProperty *GroupBoxPropertyBrowser::addAssociateProperty(QtProperty *item, const QList<QVariant> &subItems)
{
    for(int idx = 0; idx < subItems.size(); ++idx)
    {
        QtProperty *itemTemp = createProperty(subItems.at(idx).toMap());
        item->addSubProperty(itemTemp);
    }

    return item;
}

void GroupBoxPropertyBrowser::setSubItemEditable(QtProperty *item, const bool &editable)
{
    QList<QtProperty* > subItems = item->subProperties();
    for(int idx = 0; idx < subItems.size(); ++idx)
        subItems.at(idx)->setEnabled(editable);
}

void GroupBoxPropertyBrowser::addSubProperty(QtProperty *parentItem, const QList<QVariant> &subItems)
{
    for(int idx = 0; idx < subItems.size(); ++idx)
    {
        QMap<QString, QVariant> item = subItems.at(idx).toMap();
        QtProperty *propertyItem = createProperty(item);
        parentItem->addSubProperty(propertyItem);
    }
}

QtProperty *GroupBoxPropertyBrowser::addGroupProperty(const QString &propertyName, const QList<QVariant> &groupProperty)
{
    QtProperty *groupItem = m_groupManager->addProperty(propertyName);
    for(int idx = 0; idx < groupProperty.size(); ++idx)
    {
        QMap<QString, QVariant> item = groupProperty.at(idx).toMap();
        QtProperty *propertyItem = createProperty(item);
        groupItem->addSubProperty(propertyItem);
    }

    return groupItem;
}

bool GroupBoxPropertyBrowser::testExpression(const QString &expression, const QVariantMap &compareValues) const
{
    if (expression.trimmed().isEmpty()) {
        return true;
    }

    for (auto sub : expression.split("||", QString::SkipEmptyParts)) {
        if (sub.contains("&&")) {
            bool subRet = true;
            for (auto item : sub.split("&&", QString::SkipEmptyParts)) {
                if (!testCompare(item, compareValues)) {
                    subRet = false;
                    break;
                }
            }

            if (subRet) {
                return true;
            }
        }
        else {
            if (testCompare(sub, compareValues)) {
                return true;
            }
        }
    }

    return false;
}

bool GroupBoxPropertyBrowser::testCompare(const QString &compare, const QVariantMap &compareValues) const
{
    QRegularExpression re("'(?<property>\\w+)'(?<compareType>.+)'(?<value>\\w+)'");
    QRegularExpressionMatch match = re.match(compare);
    if (match.hasMatch()) {
        QString property = match.captured("property");
        QString compareType = match.captured("compareType").trimmed();
        QString value = match.captured("value");
        QVariant::Type dataType = compareValues.value(property).type();
        if (dataType == QVariant::Invalid) {
            QRegExp rx("^\\d+$");
            if (rx.exactMatch(value.trimmed())) {
                dataType = QVariant::Int;
            }
            else
            {
                QDoubleValidator doubleValidator;
                QString txt(value);
                int pos = 0;
                if (doubleValidator.validate(txt, pos) == QDoubleValidator::Acceptable) {
                    dataType = QVariant::Double;
                }
            }
        }

        switch (CompareNames.value(compareType)) {
        case Equal: {
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property, 0).toInt() == value.toInt();
            case QVariant::Double:
                return qFuzzyCompare(compareValues.value(property, 0.0).toFloat(), value.toFloat());
            default: break;
            }
            return compareValues.value(property).toString().compare(value, Qt::CaseInsensitive) == 0;
        }

        case NotEqual: {
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property).toInt() != value.toInt();
            case QVariant::Double:
                return !qFuzzyCompare(compareValues.value(property).toFloat(), value.toFloat());
            default: break;
            }
            return compareValues.value(property).toString().compare(value, Qt::CaseInsensitive) != 0;
        }

        case MoreThan: {
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property).toInt() > value.toInt();
            case QVariant::Double:
                return compareValues.value(property).toFloat() > value.toFloat();
            default: break;
            }
            return compareValues.value(property).toString() > value;
        }
        case MoreThanOrEqual: {
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property).toInt() >= value.toInt();
            case QVariant::Double:
                return compareValues.value(property).toFloat() >= value.toFloat();
            default: break;
            }
            return compareValues.value(property).toString() >= value;
        }
        case LessThan: {
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property).toInt() < value.toInt();
            case QVariant::Double:
                return compareValues.value(property).toFloat() < value.toFloat();
            default: break;
            }
            return compareValues.value(property).toString() < value;
        }
        case LessThanOrEqual:{
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property).toInt() <= value.toInt();
            case QVariant::Double:
                return compareValues.value(property).toFloat() <= value.toFloat();
            default: break;
            }
            return compareValues.value(property).toString() <= value;
        }
        default:  return false;
        }
    }

    return false;
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, bool value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}


void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, int value)
{
    if(property->propertyKey().isEmpty())
        return;

    if (sender() == m_enumManager
        && m_enumManager->enumKeys(property).size() == m_enumManager->enumNames(property).size()) {
        onPropertyChanged(property->propertyKey(), m_enumManager->enumKeys(property).value(value));
    } else {
        onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
    }
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, double value)
{
    if(property->propertyKey().isEmpty())
        return;
    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QString value)
{
    if(property->propertyKey().isEmpty())
        return;
    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QColor value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QFont value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QPoint value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QSize value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QDate value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QVariant value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), value);
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QKeySequence value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), value);
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, QRect value)
{
    if (property->propertyKey().isEmpty())
        return ;

    onPropertyChanged(property->propertyKey(), value);
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, const QVariantList & value)
{
    qDebug() << Q_FUNC_INFO << property->propertyKey() << value;
    if (property->propertyKey().isEmpty())
        return ;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, const Range &value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, const CheckableEnumValue & value)
{
    if (property->propertyKey().isEmpty())
        return ;
    QVariantList keys = m_checkableEnumManager->keys(property);
    if (!keys.isEmpty()) {
        onPropertyChanged(property->propertyKey(), QVariant::fromValue(keys));
    } else {
        onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
    }
}

void GroupBoxPropertyBrowser::onValueChanged(QtProperty *property, const RangeF &value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void GroupBoxPropertyBrowser::onValueEditFinished(QtProperty *property, const QString &value)
{
    if(property->propertyKey().isEmpty())
        return;

    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}
