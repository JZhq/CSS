#include "sceneeditorview.h"
#include "ui_sceneeditorview.h"
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QMenu>
#include <QAction>
#include <QTreeWidget>
#include <projectwidget.h>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>

SceneEditorView::SceneEditorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SceneEditorView),
    m_httpClient(NULL),
    m_frameTopItem(NULL),
    m_moduleTopItem(NULL),
    m_projectCustromMenu(NULL),
    m_resourceCustroMenu(NULL),
    m_graphView(NULL),
    m_graphScene(NULL)

{
    ui->setupUi(this);
    QList<int> verticalSizes = {800, 300};
    ui->splitter->setSizes(verticalSizes);

    QList<int> horizontalSizes = {300, 800};
    ui->splitter_2->setSizes(horizontalSizes);

    // ui->treeWidget_project->setHeaderHidden(true);
    ui->treeWidget_project->setHeaderLabel(u8"工程树");
    ui->treeWidget_project->setColumnCount(1);
    ui->treeWidget_project->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget_project->installEventFilter(this);

    // ui->treeWidget_unit->setHeaderHidden(true);
    ui->treeWidget_unit->setHeaderLabel(u8"资源树");
    ui->treeWidget_unit->setColumnCount(1);
    ui->treeWidget_unit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget_unit->installEventFilter(this);


    m_frameTopItem = new QTreeWidgetItem(QStringList() << u8"框架版本");
    ui->treeWidget_unit->addTopLevelItem(m_frameTopItem);

    m_moduleTopItem = new QTreeWidgetItem(QStringList() << u8"模块版本");
    ui->treeWidget_unit->addTopLevelItem(m_moduleTopItem);

    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)));
    m_httpClient->queryProjectList();
    m_httpClient->frameworkInfoQueryList();
    m_httpClient->moduleInfoQueryList();

    initCustromMenu();
    initScene();
}

SceneEditorView::~SceneEditorView()
{
    delete ui;
}

void SceneEditorView::initCustromMenu()
{
    m_projectCustromMenu = new QMenu(this);
    QList<QAction*> prjectMenus= { new QAction(u8"添加",this),
                                   new QAction(u8"删除",this),
                                   new QAction(u8"编辑", this),
                                   new QAction(u8"下载", this)};

    m_projectCustromMenu->addActions(prjectMenus);
    connect(ui->treeWidget_project, &QTreeWidget::customContextMenuRequested, [=](const QPoint &pos)mutable {
        QVariantHash itemUserData;
        auto *item = ui->treeWidget_project->itemAt(pos);
        if (item ){
            m_projectCustromMenu->exec(QCursor::pos());
            itemUserData = item->data(0, Qt::UserRole).toHash();
        }
        connect(m_projectCustromMenu, &QMenu::triggered, this, [=](QAction *action){
            if (action->text() == u8"添加"){
                ProjectWidget *project = new ProjectWidget();
                connect(project, &ProjectWidget::addproject,  [this](const QVariantHash &d){
                    on_addProject(d);
                });
            }
            else if (action->text() == u8"删除"){
                on_delProject(itemUserData);
            }
            else if (action->text() == u8"编辑"){
                on_updateProject(itemUserData);
            }
            else if (action->text() == u8"下载"){
                on_downloadProject();
            }
        });

    });

    m_resourceCustroMenu = new QMenu(this);
    QList<QAction*> unitMenus= { new QAction(u8"查看",this),  new QAction(u8"编辑", this)};
    m_resourceCustroMenu->addActions(unitMenus);
    connect(ui->treeWidget_unit, &QTreeWidget::customContextMenuRequested, [=](const QPoint &pos){
        QVariantHash itemUserData;
        auto *item = ui->treeWidget_project->itemAt(pos);
        if (item){
            m_resourceCustroMenu->exec(QCursor::pos());
            itemUserData = item->data(0, Qt::UserRole).toHash();
        }

        connect(m_resourceCustroMenu, &QMenu::triggered, this, [=](QAction *action){
            if (action->text() == u8"查看"){
                on_detailResModule(itemUserData);
            }
            else if (action->text() == u8"编辑"){
                on_updateResModule(itemUserData);
            }
        });
    });
}

