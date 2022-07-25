#ifndef PROPERTYCONSTANTS_H
#define PROPERTYCONSTANTS_H

#include <QString>
#include <QObject>
#include <QVariant>
#include <QHash>
#include <QColor>
#include <QIcon>
#include <QDebug>
#include "propertybrowser_global.h"
#include <QPair>


class PROPERTYBROWSERSHARED_EXPORT Range /*: public QPair<int, int>*/ {
public:
    Range(int min = 0, int max = 0);
    Range(const Range &range);
    Range(const QPair<int, int> &range);

    int from() const;
    int to() const;

    void setFrom(int v);
    void setTo(int v);

    QString toString() const;
    static Range fromString(const QString &string);

    operator QVariant() const;
    operator QString() const;
    Range &operator = (const Range &other);
    bool operator == (const Range &other) const;
    bool operator != (const Range &other) const;

private:
    static void registDataStreamOp();
    int r;
    int l;
};
Q_DECLARE_METATYPE(Range);
PROPERTYBROWSERSHARED_EXPORT QDataStream &operator << (QDataStream &out, const Range &range);
PROPERTYBROWSERSHARED_EXPORT QDataStream &operator >> (QDataStream &in, Range &range);
PROPERTYBROWSERSHARED_EXPORT bool operator>(const Range &l, const Range &r);
PROPERTYBROWSERSHARED_EXPORT bool operator<(const Range &l, const Range &r);
static QDebug operator<<(QDebug d, const Range &r) {
    return (d.nospace() << r.from() << '~' << r.to());
}


class PROPERTYBROWSERSHARED_EXPORT RangeF /*: public QPair<double, double>*/ {
public:
    RangeF(double min = 0.0, double max = 0.0);
    RangeF(const RangeF &range);
    RangeF(const QPair<double, double> &range);

    double from() const;
    double to() const;

    void setFrom(double v);
    void setTo(double v);

    QString toString() const;
    static RangeF fromString(const QString &string);

    operator QVariant() const;
    operator QString() const;

    bool operator == (const RangeF &other) const;
    bool operator != (const RangeF &other) const;

private:
    static void registDataStreamOp();
    double l;
    double r;
};
Q_DECLARE_METATYPE(RangeF);
PROPERTYBROWSERSHARED_EXPORT QDataStream &operator << (QDataStream &out, const RangeF &range);
PROPERTYBROWSERSHARED_EXPORT QDataStream &operator >> (QDataStream &in, RangeF &range);
PROPERTYBROWSERSHARED_EXPORT bool operator>(const RangeF &l, const RangeF &r);
PROPERTYBROWSERSHARED_EXPORT bool operator<(const RangeF &l, const RangeF &r);
static QDebug operator<<(QDebug d, const RangeF &r) {
    return d;/*(d.nospace() << r.first << QStringLiteral("~") << r.second)*/;
}


enum PropertyType {
    PROPERTY_TYPE_INVALID = 0, //!< invalid property type
    PROPERTY_TYPE_GROUP, //!< group property type
    PROPERTY_TYPE_CHAR, //!< char property type
    PROPERTY_TYPE_BOOL, //!< bool property type
    PROPERTY_TYPE_INT, //!< int property type
    PROPERTY_TYPE_DOUBLE, //!< double property type
    PROPERTY_TYPE_STRING, //!< string property type
    PROPERTY_TYPE_TEXTSTRING, //!< multi-line text property type
    PROPERTY_TYPE_ENUM, //!< enum property type
    PROPERTY_TYPE_CHECKABLE_ENUM, //!< checkable enum property type, multi-choice with checkbox
    PROPERTY_TYPE_ENUM_ICON, //!< enum property type
    PROPERTY_TYPE_FLAG, //!< flag property type
    PROPERTY_TYPE_COLOR, //!< color property type
    PROPERTY_TYPE_DATE, //!< date property type
    PROPERTY_TYPE_DATETIME, //!< date time property type
    PROPERTY_TYPE_TIME, //!< time property type
    PROPERTY_TYPE_POINT, //!< point property type
    PROPERTY_TYPE_SIZE, //!< size property type
    PROPERTY_TYPE_RECT, //!< rect property type
    PROPERTY_TYPE_FILESELECT, //!< select file/folder type
    PROPERTY_TYPE_FONT, //!< font property type
    PROPERTY_TYPE_KEYSEQUENCE, //!< key sequence property type
    PROPERTY_TYPE_SIZEPOLICY, //!< size policy property type
    PROPERTY_TYPE_DIALOG,
    PROPERTY_TYPE_RANGE,
    PROPERTY_TYPE_RANGEF,
};

