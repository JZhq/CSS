#include "treepropertybrowser.h"
#include <QTranslator>
#include <QCoreApplication>
#include <QDebug>
#include <QCursor>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDoubleValidator>
#include <QTimer>
#include "editwindow.h"
#include "popupwindow.h"
#include "propertyconstants.h"

const QMap<QString, TreePropertyBrowser::CompareType> TreePropertyBrowser::CompareNames = {
    { "=",  TreePropertyBrowser::Equal },
    { "==", TreePropertyBrowser::Equal },
    { ">",  TreePropertyBrowser::MoreThan },
    { ">=", TreePropertyBrowser::MoreThanOrEqual },
    { "<",  TreePropertyBrowser::LessThan },
    { "<=", TreePropertyBrowser::LessThanOrEqual },
    { "!=", TreePropertyBrowser::NotEqual },
};

TreePropertyBrowser::TreePropertyBrowser(QWidget *parent) :
    QtTreePropertyBrowser(parent)
{
    setRootIsDecorated(true);
    initEditFactory();
    initManager();
    setManagerEditor();
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));
    //获取故障树节点属性列表控件信号 by zyg
    connect(this, SIGNAL(selectCurrentItem(QtBrowserItem*)), this, SLOT(slotSelectCurrentItem(QtBrowserItem*)));

    menu = new QMenu(this);
    editAction = new QAction(QStringLiteral("编辑"), this);
    connect(editAction,SIGNAL(triggered(bool)),this,SLOT(showEditWindows()));
    menu->addAction(editAction);
}

TreePropertyBrowser::~TreePropertyBrowser()
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
    unsetFactoryForManager(m_textManager);
	unsetFactoryForManager(m_dialogManager);
}

void TreePropertyBrowser::showProperties(const QVariantList &properties)
{
    //准备修改属性描述信息 by zyg
    if(!properties.isEmpty())
    {
        propertyNameToQtProperty.clear();
        clear();
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


        m_properties = properties;
        //        m_currentVariableName.clear();

        QVariantHash t_propertyData(properties.at(0).toHash());
        if (properties.at(0).type() == QVariant::Map) {
            QVariantMap map = properties.at(0).toMap();
            for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
                t_propertyData[it.key()] = it.value();
            }
        }

        QtProperty *property = createProperty(t_propertyData);
        QString t_propertyKey = property->propertyKey();
        QString t_propertyValue = property->valueText();
        emit selectTreeProperty(t_propertyKey, t_propertyValue);

        for(int idx = 0; idx < properties.size(); ++idx)
        {
            QVariantHash propertyData;
            if (properties.at(idx).type() == QVariant::Hash) {
                propertyData = properties.at(idx).toHash();
            }
            else {
                QVariantMap map = properties.at(idx).toMap();
                for (auto it=map.constBegin(); it != map.constEnd(); ++it) {
                    propertyData[it.key()] = it.value();
                }
            }

            QtProperty *property = createProperty(propertyData);
            QtBrowserItem *item = addProperty(property);
            expandAll(item, true);
        }
    }
}

QtAbstractPropertyManager *TreePropertyBrowser::propertyManager(PropertyType type) const
{
    switch (type) {
    case PROPERTY_TYPE_BOOL          : return m_boolManager         ;
    case PROPERTY_TYPE_INT           : return m_intManager          ;
    case PROPERTY_TYPE_DOUBLE        : return m_doubleManager       ;
    case PROPERTY_TYPE_STRING        : return m_stringManager       ;
    case PROPERTY_TYPE_TEXTSTRING    : return m_stringManager       ;
    case PROPERTY_TYPE_ENUM          : return m_enumManager         ;
    case PROPERTY_TYPE_CHECKABLE_ENUM: return m_checkableEnumManager;
    case PROPERTY_TYPE_ENUM_ICON     : return m_enumManager         ;
    case PROPERTY_TYPE_COLOR         : return m_colorManager        ;
    case PROPERTY_TYPE_DATE          : return m_dateManager         ;
    case PROPERTY_TYPE_POINT         : return m_pointManager        ;
    case PROPERTY_TYPE_SIZE          : return m_sizeManager         ;
    case PROPERTY_TYPE_RECT          : return m_rectManager         ;
    case PROPERTY_TYPE_FILESELECT    : return m_filepathManager     ;
    case PROPERTY_TYPE_FONT          : return m_fontManager         ;
    case PROPERTY_TYPE_KEYSEQUENCE   : return m_keySequenceManager  ;
    case PROPERTY_TYPE_DIALOG        : return m_dialogManager       ;
    default: break;
    }
    return nullptr;
}

QtProperty *TreePropertyBrowser::property(const QString &name) const
{
    return propertyNameToQtProperty.value(name, nullptr);
}

void TreePropertyBrowser::onPropertyChanged(const QString &property, const QVariant &value)
{
    propertyData.propertyValues[property] = value;
    if (signalsBlocked()) {
        return ;
    }

    emit propertyChanged(property, value);

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
            emit showPropertiesSignal();
        });
    }
}

QList<QtProperty*> TreePropertyBrowser::addProperties(const QVariantList &properties)
{
    QList<QtProperty*> listProperty;
    if(!properties.isEmpty())
    {
        for(int idx = 0; idx < properties.size(); ++idx)
        {
            QVariantHash propertyData;
            if (properties.at(idx).type() == QVariant::Hash) {
                propertyData = properties.at(idx).toHash();
            }
            else {
                QVariantMap map = properties.at(idx).toMap();
                for (auto it=map.constBegin(); it != map.constEnd(); ++it) {
                    propertyData[it.key()] = it.value();
                }
            }

            QtProperty *property = createProperty(propertyData);
            QtBrowserItem *item = addProperty(property);
            listProperty.append(property);
            expandAll(item, true);
        }
    }
    return listProperty;
}

