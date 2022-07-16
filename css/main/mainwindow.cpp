#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QDebug>
#include <QLayout>
#include "configuration/inputinfowidget.h"
#include "scene/sceneeditorview.h"
#include "projectwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    initMenue();
    setHandle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMenue()
{
    m_createProject = new QAction(QIcon(), QStringLiteral("新建工程"), this);
    QList<QAction*> actions = {m_createProject/*, m_openProject, m_closeProject*/};

    ui->menu_file->addActions(actions);

    m_scencConfig  = new QAction(QIcon(), QStringLiteral("组合场景"), this);
    m_inputConfig  = new QAction(QIcon(), QStringLiteral("信息录入"), this);

    QList<QAction*> editActs = { m_inputConfig, m_scencConfig};
    ui->menu_edit->addActions(editActs);

    m_userConfig = new QAction(QIcon(), QStringLiteral("用户信息管理"), this);
    QList<QAction*> toolActs = {m_userConfig};
    ui->menu_tool->addActions(toolActs);
}

void MainWindow::setHandle()
{
    m_centralView = new SceneEditorView(this);
    setCentralWidget(m_centralView);
    connect(ui->menu_file, &QMenu::triggered, this, [=](QAction *act){
        if (m_centralView){
            takeCentralWidget();
            setCentralWidget(m_centralView);
        }
        if (act == m_createProject){
            ProjectWidget *project = new ProjectWidget();
            if (m_centralView){
                connect(project, &ProjectWidget::addproject, m_centralView, &SceneEditorView::on_addProject);
            }
            project->show();
        }
    });

    connect(ui->menu_edit, &QMenu::triggered, this, [=](QAction *act){
        if (act == m_scencConfig){
            if (m_input){
                m_input->deleteLater();
                delete m_input;
                m_input = nullptr;
            }
            if (m_centralView){
                takeCentralWidget();
                setCentralWidget(m_centralView);
            }
        }
        else if (act == m_inputConfig){
            if (m_input == nullptr){
                m_input = new InputInfoWidget();
                connect(m_input, &InputInfoWidget::inputEditorDataChanged, this, &MainWindow::on_editorDataChanged);
                takeCentralWidget();
                setCentralWidget(m_input);
            }
        }
    });
    connect(ui->menu_tool, &QMenu::triggered, this, [=](QAction *act){
        if (act == m_userConfig){
        }
    });


}

void MainWindow::on_editorDataChanged()
{
    qDebug() << "on_editorDataChanged::";
}

