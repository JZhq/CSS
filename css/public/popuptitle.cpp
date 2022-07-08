#include "popuptitle.h"
#include "ui_popuptitle.h"

#include <QDebug>

class PopupTitlePrivate
{
public:
    PopupTitlePrivate(PopupTitle *owner) :
        popupTitle(owner)
    {
        isModifiable = false;
    }
    PopupTitle *popupTitle;
    QString title; // 标题名（若有 [*] 也不会保存，仅保留 [*] 前面的字符串）
    bool isModifiable; // 若标题名初始化时有 [*] 则为 true，否则为 false
};

PopupTitle::PopupTitle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PopupTitle),
    d(new PopupTitlePrivate(this))
{
    initializeUi();
    handleEvents();
}

PopupTitle::~PopupTitle()
{
    delete ui;
}

void PopupTitle::initializeUi()
{
    ui->setupUi(this);
    ui->restoreButton->hide();

}

void PopupTitle::handleEvents()
{
    connect(ui->minButton, &QPushButton::clicked, [this] {
        emit signalMin();
    });

    connect(ui->maxButton, &QPushButton::clicked, [this] {
       ui->maxButton->hide();
       ui->restoreButton->show();
       emit signalMax();
    });

    connect(ui->restoreButton, &QPushButton::clicked, [this] {
        ui->restoreButton->hide();
        ui->maxButton->show();
        emit signalRestore();
    });

    connect(ui->closeButton, &QPushButton::clicked, [this] {
        emit signalClose();
    });
}

QString PopupTitle::title()
{
    return ui->titleLabel->text();
}

void PopupTitle::setTitle(const QString& title)
{
    // 标题尾部为 [*] 的字符串是需要根据窗口是否变化来显示 * 的
    if (title.endsWith(" [*]")) {
        d->title = title.left(title.length() - 4);
        ui->titleLabel->setText(d->title);
        d->isModifiable = true;
    } else {
        ui->titleLabel->setText(title);
        d->isModifiable = false;
    }

}

void PopupTitle::setIcon(const QString &icon)
{
//    const QString style = QString("QLabel{border: none; padding: 0;margin: 0; border-width: 0 0 0 0; border-image: url(%1)} 0 0 0 0 stretch stretch }").arg(icon);
//    ui->iconLabel->setStyleSheet(style);
    ui->iconLabel->setVisible(false);
}

void PopupTitle::setButtonsVisibale(bool min, bool max, bool close)
{
    ui->minButton->setVisible(min);
    ui->maxButton->setVisible(max);
    ui->closeButton->setVisible(close);
}

void PopupTitle::setTitleBarModified(bool modified)
{
    if (d->isModifiable) {
        QString title = modified ? d->title + " *" : d->title;
        ui->titleLabel->setText(title);
    }
}