void TreePropertyBrowser::updateProperties(const QVariantList &properties)
{
    if (!properties.isEmpty())
    {
        for (int idx = 0; idx < properties.size(); ++idx)
        {
            QVariantHash property;
            if (properties.at(idx).type() == QVariant::Hash) {
                property = properties.at(idx).toHash();
            }
            else {
                QVariantMap map = properties.at(idx).toMap();
                for (auto it=map.constBegin(); it != map.constEnd(); ++it) {
                    property[it.key()] = it.value();
                }
            }
            if (property.isEmpty())
                continue;

            updateProperty(NULL, property);
        }
    }
//    if(menu!=Q_NULLPTR)
//    {
//        delete menu;
//    }

//    if(editAction!=Q_NULLPTR)
//    {
//        delete editAction;
//    }
}


void TreePropertyBrowser::updatePropertiesValue(const QVariantMap &values)
{
    if (values.isEmpty()) {
        return ;
    }
#if 1
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        QtProperty *property = propertyData.propertyNameToProperty.value(it.key(), nullptr);

        if (property == nullptr) {
            continue;
        }

        switch (propertyData.propertyTypeToProperties.key(property, PROPERTY_TYPE_INVALID)) {
//        case PROPERTY_TYPE_CHAR          : m_charManager->setValue(property, it.value().toChar()); break;
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
//        case PROPERTY_TYPE_FLAG          : m_flagManager->setValue(property, it.value().toInt()); break;
        case PROPERTY_TYPE_COLOR         : m_colorManager->setValue(property, it.value().value<QColor>()); break;
        case PROPERTY_TYPE_DATE          : m_dateManager->setValue(property, it.value().toDate()); break;
//        case PROPERTY_TYPE_DATETIME      : m_dateTimeManager->setValue(property, it.value().toDateTime()); break;
//        case PROPERTY_TYPE_TIME          : m_timeManager->setValue(property, it.value().toTime()); break;
        case PROPERTY_TYPE_POINT         : m_pointManager->setValue(property, it.value().toPoint()); break;
        case PROPERTY_TYPE_SIZE          : m_sizeManager->setValue(property, it.value().toSize()); break;
        case PROPERTY_TYPE_RECT          : m_rectManager->setValue(property, it.value().toRect()); break;
        case PROPERTY_TYPE_FILESELECT    : m_filepathManager->setValue(property, it.value().toString()); break;
        case PROPERTY_TYPE_FONT          : m_fontManager->setValue(property, it.value().value<QFont>()); break;
        case PROPERTY_TYPE_KEYSEQUENCE   : m_keySequenceManager->setValue(property, it.value().value<QKeySequence>()); break;
//        case PROPERTY_TYPE_SIZEPOLICY    : m_sizePolicyManager->setValue(property, it.value().value<QSizePolicy>()); break;
        case PROPERTY_TYPE_DIALOG        : m_dialogManager->setValue(property, it.value()); break;
        }
    }
#else
    QVariantList properties;
    for(auto property : propertyData.properties) {
        QVariantMap propMap(property.toMap());
        if (values.contains(propMap.value(PropertyConstants::PROPERTY_NAME).toString())) {
            propMap[PropertyConstants::PROPERTY_VALUE] = values.value(propMap.value(PropertyConstants::PROPERTY_NAME).toString());
        }
        properties << propMap;
    }

    QTimer::singleShot(0, [=](){ showProperties(properties); });
#endif
}

void TreePropertyBrowser::updateOneProperty(const QVariantHash &property)
{
    if (!property.isEmpty())
    {
        updateProperty(NULL, property);
    }
}

void TreePropertyBrowser::setFriendVariableName(const QStringList &variableName)
{
    m_variableNamelist = variableName;
}

void TreePropertyBrowser::setAllFuncList(const QList<QString> &funcs)
{
    m_allFunctionNameList = funcs;
    m_textEditFactory->setAllFuncName(m_allFunctionNameList);
}

