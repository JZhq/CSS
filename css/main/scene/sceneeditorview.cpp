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
#include <QDrag>
#include <QMimeData>
#include <QVariantHash>
#include <QToolButton>
#include <QShortcut>
#include <QUndoStack>
#include <QMessageBox>
#include <common/configureglobal.h>
#include <QTimer>
#include <QFileDialog>

SceneEditorView::SceneEditorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SceneEditorView),
    m_httpClient(NULL),
    m_projectCustromMenu(NULL),
    m_resourceCustroMenu(NULL),
    m_graphView(NULL),
    m_graphScene(NULL),
    m_curSelectItme(NULL)

{
    ui->setupUi(this);
    QList<int> verticalSizes = {900, 300};
    ui->splitter->setSizes(verticalSizes);

    QList<int> horizontalSizes = {300, 900};
    ui->splitter_2->setSizes(horizontalSizes);

    ui->treeWidget_project->setHeaderLabel(u8"工程树");
    ui->treeWidget_project->setColumnCount(1);
    ui->treeWidget_project->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->treeWidget_unit->setHeaderLabel(u8"版本列表");
    ui->treeWidget_unit->setColumnCount(1);
    ui->treeWidget_unit->setContextMenuPolicy(Qt::CustomContextMenu);
    // ui->treeWidget_unit->setDragEnabled(true);
    ui->treeWidget_unit->viewport()->installEventFilter(this);
    installEventFilter(this);

    setMouseTracking(true);
    ui->treeWidget_unit->setMouseTracking(true);

    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)));
    m_httpClient->queryProjectList();
    m_httpClient->cpuConfigQueryList();
    QTimer::singleShot(50, [=](){
        m_httpClient->opSystemConfigQueryList();
    });
    QTimer::singleShot(200, [=](){
        m_httpClient->frameworkInfoQueryList();
    });
    QTimer::singleShot(200, [=](){
        m_httpClient->moduleInfoQueryList();
    });

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

    connect(m_graphScene, &GraphicsScene::itemChanged, this, [=](const QVariantHash& d, int edt){
        if (d.keys().contains("name")){
            on_setProjectFramwork(d, edt);
        }else{
            on_setProjectModule(d, edt);
        }
    });

    m_undoAction = m_graphScene->getUndoStack()->createUndoAction(this, tr("&Undo"));
    QShortcut *undoShortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this);
    QObject::connect(undoShortCut, &QShortcut::activated, this, [this] {
        m_undoAction->trigger();
    });

    m_redoAction = m_graphScene->getUndoStack()->createRedoAction(this, tr("&Redo"));
    QShortcut *redoShortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y), this);
    QObject::connect(redoShortCut, &QShortcut::activated, this, [this] {
        m_redoAction->trigger();
    });

    m_graphView->installUndoAction(m_graphScene->getUndoStack(), m_undoAction, m_redoAction);
    m_graphView->installOperataScene(m_graphScene);

    QVBoxLayout *m_layout = new QVBoxLayout();
    m_layout->setSpacing(5);
    toolBar = new QToolBar;
    initToolBar(toolBar, m_graphScene);
    m_layout->addWidget(toolBar);
    m_layout->addWidget(m_graphView);
    ui->frame->setLayout(m_layout);
}

void SceneEditorView::initToolBar(QToolBar *bar, GraphicsScene *scene)
{
    QIcon icon;
    toolBar->addSeparator();
    QToolButton *_line = new QToolButton(this);
    _line->setCheckable(true);
    _line->setToolTip(u8"连线(ALT+L)");
    icon.addPixmap(QPixmap(":/qss/images/line.png"));
    _line->setIcon(icon);
    toolBar->addWidget(_line);
    connect(_line, &QToolButton::clicked, [=](bool checked){
        scene->setMode( checked ? GraphicsScene::InsertLine : GraphicsScene::InsertItem);
    });

    toolBar->addSeparator();

    QToolButton *_save = new QToolButton(this);
    _save->setToolTip(u8"保存(ALT+S)");
    icon.addPixmap(QPixmap(":/qss/images/save.png"));
    _save->setIcon(icon);
    toolBar->addWidget(_save);
    connect(_save, &QToolButton::clicked, [=](){
        scene->onSaveEditTree();
    });

    toolBar->addSeparator();
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

void SceneEditorView::on_setProjectFramwork(const QVariantHash &fram, int opt)
{
    if (m_curSelectItme){
        QString name = fram.value("name").toString();
        if (name.isEmpty())
            return;

        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
        QIcon icon;
        icon.addPixmap(QPixmap(":/qss/images/plugin_node.png"));
        topItem->setIcon(0, icon);
        topItem->setData(0, Qt::UserRole, fram);
        m_curSelectItme->addChild(topItem);
    }
}

void SceneEditorView::on_setProjectModule(const QVariantHash &modu, int opt)
{
    if (m_curSelectItme){
        auto count = m_curSelectItme->childCount();
        for(int i=0; i<count; i++){
            auto childItem = m_curSelectItme->child(i);
            QString name = modu.value("modu_name").toString();
            if (name.isEmpty())
                continue;

            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << name);
            QIcon icon;
            icon.addPixmap(QPixmap(":/qss/images/modu_node.png"));
            item->setIcon(0, icon);
            item->setData(0, Qt::UserRole, modu);
            childItem->addChild(item);
            break;
        }
    }
}

