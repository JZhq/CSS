#ifndef TOPSUSPENDWIDGET_H
#define TOPSUSPENDWIDGET_H

#include <QWidget>

class QToolButton;
class QUndoStack;

//!
//! \brief view顶部右上角redo、undo button
//!
class TopSuspendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopSuspendWidget(QWidget *parent = nullptr);
    void installUndoAction(QUndoStack* stack, QAction* undoAct, QAction* redoAct);

private:
    void initializeUi();
    void handleEvents();

signals:

public slots:

private:
    QToolButton *m_undoBtn;
    QToolButton *m_redoBtn;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QUndoStack *m_undoStack;
};

#endif // TOPSUSPENDWIDGET_H