void TreePropertyBrowser::initManager()
{
    m_doubleManager      = new QtDoublePropertyManager(this);
    m_stringManager      = new QtStringPropertyManager(this);
    m_colorManager       = new QtColorPropertyManager(this);
    m_fontManager        = new QtFontPropertyManager(this);
    m_pointManager       = new QtPointPropertyManager(this);
    m_sizeManager        = new QtSizePropertyManager(this);
    m_intManager         = new QtIntPropertyManager(this);
    m_boolManager        = new QtBoolPropertyManager(this);
    m_dateManager        = new QtDatePropertyManager(this);
    m_enumManager        = new QtEnumPropertyManager(this);
    m_checkableEnumManager = new QtCheckableEnumPropertyManager(this);
    m_groupManager       = new QtGroupPropertyManager(this);
    m_keySequenceManager = new QtKeySequencePropertyManager(this);
    m_filepathManager    = new QtFilePropertyManager(this);
    m_rectManager        = new QtRectPropertyManager(this);
    m_textManager        = new CustomStringTextPropertyManager(this);
    m_dialogManager      = new QtDialogPropertyManager       (this);
    m_rangeManager       = new QtRangePropertyManager      (this);
    m_rangeFManager      = new QtRangeFPropertyManager     (this);

    connect(m_doubleManager, SIGNAL(valueChanged(QtProperty *, double)),
            this, SLOT(onValueChanged(QtProperty *, double)));
    connect(m_stringManager, SIGNAL(valueChanged(QtProperty *, QString)),
            this, SLOT(onValueChanged(QtProperty *, QString)));
    connect(m_colorManager, SIGNAL(valueChanged(QtProperty *, QColor)),
            this, SLOT(onValueChanged(QtProperty *, QColor)));
    connect(m_fontManager, SIGNAL(valueChanged(QtProperty *, QFont)),
            this, SLOT(onValueChanged(QtProperty *, QFont)));
    connect(m_pointManager, SIGNAL(valueChanged(QtProperty *, QPoint)),
            this, SLOT(onValueChanged(QtProperty *, QPoint)));
    connect(m_sizeManager, SIGNAL(valueChanged(QtProperty *, QSize)),
            this, SLOT(onValueChanged(QtProperty *, QSize)));
    connect(m_intManager, SIGNAL(valueChanged(QtProperty *, int)),
            this, SLOT(onValueChanged(QtProperty *, int)));
    connect(m_boolManager, SIGNAL(valueChanged(QtProperty *, bool)),
            this, SLOT(onValueChanged(QtProperty *, bool)));
    connect(m_dateManager, SIGNAL(valueChanged(QtProperty *, QDate)),
            this, SLOT(onValueChanged(QtProperty *, QDate)));
    connect(m_enumManager, SIGNAL(valueChanged(QtProperty *, int)),
            this, SLOT(onValueChanged(QtProperty *, int)));
//    connect(m_enumManager, SIGNAL(valueChanged(QtProperty *, QString)),
//            this, SLOT(onValueChanged(QtProperty *, QString)));
    connect(m_keySequenceManager, SIGNAL(valueChanged(QtProperty*,QKeySequence)),
            this, SLOT(onValueChanged(QtProperty*,QKeySequence)));
    connect(m_filepathManager, SIGNAL(valueChanged(QtProperty *, QString)),
            this, SLOT(onValueChanged(QtProperty *, QString)));
    connect(m_rectManager, SIGNAL(valueChanged(QtProperty *, QRect)),
            this, SLOT(onVlaueChanged(QtProperty *, QRect)));

    connect(m_textManager, SIGNAL(valueChanged(QtProperty *, QString)),
            this, SLOT(onValueChanged(QtProperty *, QString)));
    connect(m_checkableEnumManager, SIGNAL(valueChanged(QtProperty *, const CheckableEnumValue &)),
        this,    SLOT(onValueChanged(QtProperty *, const CheckableEnumValue &)));
	connect(m_dialogManager,        SIGNAL(valueChanged(QtProperty *, QVariant)),
        this,    SLOT(onValueChanged(QtProperty *, QVariant)));
    connect(m_rangeManager,        SIGNAL(valueChanged(QtProperty *, const Range &)),
        this,    SLOT(onValueChanged(QtProperty *, const Range &)));
    connect(m_rangeFManager,        SIGNAL(valueChanged(QtProperty *, const RangeF &)),
        this,    SLOT(onValueChanged(QtProperty *, const RangeF &)));

}

void TreePropertyBrowser::initEditFactory()
{
    m_doubleSpinBoxFactory  = new QtDoubleSpinBoxFactory(this);
    m_checkBoxFactory       = new QtCheckBoxFactory(this);
    m_spinBoxFactory        = new QtSpinBoxFactory(this);
    m_lineEditFactory       = new QtLineEditFactory(this);
    m_comboBoxFactory       = new QtEnumEditorFactory(this);
    m_checkableComboBoxFactory = new QtCheckableEnumEditorFactory       (this);
    m_dateFactory           = new QtDateEditFactory(this);
    m_colorFactory          = new QtColorEditorFactory(this);
    m_keySequenceFactory    = new QtKeySequenceEditorFactory(this);
    m_fileEditFactory       = new QtFileEditFactory(this);
    m_textEditFactory       = new QtTextEditFactory(this);
    m_dialogFactory         = new QtDialogFactory           (this);
    m_rangeFactory          = new QtRangeFactory           (this);
    m_rangeFFactory         = new QtRangeFFactory           (this);
}

void TreePropertyBrowser::setManagerEditor()
{
    setFactoryForManager(m_doubleManager, m_doubleSpinBoxFactory);
    setFactoryForManager(m_stringManager, m_lineEditFactory);
    setFactoryForManager(m_colorManager->subIntPropertyManager(), m_spinBoxFactory);
    setFactoryForManager(m_colorManager, m_colorFactory);
    setFactoryForManager(m_fontManager->subIntPropertyManager(), m_spinBoxFactory);
    setFactoryForManager(m_fontManager->subBoolPropertyManager(), m_checkBoxFactory);
    setFactoryForManager(m_fontManager->subEnumPropertyManager(), m_comboBoxFactory);
    setFactoryForManager(m_pointManager->subIntPropertyManager(), m_spinBoxFactory);
    setFactoryForManager(m_sizeManager->subIntPropertyManager(), m_spinBoxFactory);
    setFactoryForManager(m_rectManager->subIntPropertyManager(), m_spinBoxFactory);
    setFactoryForManager(m_intManager, m_spinBoxFactory);
    setFactoryForManager(m_boolManager, m_checkBoxFactory);
    setFactoryForManager(m_dateManager, m_dateFactory);
    setFactoryForManager(m_enumManager, m_comboBoxFactory);
    setFactoryForManager(m_checkableEnumManager,                        m_checkableComboBoxFactory);
    setFactoryForManager(m_keySequenceManager, m_keySequenceFactory);
    setFactoryForManager(m_filepathManager, m_fileEditFactory);
    setFactoryForManager(m_dialogManager,                               m_dialogFactory);
    setFactoryForManager(m_textManager,m_textEditFactory);
    setFactoryForManager(m_rangeManager,                                m_rangeFactory);
    setFactoryForManager(m_rangeFManager,                               m_rangeFFactory);
}

