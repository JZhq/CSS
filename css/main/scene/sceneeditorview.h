#ifndef SCENEEDITORVIEW_H
#define SCENEEDITORVIEW_H

#include <QWidget>

namespace Ui {
class SceneEditorView;
}

class SceneEditorView : public QWidget
{
    Q_OBJECT

public:
    explicit SceneEditorView(QWidget *parent = nullptr);
    ~SceneEditorView();

private:
    Ui::SceneEditorView *ui;
};

#endif // SCENEEDITORVIEW_H
