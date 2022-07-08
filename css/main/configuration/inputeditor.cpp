#include "inputeditor.h"
#include "ui_inputeditor.h"
#include "systeminfodlg.h"
#include "popupwindow.h"
#include "cpuinfodlg.h"
#include "frameworkinfodlg.h"
#include "moduleinfodlg.h"
#include <QDebug>

const QMap<QString, InputEditor::EditorEm> InputEditor::m_tabEditors = {
    {"tab_system",      InputEditor::System_Editor},
    {"tab_mudule",      InputEditor::Mudule_Editor},
    {"tab_framework",   InputEditor::Framework_Editor},
    {"tab_cpu",         InputEditor::Cpu_Editor}
};

const QMap<InputEditor::EditorEm, QList<QPair<QString, QString>>> InputEditor::m_editorHeaders = {
    {InputEditor::System_Editor,        {{"sys_version", QStringLiteral("操作系统版本")},
                                         {"desc", QStringLiteral("描述信息")}}},

    {InputEditor::Cpu_Editor,           {{"cpu", QStringLiteral("CPU型号")},
                                         {"desc",QStringLiteral("描述信息")}}},

    {InputEditor::Mudule_Editor,        {{"name", QStringLiteral("名称")},
                                         {"version", QStringLiteral("版本")},
                                         {"sys_version", QStringLiteral("系统版本")},
                                         {"cpu", QStringLiteral("CPU型号")},
                                         {"type", QStringLiteral("类型")}}},

    {InputEditor::Framework_Editor,     {{"name", QStringLiteral("名称")},
                                         {"version", QStringLiteral("版本")},
                                         {"sys_version",QStringLiteral("系统版本")},
                                         {"cpu", QStringLiteral("CPU型号")},
                                         {"type", QStringLiteral("类型")}}}
};

InputEditor::InputEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputEditor)
{
    ui->setupUi(this);

    m_editorEm = m_tabEditors.value(parent->objectName());
    m_model = new TableModel;
    m_model->updateHeaderData(m_editorHeaders.value(m_editorEm));
    ui->tableView->setModel(m_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->pushButton_add, &QPushButton::clicked, this, [=](){

        if (m_editorEm == System_Editor){
            SystemInfoDlg *_sysDlg = new SystemInfoDlg(this);
            PopupWindow::exec(_sysDlg, QStringLiteral("系统版本信息配置"), false, false, false);
        }
        else if (m_editorEm == Cpu_Editor){
            CpuInfoDlg *_cpuDlg = new CpuInfoDlg(this);
            PopupWindow::exec(_cpuDlg, QStringLiteral("CPU信息配置"), false, false, false);
        }
        else if (m_editorEm == Framework_Editor){
            FrameworkInfoDlg *_frameDlg = new FrameworkInfoDlg(this);
            PopupWindow::exec(_frameDlg, QStringLiteral("框架信息配置"), false, false, false);
        }
        else if (m_editorEm == Mudule_Editor){
            ModuleInfoDlg *_moduleDlg = new ModuleInfoDlg(this);
            PopupWindow::exec(_moduleDlg, QStringLiteral("模块信息配置"), false, false, false);
        }
    });

    connect(ui->pushButton_del, &QPushButton::clicked, this, [=](){
        // 删除module 选中数据
    });

    connect(ui->pushButton_edit, &QPushButton::clicked, this, [=](){
        // 编辑module 选中数据

        if (m_editorEm == System_Editor){
            SystemInfoDlg *_sysDlg = new SystemInfoDlg(this);
            PopupWindow::exec(_sysDlg, QStringLiteral("系统版本信息编辑"), false, false, false);
        }
        else if (m_editorEm == Cpu_Editor){
            CpuInfoDlg *_cpuDlg = new CpuInfoDlg(this);
            PopupWindow::exec(_cpuDlg, QStringLiteral("CPU信息编辑"), false, false, false);
        }
        else if (m_editorEm == Framework_Editor){
            FrameworkInfoDlg *_frameDlg = new FrameworkInfoDlg(this);
            PopupWindow::exec(_frameDlg, QStringLiteral("框架信息编辑"), false, false, false);
        }
        else if (m_editorEm == Mudule_Editor){
            ModuleInfoDlg *_moduleDlg = new ModuleInfoDlg(this);
            PopupWindow::exec(_moduleDlg, QStringLiteral("模块信息编辑"), false, false, false);
        }
    });
}

InputEditor::~InputEditor()
{
    delete ui;
}