void TreePropertyBrowser::expandAll(QtBrowserItem *item, bool expand)
{
    this->setExpanded(item, expand);
    for(int i = 0, iend = item->children().count(); i < iend; i++){
        this->expandAll(item->children()[i], expand);
    }
}

void TreePropertyBrowser::updateProperty(QtProperty *parent, const QVariantHash &property)
{
    QString propertyKey(property.value(PropertyConstants::PROPERTY_NAME).toString());
    QVariant value(property.value(PropertyConstants::PROPERTY_VALUE));
    QString showText(property.value(PropertyConstants::PROPERTY_SHOW_TEXT).toString());
    QtProperty *item = NULL;
    //search item
    QList<QtProperty *> items;
    if (parent == NULL){
        items = this->properties();
    }
    else{
        items = parent->subProperties();
    }

    for (int i = 0, iend = items.count(); i < iend; i++)
    {
        if(items[i]->propertyKey() == propertyKey)
        {
            item = items[i];
            break;
        }
    }

    if(item == NULL)
        return;

    switch(property.value(PropertyConstants::PROPERTY_VALUE_TYPE).toInt())
    {
    case PROPERTY_TYPE_BOOL:
    {
        m_boolManager->setValue(item, value.toBool());
        updateAssociateProperty(item, property.value(PropertyConstants::PROPERTY_ASSOCIATE).toList());

        if(!value.toBool()) {
            setSubItemEditable(item, false);
        }

        break;
    }
    case PROPERTY_TYPE_INT:
    {
        QVariant min(property.value(PropertyConstants::PROPERTY_VALUE_MIN));
        QVariant max(property.value(PropertyConstants::PROPERTY_VALUE_MAX));
        if( min.isValid() && max.isValid())
        {
            m_intManager->setMinimum(item, min.toInt());
            m_intManager->setMaximum(item, max.toInt());
        }
        m_intManager->setValue(item, value.toInt());
        m_intManager->setSingleStep(item, 1);
        break;
    }
    case PROPERTY_TYPE_DOUBLE:
    {
        QVariant min(property.value(PropertyConstants::PROPERTY_VALUE_MIN));
        QVariant max(property.value(PropertyConstants::PROPERTY_VALUE_MAX));
        if( min.isValid() && max.isValid())
        {
            m_doubleManager->setMinimum(item, min.toDouble());
            m_doubleManager->setMaximum(item, max.toDouble());
        }
        m_doubleManager->setValue(item, value.toDouble());
        m_doubleManager->setSingleStep(item, 0.1);
        m_doubleManager->setDecimals(item, 3);
        break;
    }
    case PROPERTY_TYPE_STRING:
    {
        if(propertyKey == QStringLiteral("中心频率变量"))
        {
            m_currentZxqdbl = value.toString();
        }

        if(propertyKey == QStringLiteral("信号强度变量"))
        {
            m_currentXhqdbl = value.toString();
        }
        m_stringManager->setValue(item, value.toString());
        break;
    }
    case PROPERTY_TYPE_TEXTSTRING:
    {
        m_textManager->setValue(item, value.toString());
        break;
    }
    case PROPERTY_TYPE_ENUM:
    {
        QVariantList values(property.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QVariantList enumKeys;
        QStringList enumNames;
        for(auto opt : values) {
            switch (opt.type()) {
            case QVariant::Map: {
                enumKeys << opt.toMap().value("key");
                enumNames << opt.toMap().value("name").toString();
            }break;
            case QVariant::Hash:{
                enumKeys << opt.toHash().value("key");
                enumNames << opt.toHash().value("name").toString();
            }break;
            default: {
                enumKeys << opt;
                enumNames << opt.toString();
            }
            }
        }

        m_enumManager->setEnumkeys(item, enumKeys);
        m_enumManager->setEnumNames(item, enumNames);

        if (value.type() == QVariant::String) {
            m_enumManager->setValue(item, value.toString());
        } else if (value.type() == QVariant::Int) {
            m_enumManager->setValue(item, value.toInt());
        }
        break;
    }
    case PROPERTY_TYPE_ENUM_ICON:
    {
        QVariantList values(property.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList enumNames;
        QMap<int, QIcon> enumIcons;
        int cnter = 0;
        foreach (const QVariant &enumItem, values)
        {
            QPair<QString, QString> enumPair = enumItem.value<QPair<QString, QString> > ();
            enumIcons[cnter++] = QIcon(enumPair.first);
            enumNames << enumPair.second;
        }

        m_enumManager->setEnumNames(item, enumNames);
        m_enumManager->setEnumIcons(item, enumIcons);
        m_enumManager->setValue(item, value.toInt());
        break;
    }
    case PROPERTY_TYPE_COLOR:
    {
        m_colorManager->setValue(item, value.value<QColor>());
        break;
    }
    case PROPERTY_TYPE_DATE:
    {
        m_dateManager->setValue(item, value.toDate());
        break;
    }
    case PROPERTY_TYPE_POINT:
    {
        m_pointManager->setValue(item, value.toPoint());
        break;
    }
    case PROPERTY_TYPE_SIZE:
    {
        m_sizeManager->setValue(item, value.toSize());
        break;
    }
    case PROPERTY_TYPE_RECT:
    {
        m_rectManager->setValue(item, value.toRect());
        break;
    }
    case PROPERTY_TYPE_FONT:
    {
        m_fontManager->setValue(item, value.value<QFont>());
        break;
    }
    case PROPERTY_TYPE_RANGE: {
        item = m_rangeManager->addProperty(showText);
        item->setEnabled(property.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_rangeManager->setValue(item, value.value<Range>());
        break;
    }
    case PROPERTY_TYPE_RANGEF: {
        item = m_rangeFManager->addProperty(showText);
        item->setEnabled(property.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        m_rangeFManager->setValue(item, value.value<RangeF>());
        break;
    }
    case PROPERTY_TYPE_GROUP:
    {
        QList<QVariant> groupProperty(property.value(PropertyConstants::PROPERTY_SUB_ITEMS).toList());
        updateGroupProperty(item, groupProperty);
        break;
    }
    case PROPERTY_TYPE_KEYSEQUENCE:
    {
        m_keySequenceManager->setValue(item, value.value<QString>());
        break;
    }
    case PROPERTY_TYPE_FILESELECT:
    {
        QVariantList values(property.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList filters;
        for(int idx = 0; idx < values.size(); ++idx)
            filters.append(values.at(idx).toString());

        m_filepathManager->setValue(item, value.toString());
        m_filepathManager->setFilters(item, filters);
        break;
    }
    default:
        break;
    }

    QVariant editable(property.value(PropertyConstants::PROPERTY_EDITABLE));
    if(!editable.isNull())
        item->setEnabled(editable.toBool());
}

void TreePropertyBrowser::updateProperty(QtProperty *parent, const QVariantMap &property)
{
    QVariantHash hash;
    for (auto it=property.constBegin(); it!=property.constEnd(); ++it) {
        hash[it.key()] = it.value();
    }

    updateProperty(parent, hash);
}

QtProperty *TreePropertyBrowser::createProperty(const QVariantHash &property)
{
    QtProperty *item = NULL;
    QString propertyKey(property.value(PropertyConstants::PROPERTY_NAME).toString());

    QVariantHash prop(property);
    for (ConditionData condition : propertyData.targetPropertyConditions.values(propertyKey)) {
        if (testExpression(condition.expression, propertyData.propertyValues)) {
            bool visible = condition.attributes.value(PropertyConstants::PROPERTY_VISIBLE, true).toBool();
            if (!visible) {
                return nullptr;
            }

            for (auto it = condition.attributes.constBegin(); it != condition.attributes.constEnd(); ++it) {
                prop.insert(it.key(), it.value());
            }
        }
    }

    QString showText(prop.value(PropertyConstants::PROPERTY_SHOW_TEXT).toString());
    QVariant value(prop.value(PropertyConstants::PROPERTY_VALUE));
    int type = prop.value(PropertyConstants::PROPERTY_VALUE_TYPE).toInt();
    if (prop.value(PropertyConstants::PROPERTY_VALUE_TYPE).type() != QVariant::Int) {
        type = PropertyConstants::PropertyNameMap.value(prop.value(PropertyConstants::PROPERTY_VALUE_TYPE).toString());
    }

    switch(type)
    {
    case PROPERTY_TYPE_BOOL:
    {
        item = m_boolManager->addProperty(showText);
        m_boolManager->setValue(item, value.toBool());
        item = addAssociateProperty(item, prop.value(PropertyConstants::PROPERTY_ASSOCIATE).toList());

        if(!value.toBool())
            setSubItemEditable(item, false);

        break;
    }
    case PROPERTY_TYPE_INT:
    {
        item = m_intManager->addProperty(showText);

        QVariant min(prop.value(PropertyConstants::PROPERTY_VALUE_MIN));
        QVariant max(prop.value(PropertyConstants::PROPERTY_VALUE_MAX));

        if( min.isValid() && max.isValid())
        {
            m_intManager->setMinimum(item, min.toInt());
            m_intManager->setMaximum(item, max.toInt());
        }
        m_intManager->setValue(item, value.toInt());
        m_intManager->setSingleStep(item, 1);
        break;
    }
    case PROPERTY_TYPE_DOUBLE:
    {
        item = m_doubleManager->addProperty(showText);

        QVariant min(prop.value(PropertyConstants::PROPERTY_VALUE_MIN));
        QVariant max(prop.value(PropertyConstants::PROPERTY_VALUE_MAX));

        if( min.isValid() && max.isValid())
        {
            m_doubleManager->setMinimum(item, min.toDouble());
            m_doubleManager->setMaximum(item, max.toDouble());
        }

        m_doubleManager->setValue(item, value.toDouble());
        m_doubleManager->setSingleStep(item,0.1);
        m_doubleManager->setDecimals(item, 3);
        break;
    }
    case PROPERTY_TYPE_STRING:
    {
        item = m_stringManager->addProperty(showText);

        //在这里设置正则表达式
        if(showText == QStringLiteral("变量名"))
        {
            //放开20位变量名修改    by zyg
//            m_currentVariableName.append(value.toString());
            QRegExp regExp("^[A-Za-z][A-Za-z0-9]{0,19}$");
            m_stringManager->setRegExp(item, regExp);
        } else if (propertyKey == QStringLiteral("卫星通道频率")) {
            QRegExp regExp("^(([0-9]+\\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\\.[0-9]+)|([0-9]*[1-9][0-9]*)|(^$))$");
            m_stringManager->setRegExp(item, regExp);

        } else if (propertyKey == QStringLiteral("卫星通道码速率")) {
            QRegExp regExp("^(([0-9]+\\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\\.[0-9]+)|([0-9]*[1-9][0-9]*)|(^$))$");
            m_stringManager->setRegExp(item, regExp);
        }
        if(propertyKey == QStringLiteral("中心频率变量"))
        {
            m_currentZxqdbl = value.toString();
        }

        if(propertyKey == QStringLiteral("信号强度变量"))
        {
            m_currentXhqdbl = value.toString();
        }
        m_stringManager->setValue(item, value.toString());
        break;
    }
    case PROPERTY_TYPE_TEXTSTRING:
    {
        item = m_textManager->addProperty(showText);
        m_textManager->setValue(item,value.toString());
        break;
    }
    case PROPERTY_TYPE_ENUM:
    {
        item = m_enumManager->addProperty(showText);
        QVariantList values(prop.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList enumNames;
        QVariantList enumKeys;
        for(auto opt : values) {
            if (opt.type() == QVariant::Map) {
                enumKeys.append(opt.toMap().value("key"));
                enumNames.append(opt.toMap().value("name").toString());
            }
            else if (opt.type() == QVariant::Hash){
                enumKeys.append(opt.toHash().value("key"));
                enumNames.append(opt.toHash().value("name").toString());
            }
            else {
                enumKeys.append(opt);
                enumNames.append(opt.toString());
            }
        }

        m_enumManager->setEnumkeys(item, enumKeys);
        m_enumManager->setEnumNames(item, enumNames);
        if(value.type() == QVariant::String)
        {
            m_enumManager->setValue(item, value.toString());
        }
        else if(value.type() == QVariant::Int)
        {
            m_enumManager->setValue(item, value.toInt());
        }
        break;
    }
    case PROPERTY_TYPE_ENUM_ICON:
    {
        item = m_enumManager->addProperty(showText);

        QVariantList values(prop.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList enumNames;
        QMap<int, QIcon> enumIcons;
        int cnter = 0;
        foreach (const QVariant &enumItem, values)
        {
            QPair<QString, QString> enumPair = enumItem.value<QPair<QString, QString> > ();
            enumIcons[cnter++] = QIcon(enumPair.first);
            enumNames << enumPair.second;
        }

        m_enumManager->setEnumNames(item, enumNames);
        m_enumManager->setEnumIcons(item, enumIcons);
        m_enumManager->setValue(item, value.toInt());
        break;
    }
#if 1
    case PROPERTY_TYPE_CHECKABLE_ENUM:
    {
        item = m_checkableEnumManager->addProperty(showText);
        item->setEnabled(prop.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
        QVariantList values(prop.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList  enumNames;
        QVariantList enumKeys;

        bool hasKeys = values.value(0).type() == QVariant::Map;
        if (hasKeys)
        {
            for(int idx = 0; idx < values.size(); ++idx)
            {
                QVariantMap d(values.at(idx).toMap());
                enumNames.append(d.value("name").toString());
                enumKeys.append(d.value("key"));
            }
        }
        else
        {
            for(int idx = 0; idx < values.size(); ++idx) {
                enumNames.append(values.at(idx).toString());
            }
        }

        m_checkableEnumManager->setEnumkeys(item, enumKeys);
        m_checkableEnumManager->setEnumNames(item, enumNames);

        QVariant variantVal(prop.value(PropertyConstants::PROPERTY_VALUE));
        if (variantVal.canConvert(qMetaTypeId<CheckableEnumValue>())) {
            m_checkableEnumManager->setValue(item, variantVal.value<CheckableEnumValue>());
        }
        else {
            QList<int> indexes;
            QList<int> keys;
            bool isIndex = true;
            if (variantVal.type() == QVariant::List) {
                for (auto idx : variantVal.toList()) {
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
            }else if(variantVal.type() == QVariant::String){
                int index = enumKeys.indexOf(variantVal);
                if(index >= 0)
                    indexes << index;
            }
            m_checkableEnumManager->setValue(item, isIndex ? indexes : keys);
        }
        break;
    }
    case PROPERTY_TYPE_RANGE: {
        item = m_rangeManager->addProperty(showText);
        item->setEnabled(property.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
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
        item->setEnabled(property.value(PropertyConstants::PROPERTY_EDITABLE, QVariant::fromValue(true)).toBool());
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
        m_colorManager->setValue(item, value.value<QColor>());
        break;
    }
    case PROPERTY_TYPE_DATE:
    {
        item = m_dateManager->addProperty(showText);
        m_dateManager->setValue(item, value.toDate());
        break;
    }
    case PROPERTY_TYPE_POINT:
    {
        item = m_pointManager->addProperty(showText);
        m_pointManager->setValue(item, value.toPoint());
        break;
    }
    case PROPERTY_TYPE_SIZE:
    {
        item = m_sizeManager->addProperty(showText);
        m_sizeManager->setValue(item, value.toSize());
        break;
    }
    case PROPERTY_TYPE_RECT:
    {
        item = m_rectManager->addProperty(showText);
        m_rectManager->setValue(item, value.toRect());
        break;
    }
    case PROPERTY_TYPE_FONT:
    {
        item = m_fontManager->addProperty(showText);
        m_fontManager->setValue(item, value.value<QFont>());
        break;
    }
    case PROPERTY_TYPE_GROUP:
    {
        QList<QVariant> groupProperty(prop.value(PropertyConstants::PROPERTY_SUB_ITEMS).toList());
        item = addGroupProperty(showText, groupProperty);
        break;
    }
    case PROPERTY_TYPE_KEYSEQUENCE:
    {
        item = m_keySequenceManager->addProperty(showText);
        m_keySequenceManager->setValue(item, value.value<QString>());
        break;
    }
    case PROPERTY_TYPE_FILESELECT:
    {
        QVariantList values(prop.value(PropertyConstants::PROPERTY_OPTIONS).toList());
        QStringList filters;
        for(int idx = 0; idx < values.size(); ++idx)
            filters.append(values.at(idx).toString());

        item = m_filepathManager->addProperty(showText);
        m_filepathManager->setValue(item, value.toString());
        m_filepathManager->setFilters(item, filters);
        break;
    }
    case PROPERTY_TYPE_DIALOG: {
        item = m_dialogManager->addProperty(showText);
        m_dialogManager->setClassName(item, prop.value(PropertyConstants::PROPERTY_DIALOG_CLASSNAME).toString());
        if(value.canConvert(QMetaType::QString))
        {
            value = value.toString().split(",");
        }
        m_dialogManager->setValue(item, value);
    }
    default:
        break;
    }

    if(NULL == item)
        return item;

    propertyNameToQtProperty.insert(propertyKey, item);

    QVariant editable(prop.value(PropertyConstants::PROPERTY_EDITABLE));
    if(!editable.isNull())
        item->setEnabled(editable.toBool());

    item->setPropertyKey(propertyKey);

    return item;
}

QtProperty *TreePropertyBrowser::createProperty(const QVariantMap &property)
{
    QVariantHash hash;
    for (auto it = property.constBegin(); it != property.constEnd(); ++it) {
        hash[it.key()] = it.value();
    }

    return createProperty(hash);
}

QtProperty *TreePropertyBrowser::addAssociateProperty(QtProperty *item, const QList<QVariant> &subItems)
{
    for(int idx = 0; idx < subItems.size(); ++idx)
    {
        QtProperty *itemTemp = nullptr;

        if (subItems.at(idx).type() == QVariant::Hash) {
            itemTemp = createProperty(subItems.at(idx).toHash());
        } else {
            itemTemp = createProperty(subItems.at(idx).toMap());
        }

        item->addSubProperty(itemTemp);
    }

    return item;
}

void TreePropertyBrowser::updateAssociateProperty(QtProperty *parentItem, const QList<QVariant> &subItems)
{
    if(parentItem == NULL)
        return;

    for(auto variant : subItems) {
        QVariantHash hash;
        if (variant.type() == QVariant::Hash) {
            hash = variant.toHash();
        } else {
            QVariantMap map = variant.toMap();
            for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
                hash[it.key()] = it.value();
            }
        }
        updateProperty(parentItem, hash);
    }
}

void TreePropertyBrowser::setSubItemEditable(QtProperty *item, const bool &editable)
{
    QList<QtProperty* > subItems = item->subProperties();
    for(int idx = 0; idx < subItems.size(); ++idx)
        subItems.at(idx)->setEnabled(editable);
}

void TreePropertyBrowser::addSubProperty(QtProperty *parentItem, const QList<QVariant> &subItems)
{
    for(int idx = 0; idx < subItems.size(); ++idx) {
        QVariantHash item = subItems.at(idx).toHash();
        if (subItems.at(idx).type() == QVariant::Map) {
            item.clear();
            QVariantMap map = subItems.at(idx).toMap();
            for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
                item[it.key()] = it.value();
            }
        }

        QtProperty *propertyItem = createProperty(item);
        parentItem->addSubProperty(propertyItem);
    }
}

QtProperty *TreePropertyBrowser::addGroupProperty(const QString &propertyName, const QList<QVariant> &groupProperty)
{
    QtProperty *groupItem = m_groupManager->addProperty(propertyName);
    for (auto property : groupProperty) {
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

    for(auto variant : groupProperty) {
        QtProperty *propertyItem = nullptr;
        if (variant.type() == QVariant::Map) {
            propertyItem = createProperty(variant.toMap());
        } else {
            propertyItem = createProperty(variant.toHash());
        }
        groupItem->addSubProperty(propertyItem);
    }
    return groupItem;
}

void TreePropertyBrowser::updateGroupProperty(QtProperty *parentItem, const QList<QVariant> &groupProperty)
{
    if(parentItem == NULL)
        return;

    for(int idx = 0; idx < groupProperty.size(); ++idx)
    {
        QVariantHash property = groupProperty.at(idx).toHash();

        if (groupProperty.at(idx).type() == QVariant::Map) {
            property.clear();
            QVariantMap map = groupProperty.at(idx).toMap();
            for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
                property[it.key()] = it.value();
            }
        }

        updateProperty(parentItem, property);
    }
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, bool value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));


}

void TreePropertyBrowser::onValueChanged(QtProperty *property, int value)
{
    if(property->propertyKey().isEmpty())
        return;

    if (sender() == m_enumManager) {
        onPropertyChanged(property->propertyKey(), m_enumManager->enumKeys(property).value(value).toString());
    }
    else {

           //检查变量名是否与同级节点的其他节点变量名重复 by zyg
        if(property->propertyKey() == QStringLiteral("变量名"))
        {

        }

           //将父属性的key值拼装进去
        QString comKeyStr;
        if(!property->parentPropertiesKey().isEmpty())
        {
            comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
        }
        else
        {
            comKeyStr = property->propertyKey();
        }
        onPropertyChanged(comKeyStr, QVariant::fromValue(value));
    }
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, double value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));

}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QString value)
{
    if(property->propertyKey().isEmpty())
        return;

    QString t_value = value;
    //检查变量名是否与同级节点的其他节点变量名重复 by zyg
    if(property->propertyKey() == QStringLiteral("变量名"))
    {
        if(m_variableNamelist.contains(value))
        {
            emit errorVariableName(QVariant::fromValue(value));
            updateProperties(m_properties);
            return;
        }
    }

    if(!property->parentPropertiesKey().isEmpty())
    {
        QList<QString> t_parentPropertylist = property->parentPropertiesKey();
        if(t_parentPropertylist.at(0) == QStringLiteral("输出参数"))
        {
            if(m_variableNamelist.contains(value))
            {
                    emit errorVariableName(QVariant::fromValue(value));
                    updateProperties(m_properties);
                    return;
            }

            else
            {
                if(property->propertyKey() == QStringLiteral("中心频率变量"))
                {
                    if(value == m_currentXhqdbl)
                    {
                        emit errorVariableName(QVariant::fromValue(value));
                        updateProperties(m_properties);
                        return;
                    }
                    else
                    {
                        m_currentZxqdbl = value;
                    }
                }

                if(property->propertyKey() == QStringLiteral("信号强度变量"))
                {
                    if(value == m_currentZxqdbl)
                    {
                        emit errorVariableName(QVariant::fromValue(value));
                        updateProperties(m_properties);
                        return;
                    }

                    else
                    {
                        m_currentXhqdbl = value;
                    }
                }
            }
        }
    }

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(t_value));

}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QColor value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QFont value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));

}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QPoint value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));

}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QSize value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));

}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QDate value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, QVariant::fromValue(value));

}

