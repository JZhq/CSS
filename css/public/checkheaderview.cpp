#include "checkheaderview.h"

#include <QCheckBox>
#include <QMouseEvent>
#include <QPainter>

class CheckHeaderViewPrivate
{
public:
    CheckHeaderViewPrivate() :
        pressed(false),
        checked(false),
        tristate(false),
        noChange(false),
        moving(false)
    {

    }

    bool pressed;
    bool checked;
    bool tristate;
    bool noChange;
    bool moving;
};

CheckHeaderView::CheckHeaderView(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation, parent),
    d(new CheckHeaderViewPrivate)
{
    initializeUi();
    handleEvents();
}

CheckHeaderView::~CheckHeaderView()
{
    if (d)
        delete d;
}

void CheckHeaderView::initializeUi()
{
    // setStretchLastSection(true);
    setHighlightSections(false);
    setMouseTracking(true);

    // 响应鼠标
    setSectionsClickable(true);
}

void CheckHeaderView::handleEvents()
{

}

void CheckHeaderView::onStateChanged(int state)
{
    if (state == Qt::PartiallyChecked) {
        d->tristate = true;
        d->noChange = true;
    } else {
        d->noChange = false;
    }

    d->checked = (state != Qt::Unchecked);
    update();
}

void CheckHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (logicalIndex == 0) // checkbox column
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (d->checked)
            option.state |= QStyle::State_Sunken;

        if (d->tristate && d->noChange)
            option.state |= QStyle::State_NoChange;
        else
            option.state |= d->checked ? QStyle::State_On : QStyle::State_Off;
        if (testAttribute(Qt::WA_Hover) && underMouse()) {
            if (d->moving)
                option.state |= QStyle::State_MouseOver;
            else
                option.state &= ~QStyle::State_MouseOver;
        }

        QCheckBox checkBox;
        option.iconSize = QSize(20, 20);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);
        //style()->drawItemPixmap(painter, rect, Qt::AlignCenter, QPixmap(":/images/checkBoxChecked"));
        //style()->drawControl(QStyle::CE_CheckBox, &option, painter, this);
    }
}

void CheckHeaderView::mousePressEvent(QMouseEvent *event)
{
    int nColumn = logicalIndexAt(event->pos());
    if ((event->buttons() & Qt::LeftButton) && (nColumn == 0)) // checkbox column
    {
        d->pressed = true;
    }
    else
    {
        QHeaderView::mousePressEvent(event);
    }
}

void CheckHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    if (d->pressed)
    {
        if (d->tristate && d->noChange)
        {
            d->checked = true;
            d->noChange = false;
        }
        else
        {
            d->checked = !d->checked;
        }

        update();

        Qt::CheckState state = d->checked ? Qt::Checked : Qt::Unchecked;

        emit stateChanged(state);
    }
    else
    {
        QHeaderView::mouseReleaseEvent(event);
    }

    d->pressed = false;
}

bool CheckHeaderView::event(QEvent *event)
{

    if (event->type() == QEvent::Enter || event->type() == QEvent::Leave)
    {
        QMouseEvent *pEvent = static_cast<QMouseEvent *>(event);
        int nColumn = logicalIndexAt(pEvent->x());
        if (nColumn == 0) // checkbox column
        {
            d->moving = (event->type() == QEvent::Enter);

            update();
            return true;
        }
    }

    return QHeaderView::event(event);
}
