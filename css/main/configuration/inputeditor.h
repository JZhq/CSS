#ifndef INPUTEDITOR_H
#define INPUTEDITOR_H

#include <QMap>
#include <tablemodel.h>
#include "editorbase.h"

namespace Ui {
class InputEditor;
}

class InputEditor : public EditorBase
{
    Q_OBJECT
public:
    enum EditorEm{
        System_Editor,
        Cpu_Editor,
        Mudule_Editor,
        Framework_Editor
    };
    //Q_PROPERTY(EditorEm editor READ editor WRITE setEditor NOTIFY editorChanged)

public slots:
    void on_result(bool state, const QString& respons);
public:

    explicit InputEditor(QWidget *parent = nullptr,QVariantHash d=QVariantHash(), EditorMode m=New);
    ~InputEditor();

    static const QMap<QString, EditorEm> m_tabEditors;
    static const QMap<EditorEm, QList<QPair<QString, QString> >> m_editorHeaders;
private:
    Ui::InputEditor *ui;
    EditorEm m_editorEm;
    TableModel *m_model;
};

#endif // INPUTEDITOR_H