void SceneEditorView::on_loadProjects(const QVariantList &_prjs)
{
    for(auto _prj : _prjs){
        QString name = _prj.toHash().value("projectname").toString();
        if (name.isEmpty())
            continue;

        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
        QIcon icon;
        icon.addPixmap(QPixmap(":/qss/images/project.png"));
        topItem->setIcon(0, icon);
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
    m_downProjectDir= QFileDialog::getExistingDirectory(this, u8"工程下载路径选择");
    if (m_downProjectDir.isEmpty()){
        QMessageBox::information(this, u8"提示信息", u8"工程下载目标路径为空, 请重新配置");
        return;
    }
    if (m_curSelectItme){
        HttpClient *downHttp = new HttpClient("127.0.0.1", 8080, this, SLOT(on_downResult(bool, const QString&)));

        int count = m_curSelectItme->childCount();
        for(int i=0; i<count; i++){
            auto framItem = m_curSelectItme->child(i);
            QVariantHash framData = framItem->data(0, Qt::UserRole).toHash();
            if (framData.keys().contains("name")){
                downHttp->frameworkInfoDownload(framData.value("filename").toString(), m_downProjectDir);
            }

            int modu_count = framItem->childCount();
            for(int m=0; m<modu_count; m++){
                auto moduItem = framItem->child(m);
                QVariantHash moduData = moduItem->data(0, Qt::UserRole).toHash();
                if (moduData.keys().contains("modu_name")){
                    downHttp->moduleInfoDownload(moduData.value("filename").toString(), m_downProjectDir);
                }
            }
        }
    }
}

void SceneEditorView::on_downResult(bool state, const QString &respons)
{
    qDebug() << "on_downResult" << respons;
}

void SceneEditorView::on_loadFrameworks(const QVariantList &_framework)
{
    int count = ui->treeWidget_unit->topLevelItemCount();
    for(int i=0; i<count; i++){
        auto topItem = ui->treeWidget_unit->topLevelItem(i);
        QString _cpu = topItem->data(0, Qt::UserRole).toHash().value("cpuname").toString();
        int sysCount = topItem->childCount();

        for(int j=0; j<sysCount; j++){
            auto sysItem = topItem->child(j);
            QString _sys = sysItem->data(0, Qt::UserRole).toHash().value("systemname").toString();

            QList<QTreeWidgetItem*> frameChildren;
            for(auto fram : _framework){
                QVariantHash dataHash = fram.toHash();
                if (dataHash.value("cpuname").toString() == _cpu && dataHash.value("systemname").toString() ==_sys){
                    QString type = (dataHash.value("type").toInt() == 1) ? u8"动态库": u8"源码" ;
                    QString name = QString("%1(%2)").arg(fram.toHash().value("name").toString()).arg(type);
                    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << name);
                    QIcon icon;
                    icon.addPixmap(QPixmap(":/qss/images/plugin_node.png"));
                    item->setIcon(0, icon);
                    item->setData(0, Qt::UserRole, fram.toHash());
                    frameChildren.append(item);
                }
            }
            if (!frameChildren.isEmpty()){
                   QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << u8"基础框架");
                   item->addChildren(frameChildren);
                   sysItem->addChild(item);
            }
        }
    }
}

