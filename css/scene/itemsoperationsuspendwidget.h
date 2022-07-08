#ifndef ITEMSOPERATIONSUSPENDWIDGET_H
#define ITEMSOPERATIONSUSPENDWIDGET_H

#include <QWidget>

class QToolButton;
class QGraphicsScene;

//!
//! \brief 顶部‘对齐’功能的button
//!
class ItemsOperationSuspendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ItemsOperationSuspendWidget(QWidget *parent = nullptr);
    void setScene(QGraphicsScene* scene);

private:
    void initializeUi();
    void handleEvents();

private:
    QGraphicsScene *m_scene;
    QToolButton *m_alignLeftBtn;    // 左对齐
    QToolButton *m_alignRightBtn;   // 右对齐
    QToolButton *m_alignTopBtn;     // 顶部对齐
    QToolButton *m_alignBotBtn;     // 底部对齐

    QToolButton *m_alignVerSpacingBtn; // 水平间距相等
    QToolButton *m_alignHozSpacingBtn; // 垂直间距相等
};

#endif // ITEMSOPERATIONSUSPENDWIDGET_H
