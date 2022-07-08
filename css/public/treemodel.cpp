#include "treemodel.h"
#include <QDebug>
#include <QIcon>
#include <QMimeData>
#include <QStringList>

using namespace Sio;

TreeModel::TreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_root(new Object)
{
    m_root->setObjectNotifier(qobject_cast<IObjectNotifier*>(this));
}

TreeModel::~TreeModel()
{
    delete m_root;
    m_dropActionMap.clear();
}

void TreeModel::updateHeaderData(const QList<QPair<QString, QString>>& headerData)
{
    m_headerKeys.clear();
    m_root->clearAttributes();

    for (int i = 0; i < headerData.size(); ++i) {
        m_headerKeys.append(headerData[i].first);
        m_root->setAttribute(headerData[i].first, headerData[i].second);
    }

    emit headerDataChanged(Qt::Horizontal, 0, headerData.size());
}

void TreeModel::setCheckableColumn(int column, bool checkable)
{
    bool canCheck = m_checkableColumn.contains(column);
    if (checkable && !canCheck) {
        m_checkableColumn.insert(column);
    } else if (!checkable && canCheck) {
        m_checkableColumn.remove(column);
    }

    emit dataChanged(index(0, column), index(rowCount(), column));
}

void TreeModel::setEditableColumn(int column, bool editable)
{
    bool canEdit = m_editableColumn.contains(column);
    if (editable && !canEdit) {
        m_editableColumn.insert(column);
    } else if (!editable && canEdit) {
        m_editableColumn.remove(column);
    }
    emit dataChanged(index(0, column), index(rowCount(), column));
}

void TreeModel::setDragableColumn(int column, bool dragable)
{
    bool canDrag = m_dragableColumn.contains(column);
    if (dragable && !canDrag) {
        m_dragableColumn.insert(column);
    } else if (!dragable && canDrag) {
        m_dragableColumn.remove(column);
    }
}

void TreeModel::setDropableColumn(int column, bool dropable)
{
    bool canDrop = m_dropableColumn.contains(column);
    if (dropable && !canDrop) {
        m_dropableColumn.insert(column);
    } else if (!dropable && canDrop) {
        m_dropableColumn.remove(column);
    }
}

void TreeModel::setIndexColumn(const QString& key)
{
    if (m_indexKey.compare(key) == 0) {
        return;
    }

    m_indexData.clear();
    for (int i = 0; i < m_root->childCount(); ++i) {
        updateIndexData(m_root->childAt(i));
    }
}

void TreeModel::setColumnIconAttribute(int column, const QString& key, const QIcon& defaultIcon)
{
    m_columnIcon.insert(column, qMakePair<QString, QIcon>(key, defaultIcon));
}

void TreeModel::setColumnIconAttribute(int column, const QString& key, const QString& iconpathTmpl, const QIcon& defaultIcon)
{
    m_columnIconTemplatePath.insert(column, qMakePair<QString, QString>(key, iconpathTmpl));
    m_columnDefaultIcons.insert(column, defaultIcon);
}

void TreeModel::setColumnIconMap(int column, const QString& value, const QIcon& icon)
{
    m_columnIconMap[column][value] = icon;
}

void TreeModel::setColumnDisplayMap(int column, const QString& value, const QString& display)
{
    m_columnDisplayMap[column][value] = display;
}

int TreeModel::topLevelObjectIndexOf(Object* object)
{
    return m_root->childIndexOf(object);
}

int TreeModel::addTopLevelObject(Object* object)
{
    return addChildObject(m_root, object);
}

void TreeModel::removeTopLevelObject(Object* object)
{
    m_root->removeChild(object);
}

Object* TreeModel::takeTopLevelObject(int index)
{
    return m_root->takeChild(index);
}

int TreeModel::topLevelObjectCount() const
{
    return m_root->childCount();
}

Object* TreeModel::topLevelObject(int index) const
{
    return m_root->childAt(index);
}

Object* TreeModel::findTopLevelObject(const QString& key, const QVariant& data) const
{
    for (int i = 0; i < topLevelObjectCount(); ++i) {
        Object* object = topLevelObject(i);
        if (object == nullptr) {
            continue;
        }

        if (object->attribute(key) == data) {
            return object;
        }
    }

    return nullptr;
}

