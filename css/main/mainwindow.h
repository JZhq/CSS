#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class InputInfoWidget;
class SceneEditorView;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initMenue();
    void setHandle();

public slots:
    void on_editorDataChanged();

private:
    Ui::MainWindow      *ui;
    QAction             *m_createProject = nullptr;
//    QAction             *m_importProject = nullptr;
//    QAction             *m_exportProject = nullptr;

    QAction             *m_scencConfig = nullptr;
    QAction             *m_inputConfig = nullptr;
    QAction             *m_userConfig = nullptr;

    InputInfoWidget     *m_input = nullptr;
    SceneEditorView     *m_centralView = nullptr;
};
#endif // MAINWINDOW_H
