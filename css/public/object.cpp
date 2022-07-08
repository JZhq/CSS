#include "object.h"
#include "iobjectnotifier.h"
#include "object_p.h"
#include <QDebug>
#include <QPainter>

using namespace Sio;
using namespace Internal;

/*!
 * \class Object
 * \brief The Object class
 */

/*!
 * \brief Constructor of class Object. \a parent, default nullptr.
 */
Object::Object(Object* parent)
    : d_ptr(new ObjectPrivate)
{
    d_ptr->q_ptr = this;
    if (parent != nullptr) {
        parent->addChild(this);
    }
}

/*!
 * \brief Constructor of class Object. \a dd, private instance. \a parent, default nullptr.
 */
Object::Object(ObjectPrivate& dd, Object* parent)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    if (parent != nullptr) {
        parent->addChild(this);
    }
}

/*!
 * \brief Destructor of class Object.
 */
Object::~Object()
{
    Q_D(Object);
    qDebug() << Q_FUNC_INFO << ">" << d->m_attributeMap;
    d->clear();
    qDebug() << Q_FUNC_INFO << "> Clear OK";
}

/*!
 * \brief Sets parent to \a object.
 */
void Object::setParent(Object* object)
{
    Q_D(Object);
    d->setParent(object);
}

QAbstractItemModel* Object::model() const
{
    Q_D(const Object);
    return d->m_model;
}

void Object::setModel(QAbstractItemModel* model)
{
    Q_D(Object);
    d->m_model = model;
}

/*!
 * \brief Sets object notifier to \a objectNotifier.
 */
void Object::setObjectNotifier(IObjectNotifier* objectNotifier)
{
    Q_D(Object);
    if (d->m_objectNotifierList.contains(objectNotifier)) {
        return;
    }

    d->m_objectNotifierList.append(objectNotifier);
    objectNotifier->attach(this);
}

/*!
 * \brief Unsets object notifier \a objectNotifier.
 */
void Object::unsetObjectNotifier(IObjectNotifier* objectNotifier)
{
    Q_D(Object);
    if (objectNotifier == nullptr || !d->m_objectNotifierList.contains(objectNotifier)) {
        return;
    }

    d->m_objectNotifierList.removeOne(objectNotifier);
    objectNotifier->detach(this);
}

Object* Object::find(const QString& attribute, const QVariant& value)
{
    Q_D(Object);
    return d->find(attribute, value);
}

Object* Object::find(int role, const QVariant& value)
{
    Q_D(Object);
    return d->find(role, value);
}

Object* Object::find(int index, Object* parent)
{
    Q_D(Object);
    return d->find(index, parent);
}

/*!
 * \brief Get the child count of currnet item
 */
int Object::childCount() const
{
    Q_D(const Object);
    return d->m_childList.count();
}

/*!
 * \brief Returns the child at the position \a index.
 */
Object* Object::childAt(int index) const
{
    Q_D(const Object);
    return d->childAt(index);
}

/*!
 * \brief Returns children.
 */
ObjectList Object::children() const
{
    Q_D(const Object);
    return d->m_childList;
}

/*!
 * \brief Returns childList.
 */
ObjectList& Object::childList()
{
    Q_D(Object);
    return d->m_childList;
}

/*!
 * \brief Returns parent.
 */
Object* Object::parent() const
{
    Q_D(const Object);
    return d->m_parent;
}

/*!
 * \brief Adds child \a object.
 */
bool Object::addChild(Object* object)
{
    Q_D(Object);
    return d->addChild(object);
}

/*!
 * \brief Removes child \a object.
 */
bool Object::removeChild(Object* object)
{
    Q_D(Object);
    return d->removeChild(object);
}

/*!
 * \brief Inserts child \a object at the position \a index.
 */
bool Object::insertChild(int index, Object* object)
{
    Q_D(Object);
    return d->insertChild(index, object);
}

/*!
 * \brief Finds chilad \a object and returns the position index.
 */
int Object::childIndexOf(Object* object) const
{
    Q_D(const Object);
    return d->m_childList.indexOf(object);
}

/*!
 * \brief Removes child which is at position \a index.
 */
bool Object::removeChild(int index)
{
    Q_D(Object);
    return d->removeChild(index);
}