void TreeModel::setObjectFlag(Object* object, const QString& key, Qt::ItemFlag flag, bool enable)
{
    if (object == nullptr) {
        return;
    }
    Qt::ItemFlags flags = objectFlag(object, key);
    if (enable) {
        flags |= flag;
    } else {
        flags ^= flag;
    }

    object->setAttribute(QString("%1.flags").arg(key), static_cast<int>(flags));
}

Qt::ItemFlags TreeModel::objectFlag(Object* object, const QString& key) const
{
    if (object == nullptr) {
        return static_cast<Qt::ItemFlags>(0xFFFF);
    }

    return static_cast<Qt::ItemFlags>(object->attribute(QString("%1.flags").arg(key), 0xFFFF).toInt());
}

bool TreeModel::hasObjectFlag(Object* object, const QString& key, Qt::ItemFlag flag) const
{
    return objectFlag(object, key) & flag;
}

int TreeModel::addChildObject(Object* parent, Object* child)
{
    if (child == nullptr || parent == child) {
        return -1;
    }

    Object* pObject = parent;
    if (pObject == nullptr) {
        pObject = m_root;
    }

    if (pObject->childIndexOf(child) >= 0) {
        return -1;
    }

    int row = pObject->childCount();
    beginInsertRows(index(pObject), row, row);
    bool added = pObject->addChild(child);
    endInsertRows();
    if (added) {
        child->setObjectNotifier(this);
    }

    return added ? row : -1;
}

int TreeModel::insertChildObject(Object* parent, int row, Object* child)
{
    if (child == nullptr || parent == child) {
        return -1;
    }

    Object* pObject = parent;
    if (pObject == nullptr) {
        pObject = m_root;
    }

    if (pObject->childIndexOf(child) >= 0) {
        return -1;
    }

    int idx = qBound(0, row, pObject->childCount());
    beginInsertRows(index(pObject), idx, idx);
    bool insert = pObject->insertChild(idx, child);
    endInsertRows();
    if (insert) {
        child->setObjectNotifier(this);
    }

    return insert ? idx : -1;
}

bool TreeModel::removeObject(Object* object)
{
    QModelIndex idx = index(object);
    if (!idx.isValid()) {
        return false;
    }
    removeChild(object);
    return removeRow(idx.row(), idx.parent());
}

QModelIndex TreeModel::find(const QString& key, const QVariant& data) const
{
    if (m_indexKey.compare(key) == 0) {
        Object* obj = m_indexData.value(data.toString(), nullptr);
        if (obj == nullptr) {
            return QModelIndex();
        }

        return createIndex(obj->parent()->childIndexOf(obj), m_headerKeys.indexOf(key), obj->parent());
    }

    for (int i = 0; i < m_root->childCount(); ++i) {
        Object* obj = find(m_root->childAt(i), key, data);
        if (obj != nullptr) {
            return createIndex(obj->parent()->childIndexOf(obj), m_headerKeys.indexOf(key), obj->parent());
        }
    }

    return QModelIndex();
}

Object* TreeModel::find(Object* object, const QString& key, const QVariant& data) const
{
    if (object == nullptr || key.isEmpty()) {
        return nullptr;
    }

    if (object->attribute(key) == data) {
        return object;
    }

    for (int i = 0; i < object->childCount(); ++i) {
        Object* obj = find(object->childAt(i), key, data);
        if (obj != nullptr) {
            return obj;
        }
    }

    return nullptr;
}

Object* TreeModel::object(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return m_root;
    }

    Object* parent = static_cast<Object*>(index.internalPointer());
    if (parent == nullptr) {
        return m_root->childAt(index.row());
    }

    return parent->childAt(index.row());
}

QMap<int, QVariant> TreeModel::itemData(const QModelIndex& index) const
{
    QMap<int, QVariant> roles = QAbstractItemModel::itemData(index);
    QHashIterator<int, QByteArray> it(m_roleNames);
    while (it.hasNext()) {
        it.next();
        roles.insert(it.key(), data(index, it.key()));
    }
    return roles;
}

int TreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_root->attributeCount();
}

bool TreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Object* pObject = object(parent);
    if (pObject == nullptr) {
        return false;
    }

    if (row < 0 || row > pObject->childCount()) {
        return false;
    }

    int lastRow = qBound(row, row + count - 1, pObject->childCount() - 1);
    beginRemoveRows(parent, row, lastRow);
    for (int i = lastRow; i >= row; --i) {
        pObject->removeChild(i);
    }
    endRemoveRows();

    return true;
}