void TreePropertyBrowser::onValueChanged(QtProperty *property, QVariant value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, value);
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, const QKeySequence &value)
{
    if(property->propertyKey().isEmpty())
        return;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, value);
}

void TreePropertyBrowser::onVlaueChanged(QtProperty *property, QRect value)
{
    if (property->propertyKey().isEmpty())
        return ;

    //将父属性的key值拼装进去
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(comKeyStr, value);
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, const QVariantList &value)
{
    if(property->propertyKey().isEmpty())
        return;
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(property->propertyKey(), value);
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, const Range &value)
{
    if(property->propertyKey().isEmpty())
        return;
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, const RangeF &value)
{
    if(property->propertyKey().isEmpty())
        return;
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
}

void TreePropertyBrowser::onValueChanged(QtProperty *property, const CheckableEnumValue &value)
{
    if(property->propertyKey().isEmpty())
        return;
    QString comKeyStr;
    if(!property->parentPropertiesKey().isEmpty())
    {
        comKeyStr = property->parentPropertiesKey().at(0) + "-" +property->propertyKey();
    }
    else
    {
        comKeyStr = property->propertyKey();
    }
    QVariantList keys = m_checkableEnumManager->keys(property);
    if (!keys.isEmpty()) {
        onPropertyChanged(property->propertyKey(), QVariant::fromValue(keys));
    } else {
        onPropertyChanged(property->propertyKey(), QVariant::fromValue(value));
    }
}

