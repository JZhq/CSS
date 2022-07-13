#include "sceneeditorview.h"
#include "ui_sceneeditorview.h"
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

SceneEditorView::SceneEditorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SceneEditorView),
    m_httpClient(NULL),
    m_frameTopItem(NULL),
    m_moduleTopItem(NULL)

{
    ui->setupUi(this);
    QList<int> verticalSizes = {800, 300};
    ui->splitter->setSizes(verticalSizes);

    QList<int> horizontalSizes = {300, 800};
    ui->splitter_2->setSizes(horizontalSizes);

    ui->treeWidget_project->setHeaderLabel(u8"配置树");
    ui->treeWidget_unit->setHeaderLabel(u8"资源树");

    m_frameTopItem = new QTreeWidgetItem(QStringList() << u8"框架版本");
    ui->treeWidget_unit->addTopLevelItem(m_frameTopItem);

    m_moduleTopItem = new QTreeWidgetItem(QStringList() << u8"模块版本");
    ui->treeWidget_unit->addTopLevelItem(m_moduleTopItem);

    m_httpClient = new HttpClient("127.0.0.1", 8080, this, SLOT(on_result(bool, const QString&)));
    m_httpClient->queryProjectList();
    m_httpClient->frameworkInfoQueryList();
    m_httpClient->moduleInfoQueryList();
}

SceneEditorView::~SceneEditorView()
{
    delete ui;
}

void SceneEditorView::on_updateProject(const QVariantHash &project)
{

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
        ui->treeWidget_project->addTopLevelItem(topItem);
        m_projectTopItems[name] = topItem;
    }
}

void SceneEditorView::on_loadFrameworks(const QVariantList &_framework)
{
    for(auto fram : _framework){
        QString name = fram.toHash().value("name").toString();
        if (name.isEmpty())
            continue;

        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << name);
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
        m_moduleTopItem->addChild(topItem);
    }
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