/*!
 * \brief Takes child which is at position \a index.
 */
Object* Object::takeChild(int index)
{
    Q_D(Object);
    return d->takeChild(index);
}

/*!
 * \brief Object::data
 * \param role
 * \return
 */
QVariant Object::data(int role, const QVariant& defaultValue) const
{
    Q_D(const Object);
    return d->data(role, defaultValue);
}

/*!
 * \brief Object::setData
 * \param role
 * \param value
 * \return
 */
bool Object::setData(int role, const QVariant& value)
{
    Q_D(Object);
    return d->setData(role, value);
}

/*!
 * \brief Checks attributions contain \a key or not.
 */
bool Object::attributeContains(const QString& key) const
{
    Q_D(const Object);
    return d->m_attributeMap.contains(key);
}

int Object::attributeCount() const
{
    Q_D(const Object);
    return d->m_attributeMap.size();
}

/*!
 * \brief Returns attributions.
 */
QStringList Object::attributes() const
{
    Q_D(const Object);
    return d->m_attributeMap.keys();
}

/*!
 * \brief Returns attribution specified by \a key.
 */
QVariant Object::attribute(const QString& key, const QVariant& defaultValue) const
{
    Q_D(const Object);
    return d->attribute(key, defaultValue);
}

/*!
 * \brief Sets the value of attribution specified by \a key to \a value.
 */
bool Object::setAttribute(const QString& key, const QVariant& value)
{
    Q_D(Object);
    return d->setAttribute(key, value);
}

/*!
 * \brief Returns attribution data.
 */
QVariantHash Object::attributeData() const
{
    Q_D(const Object);
    return d->m_attributeMap;
}

/*!
 * \brief Set attribution data to \a data.
 */
bool Object::setAttributeData(const QVariantHash& data, bool clearOldData)
{
    Q_D(Object);
    if (clearOldData) {
        d->m_attributeMap.clear();
        d->m_attributeMap = data;
        return true;
    }

    QHashIterator<QString, QVariant> it(data);
    while (it.hasNext()) {
        it.next();
        d->m_attributeMap.insert(it.key(), it.value());
    }
    return true;
}

/*!
 * \brief Object::setAttributeData
 * \param data
 * \return
 */
bool Object::setAttributeData(const QVariantMap& data, bool clearOldData)
{
    Q_D(Object);
    if (clearOldData) {
        d->m_attributeMap.clear();
    }

    QMapIterator<QString, QVariant> it(data);
    while (it.hasNext()) {
        it.next();
        d->m_attributeMap.insert(it.key(), it.value());
    }
    return true;
}

/*!
 * \brief Object::dataRoles
 * \return
 */
QMap<int, QString> Object::dataRoles() const
{
    Q_D(const Object);
    return d->m_attributeRole;
}

/*!
 * \brief Object::setDataRoles
 * \param roles
 * \return
 */
bool Object::setDataRoles(const QMap<int, QString>& roles, bool clearAll)
{
    Q_D(Object);
    if (clearAll) {
        d->m_attributeRole = roles;
    } else {
        QMapIterator<int, QString> it(roles);
        while (it.hasNext()) {
            it.next();
            d->m_attributeRole.insert(it.key(), it.value());
        }
    }

    return !roles.isEmpty();
}

/*!
 * \brief Returns attribution specified by \a name.
 */
int Object::attributeRole(const QString& name) const
{
    Q_D(const Object);
    return d->m_attributeRole.key(name, -1);
}

/*!
 * \brief Object::attributeName
 * \param role
 * \return
 */
QString Object::roleName(int role) const
{
    Q_D(const Object);
    return d->m_attributeRole.value(role, QString());
}

/*!
 * \brief Object::containsRole
 * \param role
 * \return
 */
bool Object::containsRole(int role) const
{
    Q_D(const Object);
    return d->m_attributeRole.contains(role);
}

bool Object::cloneData(Object* other)
{
    Q_D(Object);
    return d->cloneData(other);
}

void Object::clearAttributes()
{
    Q_D(Object);
    d->m_attributeMap.clear();
}

/*!
 * \brief Handles attribution specified by \a key changed to new \a value.
 */
