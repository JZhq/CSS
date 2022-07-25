#include "suspendwidget.h"
#include <QToolButton>
#include <QHBoxLayout>

SuspendWidget::SuspendWidget(QWidget *parent):QWidget(parent),
               m_zoomInBtn(Q_NULLPTR),
               m_zoomOutBtn(Q_NULLPTR)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //设置背景透明
    //setAttribute(Qt::WA_TranslucentBackground, true);
    initUi();
    handleEvents();
}

SuspendWidget::~SuspendWidget()
{

}

void SuspendWidget::initUi()
{
    QHBoxLayout *layout = new QHBoxLayout;

    //缩小
    m_zoomOutBtn = new QToolButton(this);
    QPixmap pix;
    QIcon icon;
    pix.load(":/resources/small_p.png");
    icon.addPixmap(pix);
    m_zoomOutBtn->setIcon(icon);
    m_zoomOutBtn->setIconSize(QSize(32,32));
    m_zoomOutBtn->setMinimumSize(QSize(40,40));
    layout->addWidget(m_zoomOutBtn);
    m_zoomOutBtn->setAutoRaise(true);

    //放大
    m_zoomInBtn = new QToolButton(this);
    layout->addWidget(m_zoomInBtn);
    pix.load(":/resources/big_p.png");
    icon.addPixmap(pix);
    m_zoomInBtn->setIcon(icon);
    m_zoomInBtn->setIconSize(QSize(32,32));
    m_zoomInBtn->setMinimumSize(QSize(40,40));
    m_zoomInBtn->setAutoRaise(true);

//    //快照
//    m_printBtn = new QToolButton(this);
//    layout->addWidget(m_printBtn);
//    pix.load(":/resources/camera_n.png");
//    icon.addPixmap(pix);
//    m_printBtn->setIcon(icon);
//    m_printBtn->setIconSize(QSize(32,32));
//    m_printBtn->setMinimumSize(QSize(40,40));
//    m_printBtn->setAutoRaise(true);
    this->setLayout(layout);
}

void SuspendWidget::handleEvents()
{
    connect(m_zoomOutBtn,&QToolButton::clicked,this,&SuspendWidget::signalZoomOut);

    connect(m_zoomInBtn,&QToolButton::clicked,this,&SuspendWidget::signalZoomIn);

    // connect(m_printBtn,&QToolButton::clicked,this,&SuspendWidget::signalPrintScene);

}