bool TreeModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
    Object* pSrcObject = object(sourceParent);
    Object* pDestObject = object(destinationParent);
    if (pSrcObject == nullptr || pDestObject == nullptr) {
        return false;
    }

    if (sourceRow < 0 || sourceRow >= rowCount(sourceParent)) {
        return false;
    }

    int srcLastRow = qBound(0, sourceRow + count - 1, pSrcObject->childCount());
    int destFirstRow = qMin(pDestObject->childCount(), destinationChild);
    destinationChild = qBound(0, destinationChild, pDestObject->childCount());

    if (beginMoveRows(sourceParent, sourceRow, srcLastRow, destinationParent, destinationChild)) {
        for (int i = srcLastRow; i >= sourceRow; --i) {
            pDestObject->insertChild(destFirstRow, pSrcObject->takeChild(i));
        }
        endMoveRows();
        return true;
    }

    return false;
}

QString TreeModel::roleName(int role) const
{
    return QString::fromUtf8(m_roleNames.value(role));
}

QHash<int, QByteArray> TreeModel::roleNames() const
{
    return m_roleNames;
}

void TreeModel::setRoleNames(const QHash<int, QByteArray>& roleNames)
{
    m_roleNames = roleNames;
}

void TreeModel::setDropableMimeType(const QString& type, Qt::DropActions actions, IDropAction* dropAciton)
{
    m_dropActionMap[type][actions] = dropAciton;
    m_supportDropActions |= actions;
}

void TreeModel::unsetDropableMimeType(const QString& type)
{
    QMap<Qt::DropActions, IDropAction*> dropActions = m_dropActionMap.take(type);
    if (dropActions.size() > 0) {
        qDeleteAll(dropActions);

        // recreate the Accept Drop Actions
        m_supportDropActions = nullptr;
        QMapIterator<QString, QMap<Qt::DropActions, IDropAction*>> it(m_dropActionMap);
        while (it.hasNext()) {
            it.next();
            QMapIterator<Qt::DropActions, IDropAction*> cit(it.value());
            while (cit.hasNext()) {
                cit.next();
                m_supportDropActions |= cit.key();
            }
        }
    }
}

void TreeModel::setDragableMimeType(const QString& type, Qt::DropActions actions, IDragAction* dragAction)
{
    m_dragActionData.mimeType = type;
    m_dragActionData.actions = actions;
    m_dragActionData.dragAction = dragAction;
}

QStringList TreeModel::mimeTypes() const
{
    if (m_dropActionMap.contains(m_dragActionData.mimeType)) {
        return m_dropActionMap.keys();
    }

    QStringList types(m_dropActionMap.keys());
    types << m_dragActionData.mimeType;
    return types;
}

Qt::DropActions TreeModel::supportedDragActions() const
{
    return m_dragActionData.actions;
}

Qt::DropActions TreeModel::supportedDropActions() const
{
    return m_supportDropActions;
}

bool TreeModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
    if (data == nullptr) {
        return false;
    }

    if (data->hasFormat(m_dragableMimeType.first)) {
        return true;
    }

    for (int i = 0; i < data->formats().size(); ++i) {
        QString format = data->formats().value(i);
        if (!m_dropActionMap.contains(format)) {
            continue;
        }

        QMapIterator<Qt::DropActions, IDropAction*> it(m_dropActionMap[format]);
        while (it.hasNext()) {
            it.next();
            if (action & it.key()) {
                if (it.value() != nullptr) {
                    return it.value()->canDrop(this, data, action, row, column, parent);
                }

                return QAbstractItemModel::canDropMimeData(data, action, row, column, parent);
            }
        }
    }

    return QAbstractItemModel::canDropMimeData(data, action, row, column, parent);
}

bool TreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    if (data == nullptr || !(action == Qt::CopyAction || action == Qt::MoveAction)) {
        return false;
    }

    for (int i = 0; i < data->formats().size(); ++i) {
        QString format(data->formats().at(i));
        if (!m_dropActionMap.contains(format)) {
            continue;
        }

        if (row > rowCount(parent)) {
            row = rowCount(parent);
        }
        if (row == -1) {
            row = rowCount(parent);
        }
        column = 0;

        IDropAction* dropAction = m_dropActionMap[format].value(action, nullptr);
        if (dropAction == nullptr) {
            continue;
        }
        dropAction->doDrop(this, data, action, row, column, parent);
    }
