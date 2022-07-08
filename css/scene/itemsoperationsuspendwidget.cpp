#include "itemsoperationsuspendwidget.h"
#include "itemsoperationtool.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QACtion>

ItemsOperationSuspendWidget::ItemsOperationSuspendWidget(QWidget *parent)
    : m_scene(Q_NULLPTR), QWidget(parent)
{
    initializeUi();
    handleEvents();
}

void ItemsOperationSuspendWidget::setScene(QGraphicsScene *scene)
{
    m_scene = scene;
}

void ItemsOperationSuspendWidget::initializeUi()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QPixmap pix;
    QIcon icon;
    QHBoxLayout *layout = new QHBoxLayout;

    //! left align
    m_alignLeftBtn = new QToolButton(this);
    m_alignLeftBtn->setToolTip(QStringLiteral("左对齐(Q)"));
    m_alignLeftBtn->setShortcut(Qt::Key_Q);
    pix.load("://resources/align_left.png");
    icon.addPixmap(pix);
    m_alignLeftBtn->setIcon(icon);
    m_alignLeftBtn->setIconSize(QSize(32,32));
    m_alignLeftBtn->setMinimumSize(QSize(40,40));
    m_alignLeftBtn->setCheckable(false);
    layout->addWidget(m_alignLeftBtn);
    m_alignLeftBtn->setAutoRaise(true);

    //! right align
    m_alignRightBtn = new QToolButton(this);
    m_alignRightBtn->setToolTip(QStringLiteral("右对齐(R)"));
    m_alignRightBtn->setShortcut(Qt::Key_R);
    pix.load("://resources/align_right.png");
    icon.addPixmap(pix);
    m_alignRightBtn->setIcon(icon);
    m_alignRightBtn->setIconSize(QSize(32,32));
    m_alignRightBtn->setMinimumSize(QSize(40,40));
    m_alignRightBtn->setCheckable(false);
    layout->addWidget(m_alignRightBtn);
    m_alignRightBtn->setAutoRaise(true);

    //! top align
    m_alignTopBtn = new QToolButton(this);
    m_alignTopBtn->setToolTip(QStringLiteral("顶部对齐(E)"));
    m_alignTopBtn->setShortcut(Qt::Key_E);
    pix.load("://resources/align_top.png");
    icon.addPixmap(pix);
    m_alignTopBtn->setIcon(icon);
    m_alignTopBtn->setIconSize(QSize(32,32));
    m_alignTopBtn->setMinimumSize(QSize(40,40));
    m_alignTopBtn->setCheckable(false);
    layout->addWidget(m_alignTopBtn);
    m_alignTopBtn->setAutoRaise(true);

    //! bottom align
    m_alignBotBtn = new QToolButton(this);
    m_alignBotBtn->setToolTip(QStringLiteral("底部对齐(W)"));
    m_alignBotBtn->setShortcut(Qt::Key_W);
    pix.load("://resources/align_bottom.png");
    icon.addPixmap(pix);
    m_alignBotBtn->setIcon(icon);
    m_alignBotBtn->setIconSize(QSize(32,32));
    m_alignBotBtn->setMinimumSize(QSize(40,40));
    m_alignBotBtn->setCheckable(false);
    layout->addWidget(m_alignBotBtn);
    m_alignBotBtn->setAutoRaise(true);

    //! horizon spacing
    m_alignHozSpacingBtn = new QToolButton(this);
    m_alignHozSpacingBtn->setToolTip(QStringLiteral("水平等间距(H)"));
    m_alignHozSpacingBtn->setShortcut(Qt::Key_H);
    pix.load("://resources/spacing_avr_hor.png");
    icon.addPixmap(pix);
    m_alignHozSpacingBtn->setIcon(icon);
    m_alignHozSpacingBtn->setIconSize(QSize(32,32));
    m_alignHozSpacingBtn->setMinimumSize(QSize(40,40));
    m_alignHozSpacingBtn->setCheckable(false);
    layout->addWidget(m_alignHozSpacingBtn);
    m_alignHozSpacingBtn->setAutoRaise(true);

    //! vertical spacing
    m_alignVerSpacingBtn = new QToolButton(this);
    m_alignVerSpacingBtn->setToolTip(QStringLiteral("垂直等间距(V)"));
    m_alignVerSpacingBtn->setShortcut(Qt::Key_V);
    pix.load("://resources/spacing_avr_ver.png");
    icon.addPixmap(pix);
    m_alignVerSpacingBtn->setIcon(icon);
    m_alignVerSpacingBtn->setIconSize(QSize(32,32));
    m_alignVerSpacingBtn->setMinimumSize(QSize(40,40));
    m_alignVerSpacingBtn->setCheckable(false);
    layout->addWidget(m_alignVerSpacingBtn);
    m_alignVerSpacingBtn->setAutoRaise(true);

    this->setLayout(layout);
}

void ItemsOperationSuspendWidget::handleEvents()
{
    connect(m_alignLeftBtn, &QToolButton::clicked, this, [this]() {
        qDebug("Align_Left");
        ItemsOperationToolInstance->processItemAlign(m_scene, ItemsOperationTool::Align_Left);
    });

    connect(m_alignRightBtn, &QToolButton::clicked, this, [this]() {
        qDebug("Align_Right");
        ItemsOperationToolInstance->processItemAlign(m_scene, ItemsOperationTool::Align_Right);
    });

    connect(m_alignTopBtn, &QToolButton::clicked, this, [this]() {
        qDebug("Align_Top");
        ItemsOperationToolInstance->processItemAlign(m_scene, ItemsOperationTool::Align_Top);
    });

    connect(m_alignBotBtn, &QToolButton::clicked, this, [this]() {
        qDebug("Align_Bottom");
        ItemsOperationToolInstance->processItemAlign(m_scene, ItemsOperationTool::Align_Bottom);
    });

    connect(m_alignHozSpacingBtn, &QToolButton::clicked, this, [this]() {
        qDebug("Align_Hor_AvrSpacing");
        ItemsOperationToolInstance->processItemAlignSpacing(m_scene, ItemsOperationTool::Align_Hor_AvrSpacing);
    });

    connect(m_alignVerSpacingBtn, &QToolButton::clicked, this, [this]() {
        qDebug("Align_Ver_AvrSpacing");
        ItemsOperationToolInstance->processItemAlignSpacing(m_scene, ItemsOperationTool::Align_Ver_AvrSpacing);
    });
}
