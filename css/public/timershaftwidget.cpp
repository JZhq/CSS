#include "timershaftwidget.h"

#include <QDebug>
#include <QTimer>
#include <QPainter>

TimerShaftWidget::TimerShaftWidget(QWidget *parent) : QWidget(parent)
{
    initializeUi();
    handleEvents();
}

void TimerShaftWidget::initializeUi()
{
    m_beginInterval = 0;
    m_endInterval = 0;
    m_showDade = QDate::currentDate();
    m_minTime = QTime(0, 0, 0);
    m_maxTime = QTime(23, 59, 59);
    m_curTime = QDateTime::currentDateTime().time();

    m_barBgColor = QColor(156, 252, 120);
    m_passBarColor = QColor(220, 220, 220);
    m_occupiedBarColor = QColor(249, 0, 68);
    m_interValBarColor = QColor(255, 255, 0);
    m_recommandColor = QColor(86, 177, 252);
    m_planBarColor = QColor(100, 184, 255);
    m_lineColor = QColor(0, 0, 0);

    m_space = 20;
    m_shortStep = getValueByTime(QTime(0, 10, 0));
    m_longStep = getValueByTime(QTime(1, 0, 0));

    m_flushTimer = new QTimer(this);
    m_flushTimer->setInterval(1000);

    setFont(QFont("Arial", 8));
}

void TimerShaftWidget::handleEvents()
{
    connect(m_flushTimer, &QTimer::timeout, this, [this](){
        m_curTime = QDateTime::currentDateTime().time();
        // 超过编排的时间段，则更新新的编排时间段
        if (QDateTime::currentDateTime() >= m_recommandPair.first)
            emit signalTimeOut();
        update();
    });
    m_flushTimer->start();
}

void TimerShaftWidget::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 绘制注释
    drawAnnotation(&painter);
    // 绘制标尺
    drawRuler(&painter);
    // 绘制柱状背景
    drawBarBg(&painter);
    // 绘制被占用的时间段
    drawOccupiedBarBg(&painter);
    // 绘制推荐编排时间
    drawRecommandBarBg(&painter);
    // 绘制已经过去的时间背景
    drawPassBar(&painter);
}

void TimerShaftWidget::drawAnnotation(QPainter *painter)
{
    setFont(QFont("Arial", 15));
    QString passText = QStringLiteral("过去时间段");
    QString occupiedText = QStringLiteral("被占用时间段");
    QString usableText = QStringLiteral("可用时间段");
    QString intervalText = QStringLiteral("任务间隔时间段");
    QString recommandText = QStringLiteral("推荐时间段");

    QStringList strList;
    strList << passText << occupiedText << usableText << intervalText << recommandText;
    QList<QColor> colorList;
    colorList << m_passBarColor << m_occupiedBarColor << m_barBgColor << m_interValBarColor << m_recommandColor;

    double lastWidth = 0;
    for (int i = 0; i < strList.size(); ++i) {
        painter->save();
        painter->setPen(m_lineColor);

        double initX = m_space + lastWidth + 5;
        double initY = 10;
        QPointF rectTopLeftPot(initX, initY);
        QPointF rectBottomRightPot(initX + 12, initY + 12);
        QRectF curRect(rectTopLeftPot, rectBottomRightPot);
        painter->setBrush(colorList[i]);
        painter->drawRect(curRect);

        double fontWidth = painter->fontMetrics().width(intervalText);
        QPointF textPot(initX + 15, initY + 9);
        painter->drawText(textPot, strList[i]);
        lastWidth += curRect.width() + fontWidth + 20;

        painter->restore();
    }
    setFont(QFont("Arial", 8));
}

