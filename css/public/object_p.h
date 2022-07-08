#ifndef TREEMODEL_OBJECT_P_H
#define TREEMODEL_OBJECT_P_H

#include <qglobal.h>
#include <QVariantHash>
#include <QAbstractItemModel>

namespace Sio {

class Object;
class IObjectNotifier;

namespace Internal {

class ObjectPrivate
{
    Q_DECLARE_PUBLIC(Object)
public:
    ObjectPrivate();
    virtual ~ObjectPrivate();

    void clear();

    void setParent(Object *object);

    Object *find(const QString &attribute, const QVariant &value);
    Object *find(int role, const QVariant &value);
    Object *find(int index, Object *m_parent);

    Object *childAt(int index) const;
    bool addChild(Object *object);
    bool removeChild(Object *object);
    bool insertChild(int index, Object *object);
    bool removeChild(int index);
    Object *takeChild(int index);

    bool cloneData(Object *other);

    QVariant data(int role, const QVariant &defaultValue = QVariant()) const;
    bool setData(int role, const QVariant &value);

    QVariant attribute(const QString &key, const QVariant &defaultValue = QVariant()) const;
    bool setAttribute(const QString &key, const QVariant &value);

    bool handleEvent(const QVariantHash &event, Object *object = nullptr);

    Object *q_ptr = nullptr;
    Object *m_parent = nullptr;
    QAbstractItemModel *m_model = nullptr;
    QList<Object *> m_childList;
    QVariantHash m_attributeMap;
    QMap<int, QString> m_attributeRole;
    QList<IObjectNotifier *> m_objectNotifierList;
    QList<QPair<Object *, QVariantHash> > m_eventQueue;
};

} // namespace Internal

} // namespace QtKit

#endif // TREEMODEL_OBJECT_P

