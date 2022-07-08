#include "sceneeditorview.h"
#include "ui_sceneeditorview.h"

SceneEditorView::SceneEditorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SceneEditorView)
{
    ui->setupUi(this);
    QList<int> verticalSizes = {800, 300};
    ui->splitter->setSizes(verticalSizes);

    QList<int> horizontalSizes = {300, 800};
    ui->splitter_2->setSizes(horizontalSizes);
}

SceneEditorView::~SceneEditorView()
{
    delete ui;
}
