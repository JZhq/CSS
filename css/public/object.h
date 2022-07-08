#ifndef TREEMODEL_OBJECT_H
#define TREEMODEL_OBJECT_H

#include <qglobal.h>
#include <QScopedPointer>
#include <QVariantHash>
#include <QList>
#include <QStringList>
#include <QAbstractItemModel>
#include <QPixmap>
#include "public_global.h"
namespace Sio {

class IObjectNotifier;
class Object;

namespace Internal {
    class ObjectPrivate;
}

typedef QList<Object*> ObjectList;

/*
 * \class Object
 * \brief The Object class is the base class which has integrated with composite and observe
 * pattern.
 */
class GLOBAL_EXPORT Object
{
    Q_DISABLE_COPY(Object)
    Q_DECLARE_PRIVATE(Internal::Object)
public:
    /*
     * \brief Constructor of class Object. \a parent, default nullptr.
     */
    explicit Object(Object *parent = nullptr);

    /*
     * \brief Destructor of class Object.
     */
    virtual ~Object();

    /*
     * \brief Returns parent.
     */
    Object *parent() const;

    /*
     * \brief Sets parent to \a object.
     */
    void setParent(Object *object);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    /*
     * \brief Sets object notifier to \a objectNotifier.
     */
    void setObjectNotifier(IObjectNotifier *objectNotifier);

    /*
     * \brief Unsets object notifier \a objectNotifier.
     */
    void unsetObjectNotifier(IObjectNotifier *objectNotifier);

    /*!
     * \brief find
     * \param attribute
     * \param value
     * \return
     */
    Object *find(const QString &attribute, const QVariant &value);

    /*!
     * \brief find
     * \param role
     * \param value
     * \return
     */
    Object *find(int role, const QVariant &value);

    /*!
     * \brief find
     * \param index
     * \param parent
     * \return
     */
    Object *find(int index, Object *parent);

    /*
     * \brief get the count of the child item
     * \return the child number of current item
     */
    int childCount() const;

    /*
     * \brief Returns the child at the position \a index.
     */
    Object *childAt(int index) const;

    /*
     * \brief Returns children.
     */
    ObjectList children() const;

    /*
     * \brief Returns childList.
     */
    ObjectList & childList();

    /*
     * \brief Adds child \a object.
     */
    bool addChild(Object *object);

    /*
     * \brief Inserts child \a object at the position \a index.
     */
    bool insertChild(int index, Object *object);

    /*
     * \brief Finds chilad \a object and returns the position index.
     */
    int childIndexOf(Object *object) const;

    /*
     * \brief Removes child \a object.
     */
    bool removeChild(Object *object);

    /*
     * \brief Removes child which is at position \a index.
     */
    bool removeChild(int index);

    /*
     * \brief Takes child which is at position \a index.
     */
    Object *takeChild(int index);

    /*!
     * \brief data
     * \param role
     * \return
     */
    QVariant data(int role, const QVariant &defaultValue = QVariant()) const;

    /*!
     * \brief setData
     * \param role
     * \param value
     * \return
     */
    bool setData(int role, const QVariant &value);

    /*
     * \brief Checks attributions contain \a key or not.
     */
    bool attributeContains(const QString &key) const;

    /*!
     * \brief attributeCount
     * \return
     */
    int attributeCount() const;

    /*
     * \brief Returns attributions.
     */
    QStringList attributes() const;

    /*
     * \brief Returns attribution specified by \a key.
     */
    QVariant attribute(const QString &key, const QVariant &defaultValue = QVariant()) const;

    /*
     * \brief Sets the value of attribution specified by \a key to \a value.
     */
    bool setAttribute(const QString &key, const QVariant &value);

    /*
     * \brief Returns attribution data.
     */
    QVariantHash attributeData() const;

    /*
     * \brief Set attribution data with hash.
     */
    bool setAttributeData(const QVariantHash &data, bool clearOldData = false);

    /*!
     * \brief setAttributeData with Map
     * \param data
     * \return
     */
    bool setAttributeData(const QVariantMap &data, bool clearOldData = false);

    /*!
     * \brief dataRoles
     * \return
     */
    QMap<int, QString> dataRoles() const;

    /*!
     * \brief setDataRoles
     * \param roles
     * \return
     */
    bool setDataRoles(const QMap<int, QString> &roles, bool clearAll = false);

    /*
     * \brief Returns attribution specified by \a name.
     */
    int attributeRole(const QString &name) const;

    /*!
     * \brief attributeName
     * \param role
     * \return
     */
    QString roleName(int role) const;

    /*!
     * \brief containsRole
     * \param role
     * \return
     */
    bool containsRole(int role) const;

    /*!
     * \brief cloneData
     * \param other
     * \return
     */
    bool cloneData(Object *other);

    /*!
     * \brief clearAttributes
     */
    void clearAttributes();

protected:
    /*
     * \brief Handles attribution specified by \a key changed to new \a value.
     */
    virtual void attributeChanged(const QString &key, const QVariant &currentValue, const QVariant &previousValue);

    /*
     * \brief Handles the object which is about to destory.
     */
    virtual void aboutDestory(Object *object);

    /*
     * \brief Handles \a child added.
     */
    virtual void childAdded(Object *object);

    /*
     * \brief Handles the object which is about to remove.
     */
    virtual void aboutRemoveChild(Object *object);

    /*
     * \brief Handles \a event.
     */
    virtual bool handleEvent(const QVariantHash &event, Object *object = nullptr);

protected:
    /*
     * \brief Constructor of class Object. \a dd, private instance. \a parent, default nullptr.
     */
    Object(Internal::ObjectPrivate &dd, Object *parent = nullptr);

    QScopedPointer<Internal::ObjectPrivate> d_ptr;

};

} // namespace QtKit

Q_DECLARE_INTERFACE(Sio::Object, "Object")
Q_DECLARE_INTERFACE(Sio::ObjectList, "ObjectList")

#endif // TREEMODEL_OBJECT_H