namespace PropertyConstants {
    static const QString PROPERTY_NAME = QStringLiteral("name");
    static const QString PROPERTY_VALUE = QStringLiteral("value");
    static const QString PROPERTY_VALUE_MIN = QStringLiteral("min");
    static const QString PROPERTY_VALUE_MAX = QStringLiteral("max");
    static const QString PROPERTY_VALUE_TYPE = QStringLiteral("type");
    static const QString PROPERTY_OPTIONS = QStringLiteral("options");
    static const QString PROPERTY_EDITABLE = QStringLiteral("editable");
    static const QString PROPERTY_ASSOCIATE = QStringLiteral("associate");
    static const QString PROPERTY_SUB_ITEMS = QStringLiteral("subitems");
    static const QString PROPERTY_SHOW_TEXT = QStringLiteral("showtext");
    static const QString PROPERTY_DECIMALS = QStringLiteral("decimals");
    static const QString PROPERTY_SINGLESTEP = QStringLiteral("singleStep");
    static const QString PROPERTY_INTEGERBASE = QStringLiteral("integerBase");
    static const QString PROPERTY_SUFFIX = QStringLiteral("suffix");
    static const QString PROPERTY_VISIBLE = QStringLiteral("visible");
    static const QString PROPERTY_STRING_ECHOMODE = QStringLiteral("echoMode");
    static const QString PROPERTY_STRING_REGEXP = QStringLiteral("regexp");
    static const QString PROPERTY_STRING_REPLACE_FORMAT = QStringLiteral("format");
    static const QString PROPERTY_TOOLTIP = QStringLiteral("toolTip");
    static const QString PROPERTY_STATUSTIP = QStringLiteral("statusTip");
    static const QString PROPERTY_WHATSTHIS = QStringLiteral("whatsThis");
    static const QString PROPERTY_CONDITION = QStringLiteral("condition");
    static const QString PROPERTY_CONDITION_WHEN = QStringLiteral("when");
    static const QString PROPERTY_DIALOG_CLASSNAME = QStringLiteral("className");

    static const QMap<QString, int> PropertyNameMap = {
        { QStringLiteral("int"),        PROPERTY_TYPE_INT        },
        { QStringLiteral("size"),       PROPERTY_TYPE_SIZE       },
        { QStringLiteral("rect"),       PROPERTY_TYPE_RECT       },
        { QStringLiteral("file"),       PROPERTY_TYPE_FILESELECT },
        { QStringLiteral("font"),       PROPERTY_TYPE_FONT       },
        { QStringLiteral("bool"),       PROPERTY_TYPE_BOOL       },
        { QStringLiteral("char"),       PROPERTY_TYPE_CHAR       },
        { QStringLiteral("flag"),       PROPERTY_TYPE_FLAG       },
        { QStringLiteral("enum"),       PROPERTY_TYPE_ENUM       },
        { QStringLiteral("multiChoice"),PROPERTY_TYPE_CHECKABLE_ENUM       },
        { QStringLiteral("date"),       PROPERTY_TYPE_DATE       },
        { QStringLiteral("time"),       PROPERTY_TYPE_TIME       },
        { QStringLiteral("color"),      PROPERTY_TYPE_COLOR      },
        { QStringLiteral("point"),      PROPERTY_TYPE_POINT      },
        { QStringLiteral("range"),      PROPERTY_TYPE_RANGE      },
        { QStringLiteral("rangef"),     PROPERTY_TYPE_RANGEF     },
        { QStringLiteral("float"),      PROPERTY_TYPE_DOUBLE     },
        { QStringLiteral("string"),     PROPERTY_TYPE_STRING     },
        { QStringLiteral("datetime"),   PROPERTY_TYPE_DATETIME   },
        { QStringLiteral("stringlist"), PROPERTY_TYPE_ENUM       },
        { QStringLiteral("shortcut"),   PROPERTY_TYPE_KEYSEQUENCE},
        { QStringLiteral("sizepolicy"), PROPERTY_TYPE_SIZEPOLICY },
        { QStringLiteral("dialog"),     PROPERTY_TYPE_DIALOG     },
        { QStringLiteral("group"),      PROPERTY_TYPE_GROUP      },
        };