void TimerShaftWidget::drawRuler(QPainter *painter)
{
    painter->save();
    painter->setPen(m_lineColor);

     //绘制纵向标尺线 20的长度为刻度尺文字的宽度
    double initX = m_space;
    double initY = height() - 20;
    QPointF leftPot(initX, initY);
    QPointF rightBot(width() - m_space, initY);
    painter->drawLine(leftPot, rightBot);

    // 绘制横向标尺刻度
    double length = width() - 2 * m_space;

    // 计算每一格移动多少
    int max = getValueByTime(m_maxTime) + getValueByTime(QTime(0, 0, 1));
    int min = getValueByTime(m_minTime);
    double increment = length / (max - min);

    //长线条短线条长度
    int longLineLen = 10;
    int shortLineLen = 7;

    //根据范围值绘制刻度值及刻度值
    for (int i = min; i <= max; i += m_shortStep) {
        if (i % m_longStep == 0) {
            QString strValue = QString("%1").arg(i / m_longStep);
            QPointF bottomPot(initX, initY);
            QPointF topPot(initX, initY - longLineLen);
            painter->drawLine(bottomPot, topPot);

            double fontWidth = painter->fontMetrics().width(strValue);
            double fontHeight = painter->fontMetrics().height();
            QPointF textPot(initX - fontWidth / 3, initY + fontHeight + 3);
            painter->drawText(textPot, strValue);
        } else {
            QPointF bottomPot(initX, initY);
            QPointF topPot(initX, initY - shortLineLen);
            painter->drawLine(bottomPot, topPot);
        }

        initX += increment * m_shortStep;
    }

    painter->restore();
}

void TimerShaftWidget::drawBarBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    //35 的长度为刻度尺文字的宽度 36为刻度尺到柱状图的宽度
    double initY = height() - 35 - 36;
    QPointF topLeftPot(m_space, initY);
    QPointF bottomRightPot(width() - m_space, height() - 35);
    m_barRect = QRectF(topLeftPot, bottomRightPot);

    painter->setBrush(m_barBgColor);
    painter->drawRect(m_barRect);
    painter->restore();
}

void TimerShaftWidget::drawOccupiedBarBg(QPainter *painter)
{   
    double barWidth = m_barRect.width();
    int max = getValueByTime(m_maxTime) + getValueByTime(QTime(0, 0, 1));
    int min = getValueByTime(m_minTime);
    double increament = (double)barWidth / (max - min);

    foreach (auto pair, m_allOccupiedTime) {
        QPair<QTime, QTime> curPairTime;
        if ((pair.first.date() == m_showDade) &&
                (pair.second.date() == m_showDade)) {
            // 开始和结束日期都在当天
            curPairTime.first = pair.first.time();
            curPairTime.second = pair.second.time();
        } else if (pair.first.date() == m_showDade) {
            // 开始日期在当天，结束日期在次日
            curPairTime.first = pair.first.time();
            curPairTime.second = QTime(23, 59, 59);
        } else if (pair.second.date() == m_showDade) {
            // 开始日期在前日，结束日期在当日
            curPairTime.first = QTime(0, 0, 0);
            curPairTime.second = pair.second.time();
        }

        // 画前后间隔时间
        if (getValueByTime(curPairTime.first.addSecs(-m_beginInterval)) < getValueByTime(curPairTime.first)) {
            drawIntervalBarBg(painter, getValueByTime(curPairTime.first.addSecs(-m_beginInterval)),
                                                      getValueByTime(curPairTime.first));
        }
        if (getValueByTime(curPairTime.second) < getValueByTime(curPairTime.second.addSecs(m_endInterval))) {
            drawIntervalBarBg(painter, getValueByTime(curPairTime.second),
                                                      getValueByTime(curPairTime.second.addSecs(m_endInterval)));
        }

        painter->save();
        painter->setPen(Qt::NoPen);

        int startVal = getValueByTime(curPairTime.first);
        int endVal = getValueByTime(curPairTime.second);
        double initX = (startVal - min) * increament;
        double finitX = (endVal - min) * increament;

        QPointF topLeftPot(initX + m_space, m_barRect.topLeft().y());
        QPointF bottomRightPot(finitX + m_space, m_barRect.bottomRight().y());
        QRectF currentRect(topLeftPot, bottomRightPot);

        painter->setBrush(m_occupiedBarColor);
        painter->drawRect(currentRect);
        painter->restore();
    }
}

void TimerShaftWidget::drawIntervalBarBg(QPainter *painter, int startVal, int endVal)
{
    double barWidth = m_barRect.width();
    int max = getValueByTime(m_maxTime) + getValueByTime(QTime(0, 0, 1));
    int min = getValueByTime(m_minTime);
    double increament = (double)barWidth / (max - min);

    painter->save();
    painter->setPen(Qt::NoPen);

    double initX = (startVal - min) * increament;
    double finitX = (endVal - min) * increament;

    QPointF topLeftPot(initX + m_space, m_barRect.topLeft().y());
    QPointF bottomRightPot(finitX + m_space, m_barRect.bottomRight().y());
    QRectF currentRect(topLeftPot, bottomRightPot);

    painter->setBrush(m_interValBarColor);
    painter->drawRect(currentRect);
    painter->restore();
}

