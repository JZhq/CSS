#ifndef SCENEEDITORVIEW_H
#define SCENEEDITORVIEW_H

#include <QMap>
#include <QWidget>
#include "httpclient.h"
#include <graphicsview.h>
#include <graphicsscene.h>
#include <QToolBar>

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
    void initToolBar(QToolBar *bar, GraphicsScene *scene);

public slots:
    void on_updateProject(const QVariantHash &_prj);
    void on_addProject(const QVariantHash &_prj);
    void on_setProjectFramwork(const QVariantHash &fram, int opt = 0);
    void on_setProjectModule(const QVariantHash &modu, int opt = 0);
    void on_loadProjects(const QVariantList &_prjs);
    void on_delProject(const QVariantHash &_prj);
    void on_downloadProject();

    void on_loadFrameworks(const QVariantList &_framework);
    void on_loadModules(const QVariantList &_modules);
    void on_loadCpuInfos(const QVariantList &_cpus);
    void on_loadSystemInfos(const QVariantList &_syss);
    void on_updateResModule(const QVariantHash &_d);
    void on_detailResModule(const QVariantHash &_d);

    void on_result(bool state, const QString& respons);
    void on_downResult(bool state, const QString& respons);

    void on_dataChanged();

    void on_appendLog(const QString &log, int level);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::SceneEditorView *ui;
    QMap<QString, QTreeWidgetItem*>  m_projectTopItems;
    HttpClient *m_httpClient;
    QMenu *m_projectCustromMenu;
    QMenu *m_resourceCustroMenu;
    GraphicsView *m_graphView;
    GraphicsScene *m_graphScene;
    QPoint m_dragPos;
    QToolBar *toolBar;

    QAction *m_undoAction;
    QAction *m_redoAction;

    QTreeWidgetItem *m_curSelectItme;
    QString m_downProjectDir;
};

#endif // SCENEEDITORVIEW_H
