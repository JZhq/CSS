#ifndef TREEMODEL_IOBJECTNOTIFIER_H
#define TREEMODEL_IOBJECTNOTIFIER_H

#include "object.h"
#include "object_p.h"
#include <QSet>
#include <QString>
#include <QVariant>
#include <QVariantHash>

namespace Sio {

/*
 * \class ExtensionSystem::IObjectNotifier
 * \brief The IObjectNotifier class supplies interfaces to
 handle \l Object's data changed.

 Clients class can inherit from this class and register instance
 to \l Object. Once the data of Object to which it registers changed.
 the relative callback will be called.
 */
class IObjectNotifier {
public:
    /*!
     * \fn IObjectNotifier::IObjectNotifier()
     * \brief constructor of class IObjectNotifier
     */
    IObjectNotifier()
        : m_enabled(true)
    {
    }

    /*
     * \fn IObjectNotifier::~IObjectNotifier()
     * \brief Destructor of class IObjectNotifier
     */
    virtual ~IObjectNotifier()
    {
        detach();
    }

protected:
    /*
     * \fn void IObjectNotifier::parentChanged(const Object *parent, const Object *prev)
     * \brief Handles \l Object parent changed to \a parent. \a prev, the previous parent instance.
     */
    virtual void parentChanged(Object* object, const Object* parent, const Object* prev)
    {
        Q_UNUSED(object)
        Q_UNUSED(parent)
        Q_UNUSED(prev)
    }

    /*
     * \fn void IObjectNotifier::attributeChanged(const QString &attribute, const QVariant &value)
     * \brief Handles the value of \a attribute changed to \a value.
     */
    virtual void attributeChanged(Object* object, const QString& attribute, const QVariant& currentValue, const QVariant& previousValue)
    {
        Q_UNUSED(object)
        Q_UNUSED(attribute)
        Q_UNUSED(currentValue)
        Q_UNUSED(previousValue)
    }

    /*
     * \fn void IObjectNotifier::attributeChanged(int role, const QVariant &value)
     * \brief Handles the value of attribution data specified by field \a role changed to \a value.
     */
    virtual void attributeChanged(Object* object, int role, const QVariant& currentValue, const QVariant& previousValue)
    {
        Q_UNUSED(object)
        Q_UNUSED(role)
        Q_UNUSED(currentValue)
        Q_UNUSED(previousValue)
    }

    /*
     * \fn void IObjectNotifier::childAdded(Object *child)
     * \brief Does some action while \a child is about to be added.
     */
    virtual void childAdded(Object* object, Object* child)
    {
        Q_UNUSED(object)
        Q_UNUSED(child)
    }

    /*
     * \fn void IObjectNotifier::aboutRemoveChild(Object *child)
     * \brief Does some action while \a child is about to be removed.
     */
    virtual void aboutRemoveChild(Object* object, Object* child)
    {
        Q_UNUSED(object)
        Q_UNUSED(child)
    }

    /*
     * \fn void IObjectNotifier::aboutDestoryed(Object *object)
     * \brief Does some action while \a object is about to destory.
     */
    virtual void aboutDestoryed(Object* object)
    {
        m_targetObjects.remove(object);
    }

    /*
     * \fn bool IObjectNotifier::aboutHandleEvent(const QVariantHash &event)
     * \brief Does some action while \a event is about to be handled..
     */
    virtual bool aboutHandleEvent(Object* object, const QVariantHash& event)
    {
        Q_UNUSED(object)
        Q_UNUSED(event)
        return false;
    }

public:
    /*!
     * \brief attach
     * \param obj
     */
    void attach(Object* obj)
    {
        if (m_targetObjects.contains(obj)) {
            return;
        }
        m_targetObjects.insert(obj);
    }

    /*!
     * \brief detach
     * clean itself form the target object
     */
    void detach(Object* obj = nullptr)
    {
        if (obj != nullptr) {
            m_targetObjects.remove(obj);
            return;
        }

        QSetIterator<Object*> i(m_targetObjects);
        while (i.hasNext()) {
            i.next()->unsetObjectNotifier(this);
        }
    }

    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

private:
    bool m_enabled;
    QSet<Object*> m_targetObjects;

    friend class Internal::ObjectPrivate;
};

} // namespace QtKit

Q_DECLARE_INTERFACE(Sio::IObjectNotifier, "Sio::IObjectNotifier")

#endif // TREEMODEL_IOBJECTNOTIFIER_H
