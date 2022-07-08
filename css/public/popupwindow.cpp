#include "popupwindow.h"
#include "ui_popupwindow.h"
#include "topwindow.h"
#include "uiutil.h"
#include "popupwidget.h"
#include "popuptitle.h"
#include "messagebox.h"

#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

//!
//! \brief 给静态函数使用，判断窗口内容是否改变
//!
static bool g_isWindowModified = false;

class PopupWindowPrivate
{
public:
    PopupWindowPrivate(PopupWindow *owner, QWidget *w) :
        popupWindow(owner),
        result(false),
        windowModified(false)
    {
        titleBar = new PopupTitle;
        center = w;
    }

    PopupWindow *popupWindow;  // 窗口界面
    QWidget *center;           // 中央界面
    PopupTitle *titleBar;      // 标题栏
    bool result;               // 点击 “确认” 返回 true，反之为 false
    bool windowModified;       // 窗口被改变后返回 true，反之为 false
};

PopupWindow::PopupWindow(QWidget *center,  bool buttonVisible) :
    ui(new Ui::PopupWindow),
    d(new PopupWindowPrivate(this, center))
{
    initializeUi();
    handleEvents();

    ui->buttonWidget->setVisible(buttonVisible);
    ::g_isWindowModified = false;
}

void PopupWindow::initializeUi()
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet(".PopupWindow { background: white; }");

    if (d->center)
        ui->centerLayout->addWidget(d->center);
}

void PopupWindow::handleEvents()
{
    // 点击取消按钮关闭窗口
    connect(ui->cancelButton, &QPushButton::clicked, [this] {
        d->result = false;
        bool isClose = false;
        // 点击取消后，相应的界面进行事后处理
        if (auto w = qobject_cast<PopupWidget*>(d->center)) {
            isClose = w->afterCancelButtonClicked();
        }
        if (isClose) {
            UiUtil::getInstance()->findWindow(this)->close();
        }
    });

    // 点击确定按钮关闭窗口
    connect(ui->confirmButton, &QPushButton::clicked, [this] {
        d->result = true;
        bool isClose = false;
        // 点击确认后，相应的界面进行事后处理
        if (auto w = qobject_cast<PopupWidget*>(d->center)) {
            isClose =  w->afterConfirmButtonClicked();
        }
        if (isClose) {
            UiUtil::getInstance()->findWindow(this)->close();
        }
    });

    // 中央窗口的内容改变了
    PopupWidget *w = qobject_cast<PopupWidget*>(d->center);
    if (w) {
        connect(w, &PopupWidget::signalWindowModified, [this](bool modified) {
           this->setWindowModified(modified);
           d->titleBar->setTitleBarModified(modified);
        });
    }
}

PopupWindow::~PopupWindow()
{
    delete ui;
}

bool PopupWindow::accepted() const
{
    return d->result;
}

PopupTitle *PopupWindow::titleBar() const
{
    return d->titleBar;
}

void PopupWindow::setWindowModified(bool modified)
{
    ::g_isWindowModified = modified;
    d->windowModified = modified;
}

bool PopupWindow::isWindowModified()
{
    return d->windowModified;
}

void PopupWindow::setWindow(TopWindow *window, QWidget *center,
                            PopupTitle *title, bool resizable, bool isDialog)
{
    window->setTitleBar(title);
    window->resize(center->width(), center->height());
    window->setResizable(resizable);
    if (isDialog) {
        window->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint);
        window->setWindowModality(Qt::ApplicationModal);
    } else {
        window->setAttribute(Qt::WA_DeleteOnClose);
    }
    PopupWindow::handleTitleBarEvents(title, window);
}

void PopupWindow::handleTitleBarEvents(PopupTitle *title, TopWindow *window)
{
    QObject::connect(title, &PopupTitle::signalMin, [window]() {
       window->showMinimized();
    });
    QObject::connect(title, &PopupTitle::signalRestore, [window]() {
       window->showNormal();
    });
    QObject::connect(title, &PopupTitle::signalMax, [window]() {
       window->showMaximized();
    });
    QObject::connect(title, &PopupTitle::signalClose, [window]() {
       bool isClose = true;
       if (::g_isWindowModified)
           // isClose = MessageBox::confirm(QStringLiteral("窗口内容已经改变，是否退出？"));
           isClose = (QMessageBox::question(window, QStringLiteral("退出"), QStringLiteral("窗口内容已经改变，是否退出？")) == QMessageBox::Yes);
       if (isClose)
           window->close();
    });
}

void PopupWindow::exec(
          QWidget *w,
          const QString &title,
          bool resizable,
          bool miniBtn,
          bool maxBtn,
          bool closeBtn,
          bool confirmBtn
          )
{
    if(!w)
        return;

    PopupWindow *popupWindow = new PopupWindow(w, confirmBtn);
    TopWindow window(popupWindow);
    PopupTitle *titleBar = popupWindow->titleBar();
    // titleBar->setIcon(title);
    titleBar->setTitle(title);
    titleBar->setButtonsVisibale(miniBtn, maxBtn, closeBtn);
    PopupWindow::setWindow(&window, w, titleBar, resizable, true);
    window.showModal();
}

bool PopupWindow::exec(const QString &title, PopupWidget *w)
{
    if(!w)
        return false;

    PopupWindow *popupWindow = new PopupWindow(w, true);
    TopWindow window(popupWindow);
    PopupTitle *titleBar = popupWindow->titleBar();
    titleBar->setTitle(title);
    titleBar->setButtonsVisibale(false, false, true);
    PopupWindow::setWindow(&window, w, titleBar, false, true);
    window.showModal();
    return popupWindow->accepted();
}

void PopupWindow::show(
            QWidget *w,
            const QString &icon,
            const QString &title,
            bool resizable,
            bool miniBtn,
            bool maxBtn,
            bool closeBtn,
            bool confirmBtn
            )
{
    if(!w)
        return;

    PopupWindow *popupWindow = new PopupWindow(w, confirmBtn);
    TopWindow *window = new TopWindow(popupWindow);
    PopupTitle *titleBar = popupWindow->titleBar();
    titleBar->setIcon(icon);
    titleBar->setTitle(title);
    titleBar->setButtonsVisibale(miniBtn, maxBtn, closeBtn);
    PopupWindow::setWindow(window, w, titleBar, resizable, false);
    window->show();
}

bool PopupWindow::show(const QString &icon, const QString &title, PopupWidget *w)
{
    if(!w)
        return false;

    PopupWindow *popupWindow = new PopupWindow(w, true);
    TopWindow *window = new TopWindow(popupWindow);
    PopupTitle *titleBar = popupWindow->titleBar();
    titleBar->setIcon(icon);
    titleBar->setTitle(title);
    titleBar->setButtonsVisibale(false, false, true);
    PopupWindow::setWindow(window, w, titleBar, false, false);
    window->show();
    return popupWindow->accepted();
}



