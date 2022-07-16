#include "inputeditor.h"
#include "ui_inputeditor.h"
#include "systeminfodlg.h"
#include "popupwindow.h"
#include "cpuinfodlg.h"
#include "frameworkinfodlg.h"
#include "moduleinfodlg.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

const QMap<QString, InputEditor::EditorEm> InputEditor::m_tabEditors = {
    {"tab_system",      InputEditor::System_Editor},
    {"tab_mudule",      InputEditor::Mudule_Editor},
    {"tab_framework",   InputEditor::Framework_Editor},
    {"tab_cpu",         InputEditor::Cpu_Editor}
};

const QMap<InputEditor::EditorEm, QList<QPair<QString, QString>>> InputEditor::m_editorHeaders = {
    {InputEditor::System_Editor,        {{"systemver", QStringLiteral("操作系统版本")},
                                         {"systemname", QStringLiteral("操作系统名称")},
                                         {"systemdesc", QStringLiteral("描述信息")}}},

    {InputEditor::Cpu_Editor,           {{"cpuname", QStringLiteral("CPU型号")},
                                         {"cpudesc",QStringLiteral("描述信息")}}},

    {InputEditor::Mudule_Editor,        {{"modu_name", QStringLiteral("名称")},
                                         {"ver", QStringLiteral("版本")},
                                         {"systemver", QStringLiteral("系统版本")},
                                         {"systemname", QStringLiteral("系统名称")},
                                         {"cpuname", QStringLiteral("CPU型号")},
                                         {"type", QStringLiteral("类型")}}},

    {InputEditor::Framework_Editor,     {{"name", QStringLiteral("名称")},
                                         {"ver", QStringLiteral("版本")},
                                         {"systemver",QStringLiteral("系统版本")},
                                         {"systemname",QStringLiteral("系统名称")},
                                         {"cpuname", QStringLiteral("CPU型号")},
                                         {"type", QStringLiteral("类型")}}}
};

void InputEditor::on_result(bool state, const QString &respons)
{
    if (state){
        auto isLogin = false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respons.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            isLogin = doc.object().value("result").toBool();
            auto resultSet = doc.object().value("resultset").toVariant().toList();
            if (isLogin){
                // qDebug() << respons << resultSet.size();
                QList<QVariantHash> _hashs;
                for(auto set: resultSet){
                    _hashs << set.toHash();
                }
                m_model->updateData(_hashs);
            }
        }
    }
}

InputEditor::InputEditor(QWidget *parent, QVariantHash d, EditorMode m) :
    EditorBase(parent,d,m),
    ui(new Ui::InputEditor),
    m_model(NULL)
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
            connect(_sysDlg, &SystemInfoDlg::addSystemInfo, this, [=](const QVariantHash &hash){
               m_model->appendData(hash);
            });
            PopupWindow::exec(_sysDlg, QStringLiteral("系统版本信息配置"), false, false, false);
        }
        else if (m_editorEm == Cpu_Editor){
            CpuInfoDlg *_cpuDlg = new CpuInfoDlg(this);
            connect(_cpuDlg, &CpuInfoDlg::addCpuInfo, this, [=](const QVariantHash& hash){
                m_model->appendData(hash);
            });
            PopupWindow::exec(_cpuDlg, QStringLiteral("CPU信息配置"), false, false, false);
        }
        else if (m_editorEm == Framework_Editor){
            FrameworkInfoDlg *_frameDlg = new FrameworkInfoDlg(this);
            connect(_frameDlg, &FrameworkInfoDlg::addFrameworkInfoSignal, this, [=](const QVariantHash& hash){
                m_model->appendData(hash);
            });
            PopupWindow::exec(_frameDlg, QStringLiteral("框架信息配置"), false, false, false);
        }
        else if (m_editorEm == Mudule_Editor){
            ModuleInfoDlg *_moduleDlg = new ModuleInfoDlg(this);
            connect(_moduleDlg, &ModuleInfoDlg::addModuleInfo, this, [=](const QVariantHash& hash){
                m_model->appendData(hash);
            });
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
            connect(_sysDlg, &SystemInfoDlg::editorDataChanged, this, &InputEditor::editorDataChanged);
            PopupWindow::exec(_sysDlg, QStringLiteral("系统版本信息编辑"), false, false, false);
        }
        else if (m_editorEm == Cpu_Editor){
            CpuInfoDlg *_cpuDlg = new CpuInfoDlg(this);
            connect(_cpuDlg, &CpuInfoDlg::editorDataChanged, this, &InputEditor::editorDataChanged);
            PopupWindow::exec(_cpuDlg, QStringLiteral("CPU信息编辑"), false, false, false);
        }
        else if (m_editorEm == Framework_Editor){
            FrameworkInfoDlg *_frameDlg = new FrameworkInfoDlg(this);
            connect(_frameDlg, &FrameworkInfoDlg::editorDataChanged, this, &InputEditor::editorDataChanged);
            PopupWindow::exec(_frameDlg, QStringLiteral("框架信息编辑"), false, false, false);
        }
        else if (m_editorEm == Mudule_Editor){
            ModuleInfoDlg *_moduleDlg = new ModuleInfoDlg(this);
            connect(_moduleDlg, &ModuleInfoDlg::editorDataChanged, this, &InputEditor::editorDataChanged);
            PopupWindow::exec(_moduleDlg, QStringLiteral("模块信息编辑"), false, false, false);
        }
    });

    if (m_editorEm == System_Editor){
        m_httpClient->opSystemConfigQueryList();
    }else if (m_editorEm == Cpu_Editor){
        m_httpClient->cpuConfigQueryList();
    }else if (m_editorEm == Mudule_Editor){
        m_httpClient->moduleInfoQueryList();
    }else{
        m_httpClient->frameworkInfoQueryList();
    }

}

InputEditor::~InputEditor()
{
    delete ui;
}