   // property type enum map to name
    static const QMap<int, QString> PropertyTypeNameMap = {
        { PROPERTY_TYPE_INVALID,    QStringLiteral("null")      },
        { PROPERTY_TYPE_INT,        QStringLiteral("int")       },
        { PROPERTY_TYPE_SIZE,       QStringLiteral("size")      },
        { PROPERTY_TYPE_RECT,       QStringLiteral("rect")      },
        { PROPERTY_TYPE_FONT,       QStringLiteral("font")      },
        { PROPERTY_TYPE_BOOL,       QStringLiteral("bool")      },
        { PROPERTY_TYPE_CHAR,       QStringLiteral("char")      },
        { PROPERTY_TYPE_FLAG,       QStringLiteral("flag")      },
        { PROPERTY_TYPE_ENUM,       QStringLiteral("enum")      },
        { PROPERTY_TYPE_CHECKABLE_ENUM, QStringLiteral("multiChoice")      },
        { PROPERTY_TYPE_DATE,       QStringLiteral("date")      },
        { PROPERTY_TYPE_TIME,       QStringLiteral("time")      },
        { PROPERTY_TYPE_COLOR,      QStringLiteral("color")     },
        { PROPERTY_TYPE_POINT,      QStringLiteral("point")     },
        { PROPERTY_TYPE_DOUBLE,     QStringLiteral("float")     },
        { PROPERTY_TYPE_STRING,     QStringLiteral("string")    },
        { PROPERTY_TYPE_DATETIME,   QStringLiteral("datetime")  },
        { PROPERTY_TYPE_FILESELECT, QStringLiteral("file")      },
        { PROPERTY_TYPE_SIZEPOLICY, QStringLiteral("sizepolicy")},
        { PROPERTY_TYPE_KEYSEQUENCE,QStringLiteral("shortcut")  },
        { PROPERTY_TYPE_DIALOG,     QStringLiteral("dialog")    },
        { PROPERTY_TYPE_RANGE,      QStringLiteral("range")     },
        { PROPERTY_TYPE_RANGEF,     QStringLiteral("rangef")    },
        };

}


/*!
 * \brief Composites the input data to a completely property item.
 * \param name Property Name
 * \param showText Property display name
 * \param type \sa PropertyType enum
 * \param property value
 * \param max the property maxiumn value
 * \param min the property miniumn value
 * \param editable the proerty item could be edit or not.
 * \param options the enum's options list.
 */
static QVariantHash getPropertyItem(const QString &name,
    const QString &showText,
    int type,
    const QVariant &value = QVariant(),
    const QVariant &max = QVariant(),
    const QVariant &min = QVariant(),
    bool editable = true,
    const QList<QVariant> &options = QList<QVariant>(),
    int decimals = 2)
{
    QVariantHash property;
    property.insert(PropertyConstants::PROPERTY_NAME, name);
    property.insert(PropertyConstants::PROPERTY_SHOW_TEXT, showText);
    property.insert(PropertyConstants::PROPERTY_VALUE_TYPE, type);
    property.insert(PropertyConstants::PROPERTY_VALUE, value);
    property.insert(PropertyConstants::PROPERTY_EDITABLE, editable);

    if(!max.isNull()) {
        property.insert(PropertyConstants::PROPERTY_VALUE_MAX, max);
    }

    if(!min.isNull()) {
        property.insert(PropertyConstants::PROPERTY_VALUE_MIN, min);
    }

    if(!options.isEmpty()) {
        property.insert(PropertyConstants::PROPERTY_OPTIONS, options);
    }

    property.insert(PropertyConstants::PROPERTY_DECIMALS, decimals);

    return property;
}

class PROPERTYBROWSERSHARED_EXPORT PropertyCommon: public QObject
{
    Q_OBJECT
public:
    static PropertyCommon *getInstance();
    QVariantHash getPropertyItem(const QString &name, const QString &showText, int type,
                                 const QVariant &value = QVariant(), const QVariant &max = QVariant(),
                                 const QVariant &min = QVariant(), bool editable = true,
                                 const QList<QVariant> &options = QList<QVariant>());
private:
    explicit PropertyCommon(QObject *parent = 0);
    static PropertyCommon *m_instance;
};

#endif // PROPERTYCONSTANTS_H
