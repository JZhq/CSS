#include "topsuspendwidget.h"
#include <QToolButton>
#include <QHBoxLayout>
#include <QACtion>
#include <QUndoStack>

TopSuspendWidget::TopSuspendWidget(QWidget *parent) : QWidget(parent),
    m_undoBtn(Q_NULLPTR),
    m_redoBtn(Q_NULLPTR)
{
    initializeUi();
}

void TopSuspendWidget::installUndoAction(QUndoStack* stack, QAction *undoAct, QAction *redoAct)
{
    m_undoStack = stack;
    m_undoAction = undoAct;
    m_redoAction = redoAct;
    handleEvents();
}

void TopSuspendWidget::initializeUi()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QPixmap pix;
    QIcon icon;
    QHBoxLayout *layout = new QHBoxLayout;

    // UNDO
    m_undoBtn = new QToolButton(this);
    m_undoBtn->setToolTip("Undo(Ctrl+Z)");
    pix.load(":/resources/undo.png");
    icon.addPixmap(pix);
    m_undoBtn->setIcon(icon);
    m_undoBtn->setIconSize(QSize(32,32));
    m_undoBtn->setMinimumSize(QSize(40,40));
    m_undoBtn->setCheckable(false);
    layout->addWidget(m_undoBtn);
    m_undoBtn->setAutoRaise(true);
    m_undoBtn->setEnabled(false);

    // REDO
    m_redoBtn = new QToolButton(this);
    m_redoBtn->setToolTip("Redo(Ctrl+Y)");
    pix.load(":/resources/redo.png");
    icon.addPixmap(pix);
    m_redoBtn->setIcon(icon);
    m_redoBtn->setIconSize(QSize(32,32));
    m_redoBtn->setMinimumSize(QSize(40,40));
    m_redoBtn->setCheckable(false);
    layout->addWidget(m_redoBtn);
    m_redoBtn->setAutoRaise(true);
    m_redoBtn->setEnabled(false);

    this->setLayout(layout);
}

void TopSuspendWidget::handleEvents()
{
    connect(m_undoBtn, &QToolButton::clicked, m_undoAction, &QAction::trigger);
    connect(m_redoBtn, &QToolButton::clicked, m_redoAction, &QAction::trigger);
    connect(m_undoStack, &QUndoStack::canUndoChanged, this, [this](bool canUndo) {
        m_undoBtn->setEnabled(canUndo);
    });
    connect(m_undoStack, &QUndoStack::canRedoChanged, this, [this](bool canRedo) {
        m_redoBtn->setEnabled(canRedo);
    });
}