void SceneEditorView::initScene()
{
    m_graphView = new GraphicsView();
    m_graphScene = new GraphicsScene();
    m_graphView->setScene(m_graphScene);
    //    m_graphView->setTopSuspendWidgetVisible(false);
    //    m_graphView->setItemsOperateSuspendWidgetVisible(false);
    QHBoxLayout *m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_graphView);
    ui->frame->setLayout(m_layout);
}

void SceneEditorView::on_updateProject(const QVariantHash &project)
{

    // m_httpClient->updateProject(project.value())
}

void SceneEditorView::on_addProject(const QVariantHash &_prj)
{
    QString name = _prj.value("projectname").toString();
    if (name.isEmpty())
        return;

    QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
    ui->treeWidget_project->addTopLevelItem(topItem);
    m_projectTopItems[name] = topItem;
}

void SceneEditorView::on_loadProjects(const QVariantList &_prjs)
{
    for(auto _prj : _prjs){
        QString name = _prj.toHash().value("projectname").toString();
        if (name.isEmpty())
            continue;

        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
        topItem->setData(0, Qt::UserRole, _prj.toHash());
        ui->treeWidget_project->addTopLevelItem(topItem);
        m_projectTopItems[name] = topItem;
    }
}

void SceneEditorView::on_delProject(const QVariantHash &_prj)
{
    if (m_httpClient){
        // todo: 有bug 待修改
        //        QTreeWidgetItemIterator it(ui->treeWidget_project);
        //        while(*it){
        //            auto data = (*it)->data(0, Qt::UserRole).toHash();
        //            if (data.value("projectname") == _prj.value("projectname")){
        //                delete (*it);
        //                break;
        //            }
        //            ++it;
        //        }
        m_httpClient->deleteProject(_prj.value("projectname").toString());
    }
}

void SceneEditorView::on_downloadProject()
{

}

void SceneEditorView::on_loadFrameworks(const QVariantList &_framework)
{
    for(auto fram : _framework){
        QString name = fram.toHash().value("name").toString();
        if (name.isEmpty())
            continue;

        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
        topItem->setData(0, Qt::UserRole, fram.toHash());
        m_frameTopItem->addChild(topItem);
    }
}

void SceneEditorView::on_loadModules(const QVariantList &_modules)
{
    for(auto modu : _modules){
        QString name = modu.toHash().value("modu_name").toString();
        if (name.isEmpty())
            continue;

        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
        topItem->setData(0, Qt::UserRole, modu.toHash());
        m_moduleTopItem->addChild(topItem);
    }
}

void SceneEditorView::on_updateResModule(const QVariantHash &_d)
{

}

void SceneEditorView::on_detailResModule(const QVariantHash &_d)
{

}

void SceneEditorView::on_result(bool state, const QString &respons)
{
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            auto resultSet = doc.object().value("resultset").toVariant().toList();
            if (isLogin){
                qDebug() << respons << resultSet.size();
                QList<QVariantHash> _hashs;
                bool isPrjectRes = false;
                bool isModuleRes = false;
                for(auto set: resultSet){
                    if (set.toHash().keys().contains("projectname")){
                        isPrjectRes = true;
                    }
                    else if (set.toHash().keys().contains("modu_name")){
                        isModuleRes = true;
                    }
                    _hashs << set.toHash();
                }
                if (isPrjectRes)
                    on_loadProjects(resultSet);
                if (isModuleRes)
                    on_loadModules(resultSet);
                if (!isPrjectRes && !isModuleRes)
                    on_loadFrameworks(resultSet);
            }
        }
    }
}

void SceneEditorView::on_dataChanged()
{

}

bool SceneEditorView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != ui->treeWidget_project || watched != ui->treeWidget_unit )
        return  QWidget::eventFilter(watched, event);
    static QPoint dragPos;
    static bool isDrag = false;
    QMouseEvent *m_eve = static_cast<QMouseEvent *>(event);
    if (m_eve->type() == QEvent::MouseButtonPress ){
        if (m_eve->buttons() && Qt::LeftButton){
            dragPos = m_eve->pos();
            isDrag = true;
            return true;
        }
    }
    else if (m_eve->type() == QEvent::MouseButtonRelease){
        if (m_eve->buttons() && Qt::LeftButton){
            isDrag = false;
            return true;
        }
    }
    else if (m_eve->type() == QEvent::MouseMove){
        if (m_eve->buttons() && Qt::LeftButton){

        }
    }

    return true;

}
