#ifndef SCENEEDITORVIEW_H
#define SCENEEDITORVIEW_H

#include <QMap>
#include <QWidget>
#include "httpclient.h"
#include <graphicsview.h>
#include <graphicsscene.h>

namespace Ui {
class SceneEditorView;
}

class QTreeWidgetItem;
class QMenu;
class SceneEditorView : public QWidget
{
    Q_OBJECT

public:
    explicit SceneEditorView(QWidget *parent = nullptr);
    ~SceneEditorView();

    void initCustromMenu();
    void initScene();

public slots:
    void on_updateProject(const QVariantHash &_prj);
    void on_addProject(const QVariantHash &_prj);
    void on_loadProjects(const QVariantList &_prjs);
    void on_delProject(const QVariantHash &_prj);
    void on_downloadProject();

    void on_loadFrameworks(const QVariantList &_framework);
    void on_loadModules(const QVariantList &_modules);
    void on_updateResModule(const QVariantHash &_d);
    void on_detailResModule(const QVariantHash &_d);

    void on_result(bool state, const QString& respons);

    void on_dataChanged();

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);


private:
    Ui::SceneEditorView *ui;
    QMap<QString, QTreeWidgetItem*>  m_projectTopItems;
    HttpClient *m_httpClient;
    QTreeWidgetItem *m_frameTopItem;
    QTreeWidgetItem *m_moduleTopItem;
    QMenu *m_projectCustromMenu;
    QMenu *m_resourceCustroMenu;
    GraphicsView *m_graphView;
    GraphicsScene *m_graphScene;

};

#endif // SCENEEDITORVIEW_H
