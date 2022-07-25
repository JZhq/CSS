#include "editwindows.h"
#include "ui_editwindows.h"

EditWindows::EditWindows(QWidget *parent) :
    PopupWidget(parent),
    ui(new Ui::EditWindows)
{
    ui->setupUi(this);
}

EditWindows::~EditWindows()
{
    delete ui;
}

bool EditWindows::afterConfirmButtonClicked()
{
    return false;
}

bool EditWindows::afterCancelButtonClicked()
{
    return false;
}