void Object::attributeChanged(const QString& key, const QVariant& currentValue, const QVariant& previousValue)
{
    Q_UNUSED(key)
    Q_UNUSED(currentValue)
    Q_UNUSED(previousValue)
}

/*!
 * \brief Handles the object which is about to destory.
 */
void Object::aboutDestory(Object* object)
{
    Q_UNUSED(object)
}

/*!
 * \brief Handles \a child added.
 */
void Object::childAdded(Object* child)
{
    Q_UNUSED(child)
}

/*!
 * \brief Handles the object which is about to remove.
 */
void Object::aboutRemoveChild(Object* child)
{
    Q_UNUSED(child)
}

/*!
 * \brief Handles \a event.
 */
bool Object::handleEvent(const QVariantHash& event, Object* object)
{
    Q_D(Object);
    return d->handleEvent(event, object);
}

ObjectPrivate::ObjectPrivate()
    : q_ptr(nullptr)
    , m_parent(nullptr)
    , m_model(nullptr)
{
}

ObjectPrivate::~ObjectPrivate()
{
}

void ObjectPrivate::clear()
{
    Q_Q(Object);
    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->aboutDestoryed(q);
        q->unsetObjectNotifier(notifier);
    }

    q->aboutDestory(q);

    setParent(nullptr);
    qDeleteAll(m_childList);
    m_childList.clear();
}

void ObjectPrivate::setParent(Object* object)
{
    Q_Q(Object);
    if (q == object) {
        return;
    }

    if (m_parent == object) {
        if (m_parent != nullptr && m_parent->childIndexOf(q) < 0) {
            m_parent->addChild(q);
        }

        return;
    }

    if (m_parent != nullptr) {
        m_parent->takeChild(m_parent->childIndexOf(q));
    }

    m_parent = object;

    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->parentChanged(q, object, m_parent);
    }

    if (m_parent == nullptr) {
        m_model = nullptr;
        return;
    }

    m_model = m_parent->model();

    for (int i = 0; i < m_eventQueue.size(); ++i) {
        QPair<Object*, QVariantHash> event = m_eventQueue.value(i);
        m_parent->handleEvent(event.second, event.first);
    }

    m_eventQueue.clear();
}

Object* ObjectPrivate::find(const QString& attribute, const QVariant& value)
{
    Q_Q(Object);
    if (m_attributeMap.contains(attribute) && m_attributeMap.value(attribute) == value) {
        return q;
    }

    for (Object* object : m_childList) {
        Object* obj = object->find(attribute, value);
        if (obj != nullptr) {
            return obj;
        }
    }

    return nullptr;
}

Object* ObjectPrivate::find(int role, const QVariant& value)
{
    QString key(m_attributeRole.value(role, QString()));
    if (key.isEmpty()) {
        return nullptr;
    }

    return find(key, value);
}

Object* ObjectPrivate::find(int index, Object* parent)
{
    Q_Q(Object);
    if (q == parent) {
        return childAt(index);
    }

    for (int i = 0; i < m_childList.size(); ++i) {
        Object* child = childAt(i);
        if (child == nullptr) {
            continue;
        }
        Object* obj = child->find(index, parent);
        if (obj != nullptr) {
            return obj;
        }
    }
    return nullptr;
}

Object* ObjectPrivate::childAt(int index) const
{
    if (index < 0 || index >= m_childList.size()) {
        return nullptr;
    }

    return m_childList.at(index);
}

bool ObjectPrivate::addChild(Object* object)
{
    Q_Q(Object);
    if (object == nullptr || q == object || m_childList.contains(object)) {
        return false;
    }

    object->setParent(q);
    m_childList.append(object);

    q->childAdded(object);

    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->childAdded(q, object);
    }

    return true;
}

bool ObjectPrivate::removeChild(Object* object)
{
    Q_Q(Object);
    if (object == nullptr || q == object || !m_childList.contains(object)) {
        return false;
    }

    if (!m_objectNotifierList.isEmpty()) {
        for (IObjectNotifier* notifier : m_objectNotifierList) {
            if (notifier == nullptr || !notifier->enabled()) {
                continue;
            }
            notifier->aboutRemoveChild(q, object);
        }
    }

    q->aboutRemoveChild(object);
    object->d_func()->m_parent = nullptr;
    if (m_childList.removeOne(object)) {
        QList<int> deleteList;
        for (int i = 0; i < m_eventQueue.size(); ++i) {
            QPair<Object*, QVariantHash> event = m_eventQueue.at(i);
            if (object == event.first) {
                deleteList.prepend(i);
            }
        }

        for (int index : deleteList) {
            m_eventQueue.removeAt(index);
        }

        delete object;
        return true;
    }
    return false;
}

