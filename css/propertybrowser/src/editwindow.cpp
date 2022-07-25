#include "editwindow.h"
#include "ui_editwindow.h"

EditWindow::EditWindow(QWidget *parent) :
    PopupWidget(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);
    resize(600,400);
}

EditWindow::~EditWindow()
{
    delete ui;
}

bool EditWindow::afterConfirmButtonClicked()
{
    emit sendBiaoDaShi(ui->textEdit->toPlainText());
    return true;
}

bool EditWindow::afterCancelButtonClicked()
{
    return true;
}

void EditWindow::setText(const QString &text)
{
    ui->textEdit->setText(text);
}
