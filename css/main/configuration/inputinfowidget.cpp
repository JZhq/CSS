#include "inputinfowidget.h"
#include "ui_inputinfowidget.h"

InputInfoWidget::InputInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputInfoWidget)
{
    ui->setupUi(this);
}

InputInfoWidget::~InputInfoWidget()
{
    delete ui;
}
