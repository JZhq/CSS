#ifndef PROPERTYBROWSER_GLOBAL_H
#define PROPERTYBROWSER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PROPERTYBROWSER_LIBRARY)
#  define PROPERTYBROWSERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PROPERTYBROWSERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PROPERTYBROWSER_GLOBAL_H