void TreePropertyBrowser::showContextMenu(const QPoint &pos)
{
    auto curItem = currentItem();
    if (curItem == nullptr)
        return;
    if(currentItem()->property()->propertyName() ==QStringLiteral("表达式")) {
        biaoDaShi = currentItem()->property()->valueText();
        menu->exec(QCursor::pos());
    }
}

void TreePropertyBrowser::setBiaoDaShi(const QString &msg)
{
    onPropertyChanged("handleModify",QVariant::fromValue(msg));
}

void TreePropertyBrowser::slotSelectCurrentItem(QtBrowserItem *item)
{
    //获取故障树节点属性列表控件信号 by zyg
    QString t_label = item->property()->propertyKey();
    QString t_value = item->property()->valueText();
    emit selectItemProperty(t_label, t_value);
    return;
}

void TreePropertyBrowser::onReceiveAllFunction(const QList<QString> &funcs)
{
    m_allFunctionNameList = funcs;

}

void TreePropertyBrowser::showEditWindows()
{
    EditWindow *w = new EditWindow;
    w->setText(biaoDaShi);
    connect(w,&EditWindow::sendBiaoDaShi,this,&TreePropertyBrowser::setBiaoDaShi);
    PopupWindow::show({}, QStringLiteral("算法表达式编辑框"),w);
}


bool TreePropertyBrowser::testExpression(const QString &expression, const QVariantMap &compareValues) const
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

bool TreePropertyBrowser::testCompare(const QString &compare, const QVariantMap &compareValues) const
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
            return compareValues.value(property).toString().compare(value) == 0;
        }

        case NotEqual: {
            switch (dataType) {
            case QVariant::Int:
                return compareValues.value(property).toInt() != value.toInt();
            case QVariant::Double:
                return !qFuzzyCompare(compareValues.value(property).toFloat(), value.toFloat());
            default: break;
            }
            return compareValues.value(property).toString().compare(value) != 0;
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
