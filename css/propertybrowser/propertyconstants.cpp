#include "propertyconstants.h"


Range::Range(int min, int max)
    : l(min)
    , r(max)
{
    registDataStreamOp();
}

Range::Range(const Range &range)
    : l(range.l)
    , r(range.r)
{

}

Range::Range(const QPair<int, int> &range)
    : l(range.first)
    , r(range.second)
{

}

int Range::from() const
{
    return l;
}

int Range::to() const
{
    return r;
}

void Range::setFrom(int v)
{
    l = v;
    if (r < v) {
        r = v;
    }
}

void Range::setTo(int v)
{
    r = v;
    if (l > v) {
        l = v;
    }
}

QString Range::toString() const
{
    return QStringLiteral("%1~%2").arg(l).arg(r);
}

Range Range::fromString(const QString &string)
{
    return Range(string.section('~', 0, 0).toInt(), string.section('~', 1, 1).toInt());
}

Range::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

Range::operator QString() const
{
    return toString();
}

Range &Range::operator =(const Range &other)
{
    if (this != &other) {
        setFrom(other.from());
        setTo(other.to());
    }
    return *this;
}

bool Range::operator ==(const Range &other) const
{
    if (this == &other) {
        return true;
    }

    return (from() == other.from() && to() == other.to());
}

bool Range::operator !=(const Range &other) const
{
    return ! (*this == other);
}

void Range::registDataStreamOp() {
    static bool inited = false;
    if(!inited) {
        qRegisterMetaType<Range>("Range");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        qRegisterMetaTypeStreamOperators<Range>("Range");
        QMetaType::registerDebugStreamOperator<Range>();
#endif
        inited = true;
        auto f = new QtPrivate::ConverterMemberFunction<Range, QString>(&Range::toString);
        QMetaType::registerConverterFunction(f, qMetaTypeId<Range>(), QMetaType::QString);
    }
}

QDataStream &operator <<(QDataStream &out, const Range &range)
{
    out << range.from() << range.to();
    return out;
}

QDataStream &operator >>(QDataStream &in, Range &range)
{
    int from = 0;
    int to = 0;
    in >> from >> to;

    range.setFrom(from);
    range.setTo(to);

    return in;
}

bool operator>(const Range &l, const Range &r) {
    return qAbs(l.to() - l.from()) > qAbs(r.to() - r.from());
}

bool operator<(const Range &l, const Range &r) {
    return !(l>r);
}

RangeF::RangeF(double min, double max)
    : l(min)
    , r(max)
{
    registDataStreamOp();
}

RangeF::RangeF(const RangeF &range)
    : l(range.l)
    , r(range.r)
{

}

RangeF::RangeF(const QPair<double, double> &range)
    : l(range.first)
    , r(range.second)
{

}

double RangeF::from() const
{
    return l;
}

double RangeF::to() const
{
    return r;
}

void RangeF::setFrom(double v)
{
    l = v;
    if (r < v) {
        r = v;
    }
}

void RangeF::setTo(double v)
{
    r = v;
    if (l > v) {
        l = v;
    }
}

QString RangeF::toString() const
{
    return QStringLiteral("%1~%2").arg(l).arg(r);
}

RangeF RangeF::fromString(const QString &string)
{
    return RangeF(string.section('~', 0, 0).toDouble(), string.section('~', 1, 1).toDouble());
}

RangeF::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

RangeF::operator QString() const
{
    return toString();
}

bool RangeF::operator ==(const RangeF &other) const
{
    if (this == &other) {
        return true;
    }

    return (from() == other.from() && to() == other.to());
}

bool RangeF::operator !=(const RangeF &other) const
{
    return !(*this == other);
}

void RangeF::registDataStreamOp() {
    static bool inited = false;
    if(!inited) {
        qRegisterMetaType<RangeF>("RangeF");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        qRegisterMetaTypeStreamOperators<RangeF>("RangeF");
        QMetaType::registerDebugStreamOperator<RangeF>();
#endif
        inited = true;
        auto f = new QtPrivate::ConverterMemberFunction<RangeF, QString>(&RangeF::toString);
        QMetaType::registerConverterFunction(f, qMetaTypeId<RangeF>(), QMetaType::QString);
    }
}

QDataStream &operator <<(QDataStream &out, const RangeF &range)
{
    out << range.from() << range.to();
    return out;
}

QDataStream &operator >>(QDataStream &in, RangeF &range)
{
    double from = 0;
    double to = 0;
    in >> from >> to;

    range.setFrom(from);
    range.setTo(to);

    return in;
}

bool operator>(const RangeF &l, const RangeF &r) {
    return qAbs(l.to() - l.from()) > qAbs(r.to() - r.from());
}

bool operator<(const RangeF &l, const RangeF &r) {
    return !(l>r);
}


PropertyCommon * PropertyCommon::m_instance = NULL;
PropertyCommon *PropertyCommon::getInstance()
{
    if(m_instance == NULL)
        m_instance = new PropertyCommon();

    return m_instance;
}

QVariantHash PropertyCommon::getPropertyItem(const QString &name, const QString &showText,
                                             int type, const QVariant &value,
                                             const QVariant &max, const QVariant &min,
                                             bool editable, const QList<QVariant> &options)
{
    QVariantHash property;
    property.insert(PropertyConstants::PROPERTY_NAME, name);
    property.insert(PropertyConstants::PROPERTY_SHOW_TEXT, showText);
    property.insert(PropertyConstants::PROPERTY_VALUE_TYPE, type);
    property.insert(PropertyConstants::PROPERTY_VALUE, value);
    property.insert(PropertyConstants::PROPERTY_EDITABLE, editable);

    if(!max.isNull())
        property.insert(PropertyConstants::PROPERTY_VALUE_MAX, max);

    if(!min.isNull())
        property.insert(PropertyConstants::PROPERTY_VALUE_MIN, min);

    if(!options.isEmpty())
    {
        if(type==PROPERTY_TYPE_GROUP) {
            property.insert(PropertyConstants::PROPERTY_SUB_ITEMS, options);
        } else {
            property.insert(PropertyConstants::PROPERTY_OPTIONS, options);
        }
    }

    return property;
}

PropertyCommon::PropertyCommon(QObject *parent)
    :QObject(parent)
{
}