void TimerShaftWidget::drawRecommandBarBg(QPainter *painter)
{
    QTime recommandTimeFirst;
    QTime recommandTimeSecond;
    QPair<QDateTime, QDateTime> pair = m_recommandPair;

    if ((pair.first.date() == m_showDade) &&
            (pair.second.date() == m_showDade)) {
        // 开始和结束日期都在当天
        recommandTimeFirst = pair.first.time();
        recommandTimeSecond = pair.second.time();
    } else if (pair.first.date() == m_showDade) {
        // 开始日期在当天，结束日期在次日
        recommandTimeFirst = pair.first.time();
        recommandTimeSecond = QTime(23, 59, 59);
    } else if (pair.second.date() == m_showDade) {
        // 开始日期在前日，结束日期在当日
        recommandTimeFirst = QTime(0, 0, 0);
        recommandTimeSecond = pair.second.time();
    } else {
        return;
    }
    double barWidth = m_barRect.width();
    int max = getValueByTime(m_maxTime) + getValueByTime(QTime(0, 0, 1));
    int min = getValueByTime(m_minTime);
    double increament = (double)barWidth / (max - min);

    painter->save();
    painter->setPen(Qt::NoPen);

    int startVal = getValueByTime(recommandTimeFirst);
    int endVal = getValueByTime(recommandTimeSecond);
    double initX = (startVal - min) * increament;
    double finitX = (endVal - min) * increament;

    QPointF topLeftPot(initX + m_space, m_barRect.topLeft().y());
    QPointF bottomRightPot(finitX + m_space, m_barRect.bottomRight().y());
    QRectF currentRect(topLeftPot, bottomRightPot);

    painter->setBrush(m_recommandColor);
    painter->drawRect(currentRect);
    painter->restore();
}

void TimerShaftWidget::drawPassBar(QPainter *painter)
{
    if (m_showDade != QDate::currentDate())
        return;

    painter->save();
    painter->setPen(Qt::NoPen);

    double barWidth = m_barRect.width();
    int max = getValueByTime(m_maxTime) + getValueByTime(QTime(0, 0, 1));
    int min = getValueByTime(m_minTime);
    int curVal = getValueByTime(m_curTime);

    double increament = (double)barWidth / (max - min);
    double initX = (curVal - min) * increament;

    QPointF topLeftPot(m_barRect.topLeft());
    QPointF bottomRightPot(initX + m_space, m_barRect.bottomRight().y());
    QRectF currentRect(topLeftPot, bottomRightPot);

    painter->setBrush(m_passBarColor);
    painter->drawRect(currentRect);
    painter->restore();
}

QTime TimerShaftWidget::getMinTime() const
{
    return m_minTime;
}

QTime TimerShaftWidget::getMaxTime() const
{
    return m_maxTime;
}

void TimerShaftWidget::setShowDate(const QDate &date)
{
    m_showDade = date;
    update();
}

void TimerShaftWidget::setOccupiedTimePairs(const QList<QPair<QDateTime, QDateTime> > &pairs)
{
    m_allOccupiedTime = pairs;
    update();
}

void TimerShaftWidget::setIntervalTime(int begin, int end)
{
    m_beginInterval = begin;
    m_endInterval = end;
    update();
}

void TimerShaftWidget::setRecommandTimePair(const QPair<QDateTime, QDateTime> &pair)
{
    m_recommandPair = pair;
    update();
}

void TimerShaftWidget::setRange(const QTime& min, const QTime& max)
{
    if (min > max)
        return;

    m_minTime = min;
    m_maxTime = max;

    update();
}

void TimerShaftWidget::setMinTime(const QTime& time)
{
    setRange(time, m_maxTime);
}

void TimerShaftWidget::setMaxTime(const QTime& time)
{
    setRange(m_minTime, time);
}

int TimerShaftWidget::getValueByTime(const QTime& time)
{
    int val = time.hour() * 60 * 60 + time.minute() * 60 + time.second();
    return val;
}