#if 0
    if (!m_dragableMimeType.first.isEmpty()
            && data->hasFormat(m_dragableMimeType.first)
            && (m_dragableMimeType.second & action))
    {

        switch (action)
        {
        case Qt::MoveAction:
        {
            Object *obj = reinterpret_cast<Object *>(data->data(m_dragableMimeType.first).toInt());
            if (obj != nullptr)
            {
                object(parent)->addChild(obj);
            }
            break;
        }
        case Qt::CopyAction:
        {
            break;
        }
        }
    }
#endif
    return false;
}

void TreeModel::attributeChanged(Object* object, const QString& attribute, const QVariant& value, const QVariant& previousValue)
{
    Q_UNUSED(attribute)
    Q_UNUSED(value)
    Q_UNUSED(previousValue)
    if (object == nullptr || object == m_root || object->parent() == nullptr) {
        return;
    }

    if (object->attributeRole(attribute) == Qt::CheckStateRole) {
        m_checkStatus.insert(object, value.toInt());
    }

    emit dataChanged(createIndex(object->parent()->childIndexOf(object), 0, object->parent()),
        createIndex(object->parent()->childIndexOf(object), columnCount() - 1, object->parent()));
}

void TreeModel::childAdded(Object* object, Object* child)
{
    Q_UNUSED(object)
    updateIndexData(child);
    setNotifierForObject(child, true);
    emit itemAdded(index(child));
}

void TreeModel::aboutRemoveChild(Object* object, Object* child)
{
    Q_UNUSED(object)
    updateIndexData(child, false);
    setNotifierForObject(child, false);
}

void TreeModel::setError(int err, const QString& errString)
{
    if (error.error == err) {
        return;
    }
    error.error = err;
    error.errorString = errString;
    qDebug() << "ERROR:" << err << ">" << errString;
    emit errorOccurred(err, errString);
}

bool TreeModel::updateObjectParent(Object* object, int state)
{
    if (object == nullptr || object->parent() == nullptr) {
        return false;
    }

    if (state == Qt::PartiallyChecked) {
        m_checkStatus.insert(object, static_cast<int>(Qt::PartiallyChecked));
        if (m_roleNames.contains(Qt::CheckStateRole)) {
            object->setData(Qt::CheckStateRole, Qt::PartiallyChecked);
        } else {
            emit dataChanged(createIndex(object->parent()->childIndexOf(object), 0, object->parent()),
                createIndex(object->parent()->childIndexOf(object), columnCount() - 1, object->parent()),
                QVector<int>(Qt::CheckStateRole));
        }
        return updateObjectParent(object->parent(), static_cast<int>(Qt::PartiallyChecked));
    }

    if (state == Qt::Checked) {
        for (int i = 0; i < object->childCount(); ++i) {
            if (m_checkStatus.value(object->childAt(i)) == Qt::Unchecked) {
                return updateObjectParent(object, static_cast<int>(Qt::PartiallyChecked));
            }
        }

        m_checkStatus.insert(object, Qt::Checked);
        if (m_roleNames.contains(Qt::CheckStateRole)) {
            object->setData(Qt::CheckStateRole, Qt::Checked);
        } else {
            emit dataChanged(createIndex(object->parent()->childIndexOf(object), 0, object->parent()),
                createIndex(object->parent()->childIndexOf(object), columnCount() - 1, object->parent()),
                QVector<int>(Qt::CheckStateRole));
        }

        return updateObjectParent(object->parent(), static_cast<int>(Qt::Checked));
    }

    for (int i = 0; i < object->childCount(); ++i) {
        if (m_checkStatus.value(object->childAt(i)) == Qt::Checked) {
            return updateObjectParent(object, static_cast<int>(Qt::PartiallyChecked));
        }
    }

    m_checkStatus.insert(object, Qt::Unchecked);
    if (m_roleNames.contains(Qt::CheckStateRole)) {
        object->setData(Qt::CheckStateRole, Qt::Unchecked);
    } else {
        emit dataChanged(createIndex(object->parent()->childIndexOf(object), 0, object->parent()),
            createIndex(object->parent()->childIndexOf(object), columnCount() - 1, object->parent()),
            QVector<int>(Qt::CheckStateRole));
    }
    return updateObjectParent(object->parent(), static_cast<int>(Qt::Unchecked));
}

