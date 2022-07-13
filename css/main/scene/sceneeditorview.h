#ifndef SCENEEDITORVIEW_H
#define SCENEEDITORVIEW_H

#include <QMap>
#include <QWidget>
#include "httpclient.h"

namespace Ui {
class SceneEditorView;
}

class QTreeWidgetItem;
class SceneEditorView : public QWidget
{
    Q_OBJECT

public:
    explicit SceneEditorView(QWidget *parent = nullptr);
    ~SceneEditorView();

public slots:
    void on_updateProject(const QVariantHash &_prj);
    void on_addProject(const QVariantHash &_prj);
    void on_loadProjects(const QVariantList &_prjs);

    void on_loadFrameworks(const QVariantList &_framework);
    void on_loadModules(const QVariantList &_modules);

    void on_result(bool state, const QString& respons);
private:
    Ui::SceneEditorView *ui;
    QMap<QString, QTreeWidgetItem*>  m_projectTopItems;
    HttpClient *m_httpClient;
    QTreeWidgetItem *m_frameTopItem;
    QTreeWidgetItem *m_moduleTopItem;
};

#endif // SCENEEDITORVIEW_H
