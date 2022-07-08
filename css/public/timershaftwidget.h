#ifndef TIMERSHAFT_H
#define TIMERSHAFT_H

#include <QWidget>
#include <QTime>
#include <QColor>
#include <QList>
#include <QPair>

#include "public_global.h"

class QTimer;
class GLOBAL_EXPORT TimerShaftWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QTime m_minTime READ getMinTime WRITE setMinTime)
    Q_PROPERTY(QTime m_maxTime READ getMaxTime WRITE setMaxTime)

public:
    explicit TimerShaftWidget(QWidget *parent = nullptr);
    void setShowDate(const QDate& date);
    void setOccupiedTimePairs(const QList<QPair<QDateTime, QDateTime>>& pairs);
    void setIntervalTime(int begin, int end);
    void setRecommandTimePair(const QPair<QDateTime, QDateTime>& pair);

    QTime getMinTime() const;
    QTime getMaxTime() const;

public slots:
    void setRange(const QTime& min, const QTime& max);
    void setMinTime(const QTime& time);
    void setMaxTime(const QTime& time);

protected:
    void paintEvent(QPaintEvent *);
    void drawAnnotation(QPainter *);
    void drawRuler(QPainter *painter);
    void drawBarBg(QPainter *painter);
    void drawOccupiedBarBg(QPainter *painter);
    void drawIntervalBarBg(QPainter *painter, int startVal, int endVal);
    void drawRecommandBarBg(QPainter *painter);
    void drawPassBar(QPainter *painter);

private:
    void initializeUi();
    void handleEvents();
    int getValueByTime(const QTime& time);

signals:
    void signalTimeOut();

public slots:

private:
    QDate m_showDade;           // 时间轴工作的日期

    QTime m_minTime;            // 最小时间
    QTime m_maxTime;            // 最大时间
    QTime m_minPlanTime;        // 最小规划时间
    QTime m_maxPlanTime;        // 最大规划时间
    QTime m_curTime;            // 当前时间
    QList<QPair<QDateTime, QDateTime>> m_allOccupiedTime; // 被占用时间段
    QPair<QDateTime, QDateTime> m_recommandPair;

    QColor m_barBgColor;        // 柱状默认背景色
    QColor m_passBarColor;      // 过去时间背景色
    QColor m_occupiedBarColor;  // 任务占用时间背景色
    QColor m_interValBarColor;  // 前后间隔时间背景色
    QColor m_planBarColor;      // 规划时间背景色
    QColor m_lineColor;         // 线条颜色
    QColor m_recommandColor;    // 推荐时间的颜色

    int m_space;                // 间距
    int m_shortStep;            // 短线条等分步长
    int m_longStep;             // 长线条等分步长
    QRectF m_barRect;           // 柱状图区域
    int m_beginInterval;
    int m_endInterval;

    QTimer *m_flushTimer;       // 定时器更新时间流逝区域
};

#endif // TIMERSHAFT_H