bool TreeModel::checkObjectChildren(Object* object, int state)
{
    if (object == nullptr) {
        return false;
    }

    //    qDebug() << " -" << object->data(Qt::DisplayRole).toString() << ":" << m_roleNames.contains(Qt::CheckStateRole)
    //             << "?" << m_roleNames.value(Qt::CheckStateRole)
    //             << "=" << object->data(Qt::CheckStateRole).toInt();

    if (object->childCount() == 0) {
        m_checkStatus.insert(object, state == Qt::Unchecked ? Qt::Unchecked : Qt::Checked);
        if (m_roleNames.contains(Qt::CheckStateRole) || object->containsRole(Qt::CheckStateRole)) {
            object->setData(Qt::CheckStateRole, state);
        }
        return true;
    }

    m_checkStatus.insert(object, state);
    if (m_roleNames.contains(Qt::CheckStateRole) || object->containsRole(Qt::CheckStateRole)) {
        object->setData(Qt::CheckStateRole, state);
    } else {
        emit dataChanged(createIndex(object->parent()->childIndexOf(object), 0, object->parent()),
            createIndex(object->parent()->childIndexOf(object), columnCount() - 1, object->parent()),
            QVector<int>(Qt::CheckStateRole));
    }

    for (int i = 0; i < object->childCount(); ++i) {
        checkObjectChildren(object->childAt(i), state);
    }

    return true;
}

void TreeModel::setNotifierForObject(Object* object, bool attach)
{
    if (object == nullptr) {
        return;
    }

    if (attach) {
        object->setObjectNotifier(qobject_cast<IObjectNotifier*>(this));
    } else {
        object->unsetObjectNotifier(qobject_cast<IObjectNotifier*>(this));
    }

    for (int i = 0; i < object->childCount(); ++i) {
        setNotifierForObject(object->childAt(i), attach);
    }
}

void TreeModel::updateIndexData(Object* object, bool cache)
{
    if (object == nullptr || object->attribute(m_indexKey).toString().isEmpty()) {
        return;
    }

    if (cache) {
        m_indexData.insert(object->attribute(m_indexKey).toString(), object);
    } else {
        m_indexData.remove(object->attribute(m_indexKey).toString());
    }

    for (int i = 0; i < object->childCount(); ++i) {
        updateIndexData(object->childAt(i), cache);
    }
}

void TreeModel::removeChild(Object* object)
{
    if (object == nullptr || object->childList().isEmpty()) {
        return;
    }

    for (Object* child : object->childList()) {
        if (!child->childList().isEmpty()) {
            removeChild(child);
        }

        child->unsetObjectNotifier(this);
        object->removeChild(child);
    }
}

QMimeData* TreeModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.isEmpty()) {
        return nullptr;
    }

    if (!indexes.first().isValid()) {
        return nullptr;
    }

    QMimeData* mime = new QMimeData;
    if (m_dragActionData.dragAction == nullptr) {
        Object* obj = object(indexes.first());
        mime->setData(m_dragActionData.mimeType, QByteArray::number(*(reinterpret_cast<int*>(&obj))));
        return mime;
    }

    return m_dragActionData.dragAction->doDrag(this, indexes);
}

Object* TreeModel::rootObject() const
{
    return m_root;
}

