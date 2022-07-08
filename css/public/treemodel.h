#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "iobjectnotifier.h"
#include "object.h"
#include <QAbstractItemModel>
#include <QByteArray>
#include <QDataStream>
#include <QHash>
#include <QIcon>
#include <QModelIndex>
#include <QPair>
#include <QSet>
#include <QVariant>
#include <qobjectdefs.h>
#include "public_global.h"
namespace Sio {

class TreeModel;

class IDragAction {
public:
    IDragAction(const QString& type, Qt::DropActions supportActions)
        : mimeType(type)
        , actions(supportActions)
    {
    }

    virtual ~IDragAction() { }
    virtual QMimeData* doDrag(const TreeModel* model, const QModelIndexList& indexes) const = 0;

protected:
    QString mimeType;
    Qt::DropActions actions;
};

class IDropAction {
public:
    IDropAction(const QString& type, Qt::DropActions supportActions)
        : mimeType(type)
        , actions(supportActions)
    {
    }

    virtual ~IDropAction() { }
    virtual bool doDrop(TreeModel* model, const QMimeData* data, Qt::DropAction action,
        int row, int column, const QModelIndex& parent)
        = 0;
    virtual bool canDrop(const TreeModel* model, const QMimeData* data, Qt::DropAction action,
        int row, int column, const QModelIndex& parent) const = 0;

protected:
    QString mimeType;
    Qt::DropActions actions;
};

class GLOBAL_EXPORT TreeModel : public QAbstractItemModel, public IObjectNotifier {
    Q_OBJECT
    Q_INTERFACES(Sio::IObjectNotifier)
public:
    explicit TreeModel(QObject* parent = nullptr);
    ~TreeModel();

    void updateHeaderData(const QList<QPair<QString, QString>>& headerData);
    void setCheckableColumn(int column, bool checkable = true);
    void setEditableColumn(int column, bool editable = true);
    void setDragableColumn(int column, bool dragable = true);
    void setDropableColumn(int column, bool dropable = true);
    void setIndexColumn(const QString& key);
    void setColumnIconAttribute(int column, const QString& key, const QIcon& defaultIcon = QIcon());
    void setColumnIconAttribute(int column, const QString& key, const QString& iconpathTmpl, const QIcon& defaultIcon = QIcon());
    void setColumnIconMap(int column, const QString& value, const QIcon& icon);
    void setColumnDisplayMap(int column, const QString& value, const QString& display);

    int topLevelObjectIndexOf(Object* object);
    int addTopLevelObject(Object* object);
    void removeTopLevelObject(Object* object);
    Object* takeTopLevelObject(int index);
    int topLevelObjectCount() const;
    Object* topLevelObject(int index) const;
    Object* findTopLevelObject(const QString& key, const QVariant& data) const;

    void setObjectFlag(Object* object, const QString& key, Qt::ItemFlag flag, bool enable = true);
    Qt::ItemFlags objectFlag(Object* object, const QString& key) const;
    bool hasObjectFlag(Object* object, const QString& key, Qt::ItemFlag flag) const;

    int addChildObject(Object* parent, Object* child);
    int insertChildObject(Object* parent, int row, Object* child);
    bool removeObject(Object* object);

    QModelIndex find(const QString& key, const QVariant& data) const;
    Object* find(Object* object, const QString& key, const QVariant& data) const;
    Object* object(const QModelIndex& index) const;

    // interface of QAbstractItemModel
public:
    QMap<int, QVariant> itemData(const QModelIndex& index) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QString headerKey(int column) const;
    int columnOfHeader(const QString& name) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(Object* object, int column = 0) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, void* internalPointer) const;
    QModelIndex parent(const QModelIndex& index) const override;

    bool hasChildren(const QModelIndex& parent) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    bool removeRows(int row, int count, const QModelIndex& parent) override;
    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;

    QString roleName(int role) const;
    QHash<int, QByteArray> roleNames() const override;
    void setRoleNames(const QHash<int, QByteArray>& roleNames);

    void setDropableMimeType(const QString& type, Qt::DropActions actions, IDropAction* dropAciton = nullptr);
    void unsetDropableMimeType(const QString& type);
    void setDragableMimeType(const QString& type, Qt::DropActions actions, IDragAction* dragAction = nullptr);

    QStringList mimeTypes() const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    virtual QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    Object* rootObject() const;
    void clearData();

signals:
    void firstColumnSpanned(int row, const QModelIndex& parent, bool spanned);
    void itemAdded(const QModelIndex& index);
    void errorOccurred(int err, const QString& errString);

    // IObjectNotifier interface
protected:
    void attributeChanged(Object* object, const QString& attribute, const QVariant& value, const QVariant& previousValue) override;
    void childAdded(Object* object, Object* child) override;
    void aboutRemoveChild(Object* object, Object* child) override;
    void setError(int err, const QString& errString);

private:
    bool updateObjectParent(Object* object, int state);
    bool checkObjectChildren(Object* object, int state);
    void setNotifierForObject(Object* object, bool attach = true);
    void updateIndexData(Object* object, bool cache = true);
    void removeChild(Object* object);

private:
    Object* m_root;
    QStringList m_headerKeys;
    QSet<int> m_checkableColumn;
    QSet<int> m_editableColumn;
    QSet<int> m_dragableColumn;
    QSet<int> m_dropableColumn;
    Qt::DropActions m_supportDropActions;
    // Drag Object: <MimeType, Drop Action>
    QPair<QString, Qt::DropActions> m_dragableMimeType;
    struct DragActionData {
        DragActionData()
        {
        }
        QString mimeType;
        Qt::DropActions actions;
        IDragAction* dragAction = nullptr;
    };
    DragActionData m_dragActionData;

    // Drop Action
    QMap<QString, QMap<Qt::DropActions, IDropAction*>> m_dropActionMap;

    // column, pair<icon attribute, icon path
    QMap<int, QPair<QString, QString>> m_columnIconTemplatePath;
    QMap<int, QIcon> m_columnDefaultIcons;
    // column, pair<icon attribute, default icon>
    QMap<int, QPair<QString, QIcon>> m_columnIcon;
    // column, map<value, icon>
    QMap<int, QMap<QString, QIcon>> m_columnIconMap;
    // column, map<value, display text>
    QMap<int, QMap<QString, QString>> m_columnDisplayMap;

    QHash<int, QByteArray> m_roleNames;
    QHash<Object*, int> m_checkStatus;
    QString m_indexKey;
    QHash<QString, Object*> m_indexData;

    struct ErrorData {
        int error = 0;
        QString errorString = tr("No Error");
    } error;
};

} // namespace QtKit

#endif // TREEMODEL_H