void SceneEditorView::on_loadModules(const QVariantList &_modules)
{
    int count = ui->treeWidget_unit->topLevelItemCount();
    for(int i=0; i<count; i++){
        auto topItem = ui->treeWidget_unit->topLevelItem(i);
        QString _cpu = topItem->data(0, Qt::UserRole).toHash().value("cpuname").toString();
        int sysCount = topItem->childCount();

        for(int j=0; j<sysCount; j++){
            auto sysItem = topItem->child(j);
            QString _sys = sysItem->data(0, Qt::UserRole).toHash().value("systemname").toString();

            QList<QTreeWidgetItem*> moduleChildren;
            for(auto modu : _modules){
                QVariantHash dataHash = modu.toHash();
                if (dataHash.value("cpuname").toString() == _cpu && dataHash.value("systemname").toString() ==_sys){
                    QString type = (dataHash.value("type").toInt() == 1) ? u8"动态库": u8"源码" ;
                    QString name = QString("%1(%2)").arg(dataHash.value("modu_name").toString()).arg(type);

                    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << name);
                    QIcon icon;
                    icon.addPixmap(QPixmap(":/qss/images/modu_node.png"));
                    item->setIcon(0, icon);
                    item->setData(0, Qt::UserRole, modu.toHash());
                    moduleChildren.append(item);
                }
            }

            if (!moduleChildren.isEmpty()){
                   QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << u8"基础模块");
                   item->addChildren(moduleChildren);
                   sysItem->addChild(item);
            }
        }
    }
}

void SceneEditorView::on_loadCpuInfos(const QVariantList &_cpus)
{
    for(auto variant : _cpus){
        QVariantHash hash = variant.toHash();
        QString nodename = QString(u8"%1(CPU型号)").arg(hash.value("cpuname").toString());
        QTreeWidgetItem *cupItem = new QTreeWidgetItem(QStringList() << nodename);
        cupItem->setData(0, Qt::UserRole, hash);
        ui->treeWidget_unit->addTopLevelItem(cupItem);
    }
}

void SceneEditorView::on_loadSystemInfos(const QVariantList &_syss)
{
    int count = ui->treeWidget_unit->topLevelItemCount();
    for(int i=0; i<count; i++){
        auto topItem = ui->treeWidget_unit->topLevelItem(i);
        for(auto variant : _syss){
            QVariantHash hash = variant.toHash();
            QString nodename = QString(u8"%1(系统版本)").arg(hash.value("systemname").toString());
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << nodename);
            item->setData(0, Qt::UserRole, hash);
            topItem->addChild(item);
        }
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
                // qDebug() <<" on_result  " <<respons << resultSet.size();
                if (!resultSet.isEmpty()){
                    QStringList keys = resultSet.value(0).toHash().keys();
                    if (keys.contains("projectname")){
                        on_loadProjects(resultSet);
                    }
                    if (keys.contains("modu_name")){
                        on_loadModules(resultSet);
                    }
                    if (keys.contains("cpuname") && (!keys.contains("modu_name") && !keys.contains("name") )){
                        on_loadCpuInfos(resultSet);
                    }
                    if (keys.contains("systemname") &&(!keys.contains("modu_name") && !keys.contains("name") )){
                        on_loadSystemInfos(resultSet);
                    }
                    if (keys.contains("name")){
                        on_loadFrameworks(resultSet);
                    }
                }
            }
        }
    }
}


void SceneEditorView::on_dataChanged()
{

}

bool SceneEditorView::eventFilter(QObject *watched, QEvent *event)
{
    if ( watched != ui->treeWidget_unit->viewport() )
        return  QWidget::eventFilter(watched, event);

    QMouseEvent *m_eve = static_cast<QMouseEvent *>(event);
    if (m_eve->type() == QEvent::MouseButtonPress ){
        if (m_eve->buttons() & Qt::LeftButton){
            m_dragPos = m_eve->pos();
        }
    }
    else if (m_eve->type() == QEvent::MouseMove){
        if (m_eve->buttons() & Qt::LeftButton){
            if ((m_eve->pos() - m_dragPos).manhattanLength() < QApplication::startDragDistance())
                return false;
            m_curSelectItme = ui->treeWidget_project->currentItem();
            if (m_curSelectItme == nullptr){
                QMessageBox::information(NULL, u8"提示",  u8"未选中工程树");
                return false;
            }
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;
            QTreeWidgetItem *item = ui->treeWidget_unit->currentItem();
            if(item == nullptr)
                return false;
            QVariant itemData = item->data(0,Qt::UserRole);
            if (itemData.toHash().keys().contains("name")){
                mimeData->setText(FrameworkNode);
            }
            else if(itemData.toHash().keys().contains("modu_name")){
                mimeData->setText(ModuleNode);
            }
            mimeData->setImageData(itemData);
            drag->setMimeData(mimeData);
            Qt::DropAction resultAction = drag->exec(Qt::MoveAction);
            if(resultAction == Qt::MoveAction){
                if(drag)
                {
                    delete drag;
                    drag = NULL;
                }
            }
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