void TreeModel::clearData()
{
    beginResetModel();
    removeChild(m_root);
    endResetModel();
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Object* it = object(index);
    if (it == nullptr) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        QString key(headerKey(index.column()));
        QVariant value(it->attribute(key));
        if (m_columnDisplayMap.contains(index.column())) {
            return m_columnDisplayMap[index.column()].value(QString::number(value.toInt()), value.toString());
        }
        return value;
    } break;
    case Qt::DecorationRole: {
        if (m_columnIconTemplatePath.contains(index.column())) {
            QString iconpath(m_columnIconTemplatePath[index.column()].second.arg(it->attribute(m_columnIconTemplatePath[index.column()].first).toString()));
            return QFile::exists(iconpath) ? QIcon(iconpath) : m_columnDefaultIcons.value(index.column());
        } else if (m_columnIcon.contains(index.column())) {
            return it->attribute(m_columnIcon[index.column()].first, m_columnIcon[index.column()].second);
        } else if (m_columnIconMap.contains(index.column())) {
            return m_columnIconMap.value(index.column()).value(it->attribute(headerKey(index.column())).toString());
        }
    } break;
    case Qt::CheckStateRole: {
        if (m_checkableColumn.contains(index.column())) {
            return m_checkStatus.value(it, Qt::Unchecked);
        }
    } break;
    default: {
        if (m_roleNames.contains(role)) {
            return it->attribute(QString::fromUtf8(m_roleNames.value(role)));
        } else if (it->attributeContains(QString("QtRole#%1").arg(role))) {
            return it->attribute(QString("QtRole#%1").arg(role));
        } else if (it->containsRole(role)) {
            return it->data(role);
        }
    } break;
    }

    return QVariant();
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    QVariant old = data(index, role);
    if (!old.isNull() && old == value) {
        return false;
    }

    Object* it = object(index);
    if (it == nullptr) {
        return false;
    }

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        it->setAttribute(headerKey(index.column()), value);
    } break;
    case Qt::CheckStateRole: {
        if (m_checkableColumn.contains(index.column())) {
            blockSignals(true);
            checkObjectChildren(it, value.toInt());
            blockSignals(false);
            emit dataChanged(createIndex(index.row(), 0, index.parent().internalPointer()),
                createIndex(index.row(), columnCount() - 1, index.parent().internalPointer()),
                QVector<int>(role));
            updateObjectParent(it->parent(), m_checkStatus.value(it));
            if (!it->roleName(role).isEmpty()) {
                it->setAttribute(it->roleName(role), value.toInt());
            }
        }

    } break;
    default: {
        if (m_roleNames.contains(role)) {
            it->setAttribute(QString::fromUtf8(m_roleNames.value(role)), value);
        } else {
            it->setAttribute(QString("QtRole#%1").arg(role), value);
        }
    } break;
    }

    return true;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        Qt::ItemFlags flag = QAbstractItemModel::flags(index);
        if (!m_dropableColumn.isEmpty()) {
            flag |= Qt::ItemIsDropEnabled;
        }
        return flag;
    }

    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (m_checkableColumn.contains(index.column()) && hasObjectFlag(object(index), headerKey(index.column()), Qt::ItemIsUserCheckable)) {
        flag |= Qt::ItemIsUserCheckable;
    }

    if (m_editableColumn.contains(index.column()) && hasObjectFlag(object(index), headerKey(index.column()), Qt::ItemIsEditable)) {
        flag |= Qt::ItemIsEditable;
    }

    if (m_dragableColumn.contains(index.column()) && hasObjectFlag(object(index), headerKey(index.column()), Qt::ItemIsDragEnabled)) {
        flag |= Qt::ItemIsDragEnabled;
    }

    if (m_dropableColumn.contains(index.column()) && hasObjectFlag(object(index), headerKey(index.column()), Qt::ItemIsDropEnabled)) {
        flag |= Qt::ItemIsDropEnabled;
    }

    return flag;
}

QString TreeModel::headerKey(int column) const
{
    return m_headerKeys.value(column);
}

int TreeModel::columnOfHeader(const QString& name) const
{
    return m_headerKeys.indexOf(name);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_root->attribute(m_headerKeys.at(section));
    }

    return QVariant();
}

QModelIndex TreeModel::index(Object* object, int column) const
{
    if (object == nullptr || object->parent() == nullptr) {
        return QModelIndex();
    }

    return createIndex(object->parent()->childIndexOf(object), column, object->parent());
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    Object* parentItem = object(parent);
    if (parentItem == nullptr) {
        return QModelIndex();
    }

    return createIndex(row, column, parentItem);
}

QModelIndex TreeModel::index(int row, int column, void* internalPointer) const
{
    return createIndex(row, column, internalPointer);
}

QModelIndex TreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    Object* parentItem = static_cast<Object*>(index.internalPointer());
    if (parentItem == nullptr || parentItem == m_root) {
        return QModelIndex();
    }

    return this->index(parentItem);
}

bool TreeModel::hasChildren(const QModelIndex& parent) const
{
    Object* parentObject = object(parent);
    if (parentObject == nullptr) {
        return false;
    }

    return parentObject->childCount() > 0;
}

int TreeModel::rowCount(const QModelIndex& parent) const
{
    Object* parentItem = object(parent);
    if (parentItem == nullptr) {
        return 0;
    }

    return parentItem->childCount();
}