bool ObjectPrivate::insertChild(int index, Object* object)
{
    Q_Q(Object);
    if (object == nullptr || q == object) {
        return false;
    }

    if (m_childList.contains(object)) {
        return false;
    }

    m_childList.insert(index, object);
    object->setParent(q);

    q->childAdded(object);
    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->childAdded(q, object);
    }

    return true;
}

bool ObjectPrivate::removeChild(int index)
{
    Q_Q(Object);
    if (index < 0 || index >= m_childList.size()) {
        return false;
    }

    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->aboutRemoveChild(q, m_childList.value(index, nullptr));
    }

    Object* obj = m_childList.takeAt(index);
    qDebug() << index << "; object =" << obj << m_childList;
    if (obj != nullptr) {
        for (int i = 0; i < m_eventQueue.size(); ++i) {
            QPair<Object*, QVariantHash> event = m_eventQueue.at(i);
            if (obj == event.first) {
                m_eventQueue.removeAt(i);
            }
        }
        obj->d_func()->m_parent = nullptr;
        delete obj;
    }

    qDebug() << "LEAVE >" << (obj != nullptr);
    return (obj != nullptr);
}

Object* ObjectPrivate::takeChild(int index)
{
    Q_Q(Object);
    if (index < 0 || index >= m_childList.size()) {
        return nullptr;
    }

    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->aboutRemoveChild(q, m_childList.value(index, nullptr));
    }

    q->aboutRemoveChild(m_childList.value(index, nullptr));
    Object* child = m_childList.takeAt(index);

    if (child == nullptr) {
        return child;
    }

    child->d_func()->m_parent = nullptr;
    QList<int> deleteList;
    for (int i = 0; i < m_eventQueue.size(); ++i) {
        QPair<Object*, QVariantHash> event = m_eventQueue.at(i);
        if (child == event.first) {
            deleteList.prepend(i);
        }
    }

    for (int index : deleteList) {
        m_eventQueue.removeAt(index);
    }

    return child;
}

bool ObjectPrivate::cloneData(Object* other)
{
    Q_Q(Object);
    if (q == other) {
        return false;
    }

    m_attributeMap = other->attributeData();
    m_attributeRole = other->dataRoles();
    return true;
}

QVariant ObjectPrivate::data(int role, const QVariant& defaultValue) const
{
    return attribute(m_attributeRole.value(role, QString()), defaultValue);
}

bool ObjectPrivate::setData(int role, const QVariant& value)
{
    QString key(m_attributeRole.value(role, QString()));
    if (key.isEmpty()) {
        return false;
    }

    return setAttribute(key, value);
}

QVariant ObjectPrivate::attribute(const QString& key, const QVariant& defaultValue) const
{
    return m_attributeMap.value(key, defaultValue);
}

bool ObjectPrivate::setAttribute(const QString& key, const QVariant& value)
{
    Q_Q(Object);
    if (m_attributeMap.contains(key) && m_attributeMap.value(key) == value) {
        return false;
    }

    QVariant previousValue(m_attributeMap.value(key));
    m_attributeMap.insert(key, value);
    q->attributeChanged(key, value, previousValue);

    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }

        notifier->attributeChanged(q, key, value, previousValue);
        int role = q->attributeRole(key);
        if (role >= 0) {
            notifier->attributeChanged(q, role, value, previousValue);
        }
    }

    return true;
}

bool ObjectPrivate::handleEvent(const QVariantHash& event, Object* object)
{
    Q_Q(Object);
    for (IObjectNotifier* notifier : m_objectNotifierList) {
        if (notifier == nullptr || !notifier->enabled()) {
            continue;
        }
        notifier->aboutHandleEvent(q, event);
    }

    if (m_parent == nullptr) {
        m_eventQueue << qMakePair<Object*, QVariantHash>(object, event);
        return false;
    }

    m_parent->handleEvent(event, object);
    return true;
}
