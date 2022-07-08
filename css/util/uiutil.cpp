#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QStackedWidget>
#include <QSpacerItem>
#include <QGridLayout>
#include <QTableView>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QModelIndex>
#include <QShortcut>
#include <QProxyStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>


#include "uiutil.h"
#include "config.h"

void UiUtil::openFile(const QString& fileName)
{
    QString urlPath = "file:///" + fileName;
    QDesktopServices::openUrl(QUrl(urlPath, QUrl::TolerantMode));
}

void UiUtil::loadQss()
{
    QStringList qssFileNames = Config::getInstance()->getQssFiles();
    QString qss;

    for (const QString &name : qssFileNames) {
        qDebug().noquote() << QString("Loading QSS file: %1").arg(name);

        QFile file(qApp->applicationDirPath() + "/" + name);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug().noquote() << QString("Error: Loading QSS file: %1 failed").arg(name);
            continue;
        }

        qss.append(file.readAll()).append("\n");
        file.close();
    }

    if (!qss.isEmpty()) {
        qApp->setStyleSheet(qss);
    }
}

void UiUtil::updateQss(QWidget* widget)
{
    widget->setStyleSheet("/**/");
}

void UiUtil::setWidgetPaddingAndSpacing(QWidget *widget, int padding, int spacing)
{
    // 设置 Widget 的 padding 和 spacing
    QLayout *layout = widget->layout();

    if (NULL != layout) {
        layout->setContentsMargins(padding, padding, padding, padding);
        layout->setSpacing(spacing);
    }
}

void UiUtil::centerWindow(QWidget *window)
{
    // This doesn't show the widget on the screen since you don't relinquish control back to the queue
    // until the hide() happens. In between, the invalidate() computes the correct positions.
    window->show();
    window->layout()->invalidate();
    window->hide();

    QSize size = qApp->desktop()->availableGeometry().size() - window->size();
    int x = qMax(0, size.width() / 2);
    int y = qMax(0, size.height() / 2);
    window->move(x, y);
}

QWidget* UiUtil::findWindow(QWidget *w)
{
    QWidget *p = w;

    while (NULL != p->parentWidget()) {
        p = p->parentWidget();
    }

    return p;
}

/**
 * 把 widget 加入到 stacked widget 里, 可以设置向四个方向的伸展
 */
void UiUtil::addWidgetIntoStackedWidget(QWidget *widget, QStackedWidget *stackedWidget,
                                        bool toLeft,
                                        bool toTop,
                                        bool toRight,
                                        bool toBottom) {
    // 使用 widget 居左上
    QGridLayout *layout = new QGridLayout();

    QSpacerItem *spacer = NULL;
    if (!toLeft) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(spacer, 1, 0);
    }

    if (!toTop) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer, 0, 1);
    }

    if (!toRight) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(spacer, 1, 2);
    }

    if (!toBottom) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer, 2, 1);
    }

    QWidget *container = new QWidget();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget, 1, 1);
    container->setLayout(layout);
    stackedWidget->addWidget(container);
}

void UiUtil::setCurrentWidgetOfStackedWidget(QWidget *widget, QStackedWidget *stackedWidget) {
    for (; NULL != widget; widget = widget->parentWidget()) {
        if (widget->parentWidget() == stackedWidget) {
            stackedWidget->setCurrentWidget(widget);
            break;
        }
    }
}
