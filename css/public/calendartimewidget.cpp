#include "calendartimewidget_P.h"
#include "ui_calendartimewidget.h"

CalendarTimeWidget::CalendarTimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalendarTimeWidget)
{
    ui->setupUi(this);
    ui->calendarWidget->setDateEditEnabled(true);
}

CalendarTimeWidget::~CalendarTimeWidget()
{
    delete ui;
}

void CalendarTimeWidget::setDateTime(const QDateTime &dateTime)
{
    ui->calendarWidget->setSelectedDate(dateTime.date());
    ui->timeEdit->setTime(dateTime.time());
}

QDateTime CalendarTimeWidget::getDateTime() const
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time = ui->timeEdit->time();
    QDateTime dateTime(date, time);
    return dateTime;
}

void CalendarTimeWidget::setDateTimeRange(const QDateTime &min, const QDateTime &max)
{
    m_min = min;
    m_max = max;
    ui->calendarWidget->setDateRange(m_min.date(), m_max.date());
}

void CalendarTimeWidget::on_calendarWidget_clicked(const QDate &date)
{
    if(date == m_min.date())
    {
        ui->timeEdit->clearMaximumTime();
        ui->timeEdit->setMinimumTime(m_min.time());
    }
    else if(date == m_max.date())
    {
        ui->timeEdit->clearMinimumTime();
        ui->timeEdit->setMaximumTime(m_max.time());
    }
    else
    {
        ui->timeEdit->clearMaximumTime();
        ui->timeEdit->clearMinimumTime();
    }
}
