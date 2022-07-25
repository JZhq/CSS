/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qteditorfactory.h"
#include "qtpropertybrowserutils_p.h"
#include <QSpinBox>
#include <QScrollBar>
#include <QComboBox>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QColorDialog>
#include <QFontDialog>
#include <QSpacerItem>
#include <QStyleOption>
#include <QPainter>
#include <QMap>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QMetaMethod>

#if defined(Q_CC_MSVC)
#    pragma warning(disable: 4786) /* MS VS 6: truncating debug info after 255 characters */
#endif

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

// Set a hard coded left margin to account for the indentation
// of the tree view icon when switching to an editor

static inline void setupTreeViewEditorMargin(QLayout *lt)
{
    enum { DecorationMargin = 4 };
    if (QApplication::layoutDirection() == Qt::LeftToRight)
        lt->setContentsMargins(DecorationMargin, 0, 0, 0);
    else
        lt->setContentsMargins(0, 0, DecorationMargin, 0);
}

// ---------- EditorFactoryPrivate :
// Base class for editor factory private classes. Manages mapping of properties to editors and vice versa.

template <class Editor>
class EditorFactoryPrivate
{
public:

    typedef QList<Editor *> EditorList;
    typedef QMap<QtProperty *, EditorList> PropertyToEditorListMap;
    typedef QMap<Editor *, QtProperty *> EditorToPropertyMap;

    Editor *createEditor(QtProperty *property, QWidget *parent);
    void initializeEditor(QtProperty *property, Editor *e);
    void slotEditorDestroyed(QObject *object);

    PropertyToEditorListMap  m_createdEditors;
    EditorToPropertyMap m_editorToProperty;
};

template <class Editor>
Editor *EditorFactoryPrivate<Editor>::createEditor(QtProperty *property, QWidget *parent)
{
    Editor *editor = new Editor(parent);
    initializeEditor(property, editor);
    return editor;
}

template <class Editor>
void EditorFactoryPrivate<Editor>::initializeEditor(QtProperty *property, Editor *editor)
{
    typename PropertyToEditorListMap::iterator it = m_createdEditors.find(property);
    if (it == m_createdEditors.end())
        it = m_createdEditors.insert(property, EditorList());
    it.value().append(editor);
    m_editorToProperty.insert(editor, property);
}

template <class Editor>
void EditorFactoryPrivate<Editor>::slotEditorDestroyed(QObject *object)
{
    const typename EditorToPropertyMap::iterator ecend = m_editorToProperty.end();
    for (typename EditorToPropertyMap::iterator itEditor = m_editorToProperty.begin(); itEditor !=  ecend; ++itEditor) {
        if (itEditor.key() == object) {
            Editor *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            const typename PropertyToEditorListMap::iterator pit = m_createdEditors.find(property);
            if (pit != m_createdEditors.end()) {
                pit.value().removeAll(editor);
                if (pit.value().empty())
                    m_createdEditors.erase(pit);
            }
            m_editorToProperty.erase(itEditor);
            return;
        }
    }
}

// ------------ QtSpinBoxFactory
class IntEditor : public QWidget
{
    Q_OBJECT
public:
    IntEditor(QWidget *parent = 0)
        : QWidget(parent)
    {
        _editor = new QSpinBox(this);
        connect(_editor, QOverload<int>::of(&QSpinBox::valueChanged), this, &IntEditor::valueChanged);
        _suffix = new QLabel(this);
        _suffix->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(_editor);
        layout->addWidget(_suffix);

        setLayout(layout);
    }

    QSpinBox *editor() const { return _editor; }
    QString suffix() const { return _suffix->text(); }

signals:
    void valueChanged(int value);
    void suffixChanged(const QString &suffix);

public slots:
    void setSuffix(const QString &suffix) {
        if (_suffix->text().compare(suffix) == 0) {
            return ;
        }
        _suffix->setText(suffix);
        emit suffixChanged(_suffix->text());
    }

private:
    QSpinBox *_editor = nullptr;
    QLabel *_suffix = nullptr;
};

class QtSpinBoxFactoryPrivate : public EditorFactoryPrivate<IntEditor>
{
    QtSpinBoxFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtSpinBoxFactory)
public:
    QtSpinBoxFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, int value);
    void slotRangeChanged(QtProperty *property, int min, int max);
    void slotSingleStepChanged(QtProperty *property, int step);
    void slotIntegerBaseChanged(QtProperty *property, int base);
    void slotReadOnlyChanged(QtProperty *property, bool readOnly);
    void slotSuffixChanged(QtProperty *property, const QString &suffix);
    void slotSetValue(int value);
};

void QtSpinBoxFactoryPrivate::slotPropertyChanged(QtProperty *property, int value)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<IntEditor *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        IntEditor *editor = itEditor.next();
        if (editor->editor()->value() != value) {
            editor->editor()->blockSignals(true);
            editor->editor()->setValue(value);
            editor->editor()->blockSignals(false);
        }
    }
}

void QtSpinBoxFactoryPrivate::slotRangeChanged(QtProperty *property, int min, int max)
{
    if (!m_createdEditors.contains(property))
        return;

    QtIntPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<IntEditor *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        IntEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setRange(min, max);
        editor->editor()->setValue(manager->value(property));
        editor->editor()->blockSignals(false);
    }
}

void QtSpinBoxFactoryPrivate::slotSingleStepChanged(QtProperty *property, int step)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<IntEditor *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        IntEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setSingleStep(step);
        editor->editor()->blockSignals(false);
    }
}

void QtSpinBoxFactoryPrivate::slotIntegerBaseChanged(QtProperty *property, int base)
{
    const auto it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.cend())
        return;
    for (IntEditor *editor : it.value()) {
        editor->editor()->blockSignals(true);
        editor->editor()->setDisplayIntegerBase(base);
        static QMap<int, QString> prefix = { {2, "0B"}, {8, "0O"}, {10, ""}, {16, "0x"}};
        editor->editor()->setPrefix(prefix.value(base));
        editor->editor()->blockSignals(false);
    }
}
void QtSpinBoxFactoryPrivate::slotReadOnlyChanged( QtProperty *property, bool readOnly)
{
    if (!m_createdEditors.contains(property))
        return;

    QtIntPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<IntEditor *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        IntEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setReadOnly(readOnly);
        editor->editor()->blockSignals(false);
    }
}

void QtSpinBoxFactoryPrivate::slotSuffixChanged(QtProperty *property, const QString &suffix)
{
    const auto it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.cend())
        return;
		
    QtIntPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    for (IntEditor *editor : it.value()) {
        editor->blockSignals(true);
        editor->setSuffix(suffix);
        editor->blockSignals(false);
    }
}
void QtSpinBoxFactoryPrivate::slotSetValue(int value)
{
    QObject *object = q_ptr->sender();
    const QMap<IntEditor *, QtProperty *>::ConstIterator  ecend = m_editorToProperty.constEnd();
    for (QMap<IntEditor *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor !=  ecend; ++itEditor) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtIntPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
    }
}

/*!
    \class QtSpinBoxFactory

    \brief The QtSpinBoxFactory class provides QSpinBox widgets for
    properties created by QtIntPropertyManager objects.

    \sa QtAbstractEditorFactory, QtIntPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtSpinBoxFactory::QtSpinBoxFactory(QObject *parent)
    : QtAbstractEditorFactory<QtIntPropertyManager>(parent)
{
    d_ptr = new QtSpinBoxFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtSpinBoxFactory::~QtSpinBoxFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtSpinBoxFactory::connectPropertyManager(QtIntPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    connect(manager, SIGNAL(rangeChanged(QtProperty *, int, int)),
                this, SLOT(slotRangeChanged(QtProperty *, int, int)));
    connect(manager, SIGNAL(singleStepChanged(QtProperty *, int)),
                this, SLOT(slotSingleStepChanged(QtProperty *, int)));
    connect(manager, SIGNAL(integerBaseChanged(QtProperty *, int)),
        this, SLOT(slotIntegerBaseChanged(QtProperty *, int)));
    connect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
                this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtSpinBoxFactory::createEditor(QtIntPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    IntEditor *editor = d_ptr->createEditor(property, parent);
    editor->editor()->setSingleStep(manager->singleStep(property));
    editor->setSuffix(manager->suffix(property));
    editor->editor()->setRange(manager->minimum(property), manager->maximum(property));
    editor->editor()->setValue(manager->value(property));
    editor->editor()->setDisplayIntegerBase(manager->integerBase(property));
    if (manager->integerBase(property) != 10) {
        static QMap<int, QString> prefix = { {2, "0B"}, {8, "0O"}, {16, "0x"}};
        editor->editor()->setPrefix(prefix.value(manager->integerBase(property)));
    }
    editor->editor()->setKeyboardTracking(false);
    editor->editor()->setReadOnly(manager->isReadOnly(property));

    connect(editor, SIGNAL(valueChanged(int)), this, SLOT(slotSetValue(int)));
    connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtSpinBoxFactory::disconnectPropertyManager(QtIntPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(rangeChanged(QtProperty *, int, int)),
                this, SLOT(slotRangeChanged(QtProperty *, int, int)));
    disconnect(manager, SIGNAL(singleStepChanged(QtProperty *, int)),
                this, SLOT(slotSingleStepChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
                this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

// QtSliderFactory

class QtSliderFactoryPrivate : public EditorFactoryPrivate<QSlider>
{
    QtSliderFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtSliderFactory)
public:
    QtSliderFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, int value);
    void slotRangeChanged(QtProperty *property, int min, int max);
    void slotSingleStepChanged(QtProperty *property, int step);
    void slotSetValue(int value);
};

void QtSliderFactoryPrivate::slotPropertyChanged(QtProperty *property, int value)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<QSlider *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QSlider *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setValue(value);
        editor->blockSignals(false);
    }
}

void QtSliderFactoryPrivate::slotRangeChanged(QtProperty *property, int min, int max)
{
    if (!m_createdEditors.contains(property))
        return;

    QtIntPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QSlider *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QSlider *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setRange(min, max);
        editor->setValue(manager->value(property));
        editor->blockSignals(false);
    }
}

void QtSliderFactoryPrivate::slotSingleStepChanged(QtProperty *property, int step)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<QSlider *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QSlider *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setSingleStep(step);
        editor->blockSignals(false);
    }
}

void QtSliderFactoryPrivate::slotSetValue(int value)
{
    QObject *object = q_ptr->sender();
    const QMap<QSlider *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QSlider *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor ) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtIntPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
    }
}

/*!
    \class QtSliderFactory

    \brief The QtSliderFactory class provides QSlider widgets for
    properties created by QtIntPropertyManager objects.

    \sa QtAbstractEditorFactory, QtIntPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtSliderFactory::QtSliderFactory(QObject *parent)
    : QtAbstractEditorFactory<QtIntPropertyManager>(parent)
{
    d_ptr = new QtSliderFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtSliderFactory::~QtSliderFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtSliderFactory::connectPropertyManager(QtIntPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    connect(manager, SIGNAL(rangeChanged(QtProperty *, int, int)),
                this, SLOT(slotRangeChanged(QtProperty *, int, int)));
    connect(manager, SIGNAL(singleStepChanged(QtProperty *, int)),
                this, SLOT(slotSingleStepChanged(QtProperty *, int)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtSliderFactory::createEditor(QtIntPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QSlider *editor = new QSlider(Qt::Horizontal, parent);
    d_ptr->initializeEditor(property, editor);
    editor->setSingleStep(manager->singleStep(property));
    editor->setRange(manager->minimum(property), manager->maximum(property));
    editor->setValue(manager->value(property));

    connect(editor, SIGNAL(valueChanged(int)), this, SLOT(slotSetValue(int)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtSliderFactory::disconnectPropertyManager(QtIntPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(rangeChanged(QtProperty *, int, int)),
                this, SLOT(slotRangeChanged(QtProperty *, int, int)));
    disconnect(manager, SIGNAL(singleStepChanged(QtProperty *, int)),
                this, SLOT(slotSingleStepChanged(QtProperty *, int)));
}

// QtSliderFactory

class QtScrollBarFactoryPrivate : public  EditorFactoryPrivate<QScrollBar>
{
    QtScrollBarFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtScrollBarFactory)
public:
    QtScrollBarFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, int value);
    void slotRangeChanged(QtProperty *property, int min, int max);
    void slotSingleStepChanged(QtProperty *property, int step);
    void slotSetValue(int value);
};

void QtScrollBarFactoryPrivate::slotPropertyChanged(QtProperty *property, int value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QScrollBar *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QScrollBar *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setValue(value);
        editor->blockSignals(false);
    }
}

void QtScrollBarFactoryPrivate::slotRangeChanged(QtProperty *property, int min, int max)
{
    if (!m_createdEditors.contains(property))
        return;

    QtIntPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QScrollBar *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QScrollBar *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setRange(min, max);
        editor->setValue(manager->value(property));
        editor->blockSignals(false);
    }
}

void QtScrollBarFactoryPrivate::slotSingleStepChanged(QtProperty *property, int step)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<QScrollBar *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QScrollBar *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setSingleStep(step);
        editor->blockSignals(false);
    }
}

void QtScrollBarFactoryPrivate::slotSetValue(int value)
{
    QObject *object = q_ptr->sender();
    const QMap<QScrollBar *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QScrollBar *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtIntPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtScrollBarFactory

    \brief The QtScrollBarFactory class provides QScrollBar widgets for
    properties created by QtIntPropertyManager objects.

    \sa QtAbstractEditorFactory, QtIntPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtScrollBarFactory::QtScrollBarFactory(QObject *parent)
    : QtAbstractEditorFactory<QtIntPropertyManager>(parent)
{
    d_ptr = new QtScrollBarFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtScrollBarFactory::~QtScrollBarFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtScrollBarFactory::connectPropertyManager(QtIntPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    connect(manager, SIGNAL(rangeChanged(QtProperty *, int, int)),
                this, SLOT(slotRangeChanged(QtProperty *, int, int)));
    connect(manager, SIGNAL(singleStepChanged(QtProperty *, int)),
                this, SLOT(slotSingleStepChanged(QtProperty *, int)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtScrollBarFactory::createEditor(QtIntPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QScrollBar *editor = new QScrollBar(Qt::Horizontal, parent);
    d_ptr->initializeEditor(property, editor);
    editor->setSingleStep(manager->singleStep(property));
    editor->setRange(manager->minimum(property), manager->maximum(property));
    editor->setValue(manager->value(property));
    connect(editor, SIGNAL(valueChanged(int)), this, SLOT(slotSetValue(int)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtScrollBarFactory::disconnectPropertyManager(QtIntPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(rangeChanged(QtProperty *, int, int)),
                this, SLOT(slotRangeChanged(QtProperty *, int, int)));
    disconnect(manager, SIGNAL(singleStepChanged(QtProperty *, int)),
                this, SLOT(slotSingleStepChanged(QtProperty *, int)));
}

// QtCheckBoxFactory

class QtCheckBoxFactoryPrivate : public EditorFactoryPrivate<QtBoolEdit>
{
    QtCheckBoxFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtCheckBoxFactory)
public:
    QtCheckBoxFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, bool value);
    void slotTextVisibleChanged(QtProperty *property, bool textVisible);
    void slotSetValue(bool value);
};

void QtCheckBoxFactoryPrivate::slotPropertyChanged(QtProperty *property, bool value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtBoolEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtBoolEdit *editor = itEditor.next();
        editor->blockCheckBoxSignals(true);
        editor->setChecked(value);
        editor->blockCheckBoxSignals(false);
    }
}

void QtCheckBoxFactoryPrivate::slotTextVisibleChanged(QtProperty *property, bool textVisible)
{
    if (!m_createdEditors.contains(property))
        return;

    QtBoolPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QtBoolEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtBoolEdit *editor = itEditor.next();
        editor->setTextVisible(textVisible);
    }
}

void QtCheckBoxFactoryPrivate::slotSetValue(bool value)
{
    QObject *object = q_ptr->sender();

    const QMap<QtBoolEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtBoolEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend;  ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtBoolPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtCheckBoxFactory

    \brief The QtCheckBoxFactory class provides QCheckBox widgets for
    properties created by QtBoolPropertyManager objects.

    \sa QtAbstractEditorFactory, QtBoolPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtCheckBoxFactory::QtCheckBoxFactory(QObject *parent)
    : QtAbstractEditorFactory<QtBoolPropertyManager>(parent)
{
    d_ptr = new QtCheckBoxFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtCheckBoxFactory::~QtCheckBoxFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCheckBoxFactory::connectPropertyManager(QtBoolPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, bool)),
                this, SLOT(slotPropertyChanged(QtProperty *, bool)));
    connect(manager, SIGNAL(textVisibleChanged(QtProperty *, bool)),
                this, SLOT(slotTextVisibleChanged(QtProperty *, bool)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtCheckBoxFactory::createEditor(QtBoolPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    qDebug() << Q_FUNC_INFO;
    QtBoolEdit *editor = d_ptr->createEditor(property, parent);
    editor->setChecked(manager->value(property));
    editor->setTextVisible(manager->textVisible(property));

    connect(editor, SIGNAL(toggled(bool)), this, SLOT(slotSetValue(bool)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCheckBoxFactory::disconnectPropertyManager(QtBoolPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, bool)),
                this, SLOT(slotPropertyChanged(QtProperty *, bool)));
    disconnect(manager, SIGNAL(textVisibleChanged(QtProperty *, bool)),
                this, SLOT(slotTextVisibleChanged(QtProperty *, bool)));
}

// QtDoubleSpinBoxFactory
class DoubleEditor : public QWidget
{
    Q_OBJECT
public:
    DoubleEditor(QWidget *parent = 0)
        : QWidget(parent)
    {
        _editor = new QDoubleSpinBox(this);
        connect(_editor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &DoubleEditor::valueChanged);
        _suffix = new QLabel(this);
        _suffix->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(_editor);
        layout->addWidget(_suffix);

        setLayout(layout);
    }

    QDoubleSpinBox *editor() const { return _editor; }
    QString suffix() const { return _suffix->text(); }

signals:
    void suffixChanged(const QString &suffix);
    void valueChanged(double);

public slots:
    void setSuffix(const QString &suffix) {
        if (_suffix->text().compare(suffix) == 0) {
            return ;
        }
        _suffix->setText(suffix);
        emit suffixChanged(_suffix->text());
    }

private:
    QDoubleSpinBox *_editor = nullptr;
    QLabel *_suffix = nullptr;
};
class QtDoubleSpinBoxFactoryPrivate : public EditorFactoryPrivate<DoubleEditor>
{
    QtDoubleSpinBoxFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtDoubleSpinBoxFactory)
public:
    QtDoubleSpinBoxFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, double value);
    void slotRangeChanged(QtProperty *property, double min, double max);
    void slotSingleStepChanged(QtProperty *property, double step);
    void slotDecimalsChanged(QtProperty *property, int prec);
    void slotReadOnlyChanged(QtProperty *property, bool readOnly);
    void slotSuffixChanged(QtProperty *property, const QString &suffix);
    void slotSetValue(double value);
};

void QtDoubleSpinBoxFactoryPrivate::slotPropertyChanged(QtProperty *property, double value)
{
    QList<DoubleEditor *> editors = m_createdEditors[property];
    QListIterator<DoubleEditor *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        DoubleEditor *editor = itEditor.next();
        if (editor->editor()->value() != value) {
            editor->editor()->blockSignals(true);
            editor->editor()->setValue(value);
            editor->editor()->blockSignals(false);
        }
    }
}

void QtDoubleSpinBoxFactoryPrivate::slotRangeChanged(QtProperty *property,
            double min, double max)
{
    if (!m_createdEditors.contains(property))
        return;

    QtDoublePropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QList<DoubleEditor *> editors = m_createdEditors[property];
    QListIterator<DoubleEditor *> itEditor(editors);
    while (itEditor.hasNext()) {
        DoubleEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setRange(min, max);
        editor->editor()->setValue(manager->value(property));
        editor->editor()->blockSignals(false);
    }
}

void QtDoubleSpinBoxFactoryPrivate::slotSingleStepChanged(QtProperty *property, double step)
{
    if (!m_createdEditors.contains(property))
        return;

    QtDoublePropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QList<DoubleEditor *> editors = m_createdEditors[property];
    QListIterator<DoubleEditor *> itEditor(editors);
    while (itEditor.hasNext()) {
        DoubleEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setSingleStep(step);
        editor->editor()->blockSignals(false);
    }
}

void QtDoubleSpinBoxFactoryPrivate::slotReadOnlyChanged( QtProperty *property, bool readOnly)
{
    if (!m_createdEditors.contains(property))
        return;

    QtDoublePropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<DoubleEditor *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        DoubleEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setReadOnly(readOnly);
        editor->editor()->blockSignals(false);
    }
}

void QtDoubleSpinBoxFactoryPrivate::slotSuffixChanged(QtProperty *property, const QString &suffix)
{
    const auto it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.cend())
        return;

    QtDoublePropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    for (DoubleEditor *editor : it.value()) {
        editor->editor()->blockSignals(true);
        editor->setSuffix(suffix);
        editor->editor()->blockSignals(false);
    }
}
void QtDoubleSpinBoxFactoryPrivate::slotDecimalsChanged(QtProperty *property, int prec)
{
    if (!m_createdEditors.contains(property))
        return;

    QtDoublePropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QList<DoubleEditor *> editors = m_createdEditors[property];
    QListIterator<DoubleEditor *> itEditor(editors);
    while (itEditor.hasNext()) {
        DoubleEditor *editor = itEditor.next();
        editor->editor()->blockSignals(true);
        editor->editor()->setDecimals(prec);
        editor->editor()->setValue(manager->value(property));
        editor->editor()->blockSignals(false);
    }
}

void QtDoubleSpinBoxFactoryPrivate::slotSetValue(double value)
{
    QObject *object = q_ptr->sender();
    const QMap<DoubleEditor *, QtProperty *>::ConstIterator itcend = m_editorToProperty.constEnd();
    for (QMap<DoubleEditor *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != itcend; ++itEditor) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtDoublePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
    }
}

/*! \class QtDoubleSpinBoxFactory

    \brief The QtDoubleSpinBoxFactory class provides QDoubleSpinBox
    widgets for properties created by QtDoublePropertyManager objects.

    \sa QtAbstractEditorFactory, QtDoublePropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtDoubleSpinBoxFactory::QtDoubleSpinBoxFactory(QObject *parent)
    : QtAbstractEditorFactory<QtDoublePropertyManager>(parent)
{
    d_ptr = new QtDoubleSpinBoxFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtDoubleSpinBoxFactory::~QtDoubleSpinBoxFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtDoubleSpinBoxFactory::connectPropertyManager(QtDoublePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(slotPropertyChanged(QtProperty *, double)));
    connect(manager, SIGNAL(rangeChanged(QtProperty *, double, double)),
                this, SLOT(slotRangeChanged(QtProperty *, double, double)));
    connect(manager, SIGNAL(singleStepChanged(QtProperty *, double)),
                this, SLOT(slotSingleStepChanged(QtProperty *, double)));
    connect(manager, SIGNAL(decimalsChanged(QtProperty *, int)),
                this, SLOT(slotDecimalsChanged(QtProperty *, int)));
    connect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
                this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtDoubleSpinBoxFactory::createEditor(QtDoublePropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    DoubleEditor *editor = d_ptr->createEditor(property, parent);
    editor->editor()->setSingleStep(manager->singleStep(property));
    editor->setSuffix(manager->suffix(property));
    editor->editor()->setDecimals(manager->decimals(property));
    editor->editor()->setRange(manager->minimum(property), manager->maximum(property));
    editor->editor()->setValue(manager->value(property));
    editor->editor()->setKeyboardTracking(false);
    editor->editor()->setReadOnly(manager->isReadOnly(property));

    connect(editor, SIGNAL(valueChanged(double)), this, SLOT(slotSetValue(double)));
    connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtDoubleSpinBoxFactory::disconnectPropertyManager(QtDoublePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(slotPropertyChanged(QtProperty *, double)));
    disconnect(manager, SIGNAL(rangeChanged(QtProperty *, double, double)),
                this, SLOT(slotRangeChanged(QtProperty *, double, double)));
    disconnect(manager, SIGNAL(singleStepChanged(QtProperty *, double)),
                this, SLOT(slotSingleStepChanged(QtProperty *, double)));
    disconnect(manager, SIGNAL(decimalsChanged(QtProperty *, int)),
                this, SLOT(slotDecimalsChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
                this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

// QtLineEditFactory
#include "qthistoricalrecordlineedit.h"
class QtLineEditFactoryPrivate : public EditorFactoryPrivate<QHistoricalRecordLineEdit>
{
    QtLineEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtLineEditFactory)
public:
    QtLineEditFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QString &value);
    void slotRegExpChanged(QtProperty *property, const QRegExp &regExp);
    void slotSetValue(const QString &value);
    void slotEchoModeChanged(QtProperty *, int);
    void slotReadOnlyChanged(QtProperty *, bool);
    void slotEditingFinished();//by shu
};

void QtLineEditFactoryPrivate::slotPropertyChanged(QtProperty *property,
                const QString &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QHistoricalRecordLineEdit *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
		QHistoricalRecordLineEdit *editor = itEditor.next();
        if (editor->text() != value) {
            editor->blockSignals(true);
            editor->setText(value);
            editor->blockSignals(false);
        }
    }
}

void QtLineEditFactoryPrivate::slotRegExpChanged(QtProperty *property,
            const QRegExp &regExp)
{
    if (!m_createdEditors.contains(property))
        return;

    QtStringPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QHistoricalRecordLineEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
		QHistoricalRecordLineEdit *editor = itEditor.next();
        editor->blockSignals(true);
        const QValidator *oldValidator = editor->validator();
        QValidator *newValidator = 0;
        if (regExp.isValid()) {
            newValidator = new QRegExpValidator(regExp, editor);
        }
        editor->setValidator(newValidator);
        if (oldValidator)
            delete oldValidator;
        editor->blockSignals(false);
    }
}

void QtLineEditFactoryPrivate::slotEchoModeChanged(QtProperty *property, int echoMode)
{
    if (!m_createdEditors.contains(property))
        return;

    QtStringPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QHistoricalRecordLineEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
		QHistoricalRecordLineEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setEchoMode((EchoMode)echoMode);
        editor->blockSignals(false);
    }
}

void QtLineEditFactoryPrivate::slotReadOnlyChanged( QtProperty *property, bool readOnly)
{
    if (!m_createdEditors.contains(property))
        return;

    QtStringPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QHistoricalRecordLineEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
		QHistoricalRecordLineEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setReadOnly(readOnly);
        editor->blockSignals(false);
    }
}
void QtLineEditFactoryPrivate::slotEditingFinished()
{
    QObject *object = q_ptr->sender();
    const QMap<QHistoricalRecordLineEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QHistoricalRecordLineEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtStringPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;

			if (itEditor.key())
			{
				manager->setValue(property, itEditor.key()->text());
			}
            return;
        }
    }
}

void QtLineEditFactoryPrivate::slotSetValue(const QString &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QHistoricalRecordLineEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QHistoricalRecordLineEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtStringPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtLineEditFactory

    \brief The QtLineEditFactory class provides QLineEdit widgets for
    properties created by QtStringPropertyManager objects.

    \sa QtAbstractEditorFactory, QtStringPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtLineEditFactory::QtLineEditFactory(QObject *parent)
    : QtAbstractEditorFactory<QtStringPropertyManager>(parent)
{
    d_ptr = new QtLineEditFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtLineEditFactory::~QtLineEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}


/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtLineEditFactory::connectPropertyManager(QtStringPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
            this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    connect(manager, SIGNAL(regExpChanged(QtProperty *, const QRegExp &)),
            this, SLOT(slotRegExpChanged(QtProperty *, const QRegExp &)));
    connect(manager, SIGNAL(echoModeChanged(QtProperty*, int)),
            this, SLOT(slotEchoModeChanged(QtProperty *, int)));
    connect(manager, SIGNAL(readOnlyChanged(QtProperty*, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtLineEditFactory::createEditor(QtStringPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QLineEdit *editor = d_ptr->createEditor(property, parent);
    editor->setEchoMode((EchoMode)manager->echoMode(property));
    editor->setReadOnly(manager->isReadOnly(property));
    QRegExp regExp = manager->regExp(property);
    if (regExp.isValid()) {
        QValidator *validator = new QRegExpValidator(regExp, editor);
        editor->setValidator(validator);
    }
    editor->setText(manager->value(property));

    connect(editor, SIGNAL(textEdited(const QString &)),
                this, SLOT(slotSetValue(const QString &)));
    connect(editor, SIGNAL(destroyed(QObject*)),
            this,SLOT(slotEditingFinished()));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtLineEditFactory::disconnectPropertyManager(QtStringPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    disconnect(manager, SIGNAL(regExpChanged(QtProperty *, const QRegExp &)),
                this, SLOT(slotRegExpChanged(QtProperty *, const QRegExp &)));
    disconnect(manager, SIGNAL(echoModeChanged(QtProperty*,int)),
                this, SLOT(slotEchoModeChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(readOnlyChanged(QtProperty*, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));

}

// QtDateEditFactory

class QtDateEditFactoryPrivate : public EditorFactoryPrivate<QDateEdit>
{
    QtDateEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtDateEditFactory)
public:
    QtDateEditFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QDate &value);
    void slotRangeChanged(QtProperty *property, const QDate &min, const QDate &max);
    void slotSetValue(const QDate &value);
};

void QtDateEditFactoryPrivate::slotPropertyChanged(QtProperty *property, const QDate &value)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<QDateEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QDateEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setDate(value);
        editor->blockSignals(false);
    }
}

void QtDateEditFactoryPrivate::slotRangeChanged(QtProperty *property,
                const QDate &min, const QDate &max)
{
    if (!m_createdEditors.contains(property))
        return;

    QtDatePropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QDateEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QDateEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setDateRange(min, max);
        editor->setDate(manager->value(property));
        editor->blockSignals(false);
    }
}

void QtDateEditFactoryPrivate::slotSetValue(const QDate &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QDateEdit *, QtProperty *>::ConstIterator  ecend = m_editorToProperty.constEnd();
    for (QMap<QDateEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtDatePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtDateEditFactory

    \brief The QtDateEditFactory class provides QDateEdit widgets for
    properties created by QtDatePropertyManager objects.

    \sa QtAbstractEditorFactory, QtDatePropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtDateEditFactory::QtDateEditFactory(QObject *parent)
    : QtAbstractEditorFactory<QtDatePropertyManager>(parent)
{
    d_ptr = new QtDateEditFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtDateEditFactory::~QtDateEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtDateEditFactory::connectPropertyManager(QtDatePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QDate &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QDate &)));
    connect(manager, SIGNAL(rangeChanged(QtProperty *, const QDate &, const QDate &)),
                this, SLOT(slotRangeChanged(QtProperty *, const QDate &, const QDate &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtDateEditFactory::createEditor(QtDatePropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QDateEdit *editor = d_ptr->createEditor(property, parent);
    editor->setCalendarPopup(true);
    editor->setDateRange(manager->minimum(property), manager->maximum(property));
    editor->setDate(manager->value(property));

    connect(editor, SIGNAL(dateChanged(const QDate &)),
                this, SLOT(slotSetValue(const QDate &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtDateEditFactory::disconnectPropertyManager(QtDatePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QDate &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QDate &)));
    disconnect(manager, SIGNAL(rangeChanged(QtProperty *, const QDate &, const QDate &)),
                this, SLOT(slotRangeChanged(QtProperty *, const QDate &, const QDate &)));
}

// QtTimeEditFactory

class QtTimeEditFactoryPrivate : public EditorFactoryPrivate<QTimeEdit>
{
    QtTimeEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtTimeEditFactory)
public:
    QtTimeEditFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QTime &value);
    void slotSetValue(const QTime &value);
};

void QtTimeEditFactoryPrivate::slotPropertyChanged(QtProperty *property, const QTime &value)
{
    if (!m_createdEditors.contains(property))
        return;
    QListIterator<QTimeEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QTimeEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setTime(value);
        editor->blockSignals(false);
    }
}

void QtTimeEditFactoryPrivate::slotSetValue(const QTime &value)
{
    QObject *object = q_ptr->sender();
    const  QMap<QTimeEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QTimeEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtTimePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtTimeEditFactory

    \brief The QtTimeEditFactory class provides QTimeEdit widgets for
    properties created by QtTimePropertyManager objects.

    \sa QtAbstractEditorFactory, QtTimePropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtTimeEditFactory::QtTimeEditFactory(QObject *parent)
    : QtAbstractEditorFactory<QtTimePropertyManager>(parent)
{
    d_ptr = new QtTimeEditFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtTimeEditFactory::~QtTimeEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtTimeEditFactory::connectPropertyManager(QtTimePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QTime &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QTime &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtTimeEditFactory::createEditor(QtTimePropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QTimeEdit *editor = d_ptr->createEditor(property, parent);
    editor->setTime(manager->value(property));

    connect(editor, SIGNAL(timeChanged(const QTime &)),
                this, SLOT(slotSetValue(const QTime &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtTimeEditFactory::disconnectPropertyManager(QtTimePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QTime &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QTime &)));
}

// QtDateTimeEditFactory

class QtDateTimeEditFactoryPrivate : public EditorFactoryPrivate<QDateTimeEdit>
{
    QtDateTimeEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtDateTimeEditFactory)
public:
    QtDateTimeEditFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QDateTime &value);
    void slotSetValue(const QDateTime &value);

};

void QtDateTimeEditFactoryPrivate::slotPropertyChanged(QtProperty *property,
            const QDateTime &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QDateTimeEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QDateTimeEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setDateTime(value);
        editor->blockSignals(false);
    }
}

void QtDateTimeEditFactoryPrivate::slotSetValue(const QDateTime &value)
{
    QObject *object = q_ptr->sender();
    const  QMap<QDateTimeEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QDateTimeEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend;  ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtDateTimePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtDateTimeEditFactory

    \brief The QtDateTimeEditFactory class provides QDateTimeEdit
    widgets for properties created by QtDateTimePropertyManager objects.

    \sa QtAbstractEditorFactory, QtDateTimePropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtDateTimeEditFactory::QtDateTimeEditFactory(QObject *parent)
    : QtAbstractEditorFactory<QtDateTimePropertyManager>(parent)
{
    d_ptr = new QtDateTimeEditFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtDateTimeEditFactory::~QtDateTimeEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtDateTimeEditFactory::connectPropertyManager(QtDateTimePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QDateTime &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QDateTime &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtDateTimeEditFactory::createEditor(QtDateTimePropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QDateTimeEdit *editor =  d_ptr->createEditor(property, parent);
    editor->setDateTime(manager->value(property));

    connect(editor, SIGNAL(dateTimeChanged(const QDateTime &)),
                this, SLOT(slotSetValue(const QDateTime &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtDateTimeEditFactory::disconnectPropertyManager(QtDateTimePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QDateTime &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QDateTime &)));
}

// QtKeySequenceEditorFactory

class QtKeySequenceEditorFactoryPrivate : public EditorFactoryPrivate<QtKeySequenceEdit>
{
    QtKeySequenceEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtKeySequenceEditorFactory)
public:
    QtKeySequenceEditorFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QKeySequence &value);
    void slotSetValue(const QKeySequence &value);
};

void QtKeySequenceEditorFactoryPrivate::slotPropertyChanged(QtProperty *property,
            const QKeySequence &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtKeySequenceEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtKeySequenceEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setKeySequence(value);
        editor->blockSignals(false);
    }
}

void QtKeySequenceEditorFactoryPrivate::slotSetValue(const QKeySequence &value)
{
    QObject *object = q_ptr->sender();
    const  QMap<QtKeySequenceEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtKeySequenceEdit *, QtProperty *>::ConstIterator itEditor =  m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtKeySequencePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtKeySequenceEditorFactory

    \brief The QtKeySequenceEditorFactory class provides editor
    widgets for properties created by QtKeySequencePropertyManager objects.

    \sa QtAbstractEditorFactory
*/

/*!
    Creates a factory with the given \a parent.
*/
QtKeySequenceEditorFactory::QtKeySequenceEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<QtKeySequencePropertyManager>(parent)
{
    d_ptr = new QtKeySequenceEditorFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtKeySequenceEditorFactory::~QtKeySequenceEditorFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtKeySequenceEditorFactory::connectPropertyManager(QtKeySequencePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QKeySequence &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QKeySequence &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtKeySequenceEditorFactory::createEditor(QtKeySequencePropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QtKeySequenceEdit *editor = d_ptr->createEditor(property, parent);
    editor->setKeySequence(manager->value(property));

    connect(editor, SIGNAL(keySequenceChanged(const QKeySequence &)),
                this, SLOT(slotSetValue(const QKeySequence &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtKeySequenceEditorFactory::disconnectPropertyManager(QtKeySequencePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QKeySequence &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QKeySequence &)));
}

// QtCharEdit

class QtCharEdit : public QWidget
{
    Q_OBJECT
public:
    explicit QtCharEdit(QWidget *parent = 0);

    QChar value() const;
    bool eventFilter(QObject *o, QEvent *e);
public Q_SLOTS:
    void setValue(const QChar &value);
Q_SIGNALS:
    void valueChanged(const QChar &value);
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *);
    bool event(QEvent *e);
private slots:
    void slotClearChar();
private:
    void handleKeyEvent(QKeyEvent *e);

    QChar m_value;
    QLineEdit *m_lineEdit;
};

QtCharEdit::QtCharEdit(QWidget *parent)
    : QWidget(parent),  m_lineEdit(new QLineEdit(this))
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->setMargin(0);
    m_lineEdit->installEventFilter(this);
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setFocusProxy(this);
    setFocusPolicy(m_lineEdit->focusPolicy());
    setAttribute(Qt::WA_InputMethodEnabled);
}

bool QtCharEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_lineEdit && e->type() == QEvent::ContextMenu) {
        QContextMenuEvent *c = static_cast<QContextMenuEvent *>(e);
        QMenu *menu = m_lineEdit->createStandardContextMenu();
        QList<QAction *> actions = menu->actions();
        QListIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            action->setShortcut(QKeySequence());
            QString actionString = action->text();
            const int pos = actionString.lastIndexOf(QLatin1Char('\t'));
            if (pos > 0)
                actionString = actionString.remove(pos, actionString.length() - pos);
            action->setText(actionString);
        }
        QAction *actionBefore = 0;
        if (actions.count() > 0)
            actionBefore = actions[0];
        QAction *clearAction = new QAction(tr("Clear Char"), menu);
        menu->insertAction(actionBefore, clearAction);
        menu->insertSeparator(actionBefore);
        clearAction->setEnabled(!m_value.isNull());
        connect(clearAction, SIGNAL(triggered()), this, SLOT(slotClearChar()));
        menu->exec(c->globalPos());
        delete menu;
        e->accept();
        return true;
    }

    return QWidget::eventFilter(o, e);
}

void QtCharEdit::slotClearChar()
{
    if (m_value.isNull())
        return;
    setValue(QChar());
    emit valueChanged(m_value);
}

void QtCharEdit::handleKeyEvent(QKeyEvent *e)
{
    const int key = e->key();
    switch (key) {
    case Qt::Key_Control:
    case Qt::Key_Shift:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
    case Qt::Key_Super_L:
    case Qt::Key_Return:
        return;
    default:
        break;
    }

    const QString text = e->text();
    if (text.count() != 1)
        return;

    const QChar c = text.at(0);
    if (!c.isPrint())
        return;

    if (m_value == c)
        return;

    m_value = c;
    const QString str = m_value.isNull() ? QString() : QString(m_value);
    m_lineEdit->setText(str);
    e->accept();
    emit valueChanged(m_value);
}

void QtCharEdit::setValue(const QChar &value)
{
    if (value == m_value)
        return;

    m_value = value;
    QString str = value.isNull() ? QString() : QString(value);
    m_lineEdit->setText(str);
}

QChar QtCharEdit::value() const
{
    return m_value;
}

void QtCharEdit::focusInEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    m_lineEdit->selectAll();
    QWidget::focusInEvent(e);
}

void QtCharEdit::focusOutEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void QtCharEdit::keyPressEvent(QKeyEvent *e)
{
    handleKeyEvent(e);
    e->accept();
}

void QtCharEdit::keyReleaseEvent(QKeyEvent *e)
{
    m_lineEdit->event(e);
}

void QtCharEdit::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool QtCharEdit::event(QEvent *e)
{
    switch(e->type()) {
    case QEvent::Shortcut:
    case QEvent::ShortcutOverride:
    case QEvent::KeyRelease:
        e->accept();
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}

// QtCharEditorFactory

class QtCharEditorFactoryPrivate : public EditorFactoryPrivate<QtCharEdit>
{
    QtCharEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtCharEditorFactory)
public:
    QtCharEditorFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QChar &value);
    void slotSetValue(const QChar &value);

};

void QtCharEditorFactoryPrivate::slotPropertyChanged(QtProperty *property,
            const QChar &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtCharEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtCharEdit *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setValue(value);
        editor->blockSignals(false);
    }
}

void QtCharEditorFactoryPrivate::slotSetValue(const QChar &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QtCharEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtCharEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend;  ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtCharPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtCharEditorFactory

    \brief The QtCharEditorFactory class provides editor
    widgets for properties created by QtCharPropertyManager objects.

    \sa QtAbstractEditorFactory
*/

/*!
    Creates a factory with the given \a parent.
*/
QtCharEditorFactory::QtCharEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<QtCharPropertyManager>(parent)
{
    d_ptr = new QtCharEditorFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtCharEditorFactory::~QtCharEditorFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCharEditorFactory::connectPropertyManager(QtCharPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QChar &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QChar &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtCharEditorFactory::createEditor(QtCharPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QtCharEdit *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));

    connect(editor, SIGNAL(valueChanged(const QChar &)),
                this, SLOT(slotSetValue(const QChar &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCharEditorFactory::disconnectPropertyManager(QtCharPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QChar &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QChar &)));
}

// QtEnumEditorFactory

class QtEnumEditorFactoryPrivate : public EditorFactoryPrivate<QComboBox>
{
    QtEnumEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtEnumEditorFactory)
public:
    QtEnumEditorFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, int value);
    void slotEnumNamesChanged(QtProperty *property, const QStringList &);
    void slotEnumIconsChanged(QtProperty *property, const QMap<int, QIcon> &);
    void slotSetValue(int value);
};

void QtEnumEditorFactoryPrivate::slotPropertyChanged(QtProperty *property, int value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QComboBox *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QComboBox *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setCurrentIndex(value);
        editor->blockSignals(false);
    }
}

void QtEnumEditorFactoryPrivate::slotEnumNamesChanged(QtProperty *property,
                const QStringList &enumNames)
{
    if (!m_createdEditors.contains(property))
        return;

    QtEnumPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QMap<int, QIcon> enumIcons = manager->enumIcons(property);

    QListIterator<QComboBox *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QComboBox *editor = itEditor.next();
        editor->blockSignals(true);
        editor->clear();
        editor->addItems(enumNames);
        const int nameCount = enumNames.count();
        for (int i = 0; i < nameCount; i++)
            editor->setItemIcon(i, enumIcons.value(i));
        editor->setCurrentIndex(manager->value(property));
        editor->blockSignals(false);
    }
}

void QtEnumEditorFactoryPrivate::slotEnumIconsChanged(QtProperty *property,
                const QMap<int, QIcon> &enumIcons)
{
    if (!m_createdEditors.contains(property))
        return;

    QtEnumPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    const QStringList enumNames = manager->enumNames(property);
    QListIterator<QComboBox *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QComboBox *editor = itEditor.next();
        editor->blockSignals(true);
        const int nameCount = enumNames.count();
        for (int i = 0; i < nameCount; i++)
            editor->setItemIcon(i, enumIcons.value(i));
        editor->setCurrentIndex(manager->value(property));
        editor->blockSignals(false);
    }
}

void QtEnumEditorFactoryPrivate::slotSetValue(int value)
{
    QObject *object = q_ptr->sender();
    const  QMap<QComboBox *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QComboBox *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtEnumPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtEnumEditorFactory

    \brief The QtEnumEditorFactory class provides QComboBox widgets for
    properties created by QtEnumPropertyManager objects.

    \sa QtAbstractEditorFactory, QtEnumPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtEnumEditorFactory::QtEnumEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<QtEnumPropertyManager>(parent)
{
    d_ptr = new QtEnumEditorFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtEnumEditorFactory::~QtEnumEditorFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtEnumEditorFactory::connectPropertyManager(QtEnumPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    connect(manager, SIGNAL(enumNamesChanged(QtProperty *, const QStringList &)),
                this, SLOT(slotEnumNamesChanged(QtProperty *, const QStringList &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtEnumEditorFactory::createEditor(QtEnumPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QComboBox *editor = d_ptr->createEditor(property, parent);
    editor->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    editor->setMinimumContentsLength(1);
    editor->view()->setTextElideMode(Qt::ElideRight);
    QStringList enumNames = manager->enumNames(property);
    editor->addItems(enumNames);
    QMap<int, QIcon> enumIcons = manager->enumIcons(property);
    const int enumNamesCount = enumNames.count();
    for (int i = 0; i < enumNamesCount; i++)
        editor->setItemIcon(i, enumIcons.value(i));
    editor->setCurrentIndex(manager->value(property));

    connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetValue(int)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtEnumEditorFactory::disconnectPropertyManager(QtEnumPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotPropertyChanged(QtProperty *, int)));
    disconnect(manager, SIGNAL(enumNamesChanged(QtProperty *, const QStringList &)),
                this, SLOT(slotEnumNamesChanged(QtProperty *, const QStringList &)));
}

// QtCheckableEnumEditorFactory

class QtCheckableEnumEditorFactoryPrivate : public EditorFactoryPrivate<CheckableComboBox>
{
    QtCheckableEnumEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtCheckableEnumEditorFactory)
public:
    QtCheckableEnumEditorFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const CheckableEnumValue &value);
    void slotEnumNamesChanged(QtProperty *property, const QStringList &);
    void slotSetValue(const CheckableEnumValue &value);
};

void QtCheckableEnumEditorFactoryPrivate::slotPropertyChanged(QtProperty *property, const CheckableEnumValue &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<CheckableComboBox *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        CheckableComboBox *editor = itEditor.next();
        editor->blockSignals(true);
        editor->setCurrentIndexes(value);
        editor->blockSignals(false);
    }
}

void QtCheckableEnumEditorFactoryPrivate::slotEnumNamesChanged(QtProperty *property,
                const QStringList &enumNames)
{
    if (!m_createdEditors.contains(property))
        return;

    QtCheckableEnumPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<CheckableComboBox *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        CheckableComboBox *editor = itEditor.next();
        editor->blockSignals(true);
        editor->clear();
        editor->addItems(enumNames);
        editor->setCurrentIndexes(manager->value(property));
        editor->blockSignals(false);
    }
}

void QtCheckableEnumEditorFactoryPrivate::slotSetValue(const CheckableEnumValue &value)
{
    QObject *object = q_ptr->sender();
    const QMap<CheckableComboBox *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<CheckableComboBox *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtCheckableEnumPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtCheckableEnumEditorFactory

    \brief The QtCheckableEnumEditorFactory class provides QComboBox widgets for
    properties created by QtEnumPropertyManager objects.

    \sa QtAbstractEditorFactory, QtEnumPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtCheckableEnumEditorFactory::QtCheckableEnumEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<QtCheckableEnumPropertyManager>(parent)
{
    d_ptr = new QtCheckableEnumEditorFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtCheckableEnumEditorFactory::~QtCheckableEnumEditorFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCheckableEnumEditorFactory::connectPropertyManager(QtCheckableEnumPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const CheckableEnumValue &)),
                this, SLOT(slotPropertyChanged(QtProperty *, CheckableEnumValue)));
    connect(manager, SIGNAL(enumNamesChanged(QtProperty *, const QStringList &)),
                this, SLOT(slotEnumNamesChanged(QtProperty *, const QStringList &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtCheckableEnumEditorFactory::createEditor(QtCheckableEnumPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    CheckableComboBox *editor = d_ptr->createEditor(property, parent);
    editor->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    editor->setMinimumContentsLength(1);
    editor->view()->setTextElideMode(Qt::ElideRight);
    QStringList enumNames = manager->enumNames(property);
    editor->addItems(enumNames);
    editor->setCurrentIndexes(manager->value(property));

    connect(editor, SIGNAL(currentIndexesChanged(const CheckableEnumValue &)), this, SLOT(slotSetValue(const CheckableEnumValue &)));
    connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCheckableEnumEditorFactory::disconnectPropertyManager(QtCheckableEnumPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const CheckableEnumValue &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const CheckableEnumValue &)));
    disconnect(manager, SIGNAL(enumNamesChanged(QtProperty *, const QStringList &)),
                this, SLOT(slotEnumNamesChanged(QtProperty *, const QStringList &)));
}

// QtCursorEditorFactory

Q_GLOBAL_STATIC(QtCursorDatabase, cursorDatabase)

class QtCursorEditorFactoryPrivate
{
    QtCursorEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtCursorEditorFactory)
public:
    QtCursorEditorFactoryPrivate();

    void slotPropertyChanged(QtProperty *property, const QCursor &cursor);
    void slotEnumChanged(QtProperty *property, int value);
    void slotEditorDestroyed(QObject *object);

    QtEnumEditorFactory *m_enumEditorFactory;
    QtEnumPropertyManager *m_enumPropertyManager;

    QMap<QtProperty *, QtProperty *> m_propertyToEnum;
    QMap<QtProperty *, QtProperty *> m_enumToProperty;
    QMap<QtProperty *, QList<QWidget *> > m_enumToEditors;
    QMap<QWidget *, QtProperty *> m_editorToEnum;
    bool m_updatingEnum;
};

QtCursorEditorFactoryPrivate::QtCursorEditorFactoryPrivate()
    : q_ptr(NULL), m_enumEditorFactory(NULL), m_enumPropertyManager(NULL),
      m_updatingEnum(false)
{

}

void QtCursorEditorFactoryPrivate::slotPropertyChanged(QtProperty *property, const QCursor &cursor)
{
    // update enum property
    QtProperty *enumProp = m_propertyToEnum.value(property);
    if (!enumProp)
        return;

    m_updatingEnum = true;
    m_enumPropertyManager->setValue(enumProp, cursorDatabase()->cursorToValue(cursor));
    m_updatingEnum = false;
}

void QtCursorEditorFactoryPrivate::slotEnumChanged(QtProperty *property, int value)
{
    if (m_updatingEnum)
        return;
    // update cursor property
    QtProperty *prop = m_enumToProperty.value(property);
    if (!prop)
        return;
    QtCursorPropertyManager *cursorManager = q_ptr->propertyManager(prop);
    if (!cursorManager)
        return;
#ifndef QT_NO_CURSOR
    cursorManager->setValue(prop, QCursor(cursorDatabase()->valueToCursor(value)));
#endif
}

void QtCursorEditorFactoryPrivate::slotEditorDestroyed(QObject *object)
{
    // remove from m_editorToEnum map;
    // remove from m_enumToEditors map;
    // if m_enumToEditors doesn't contains more editors delete enum property;
    const  QMap<QWidget *, QtProperty *>::ConstIterator ecend = m_editorToEnum.constEnd();
    for (QMap<QWidget *, QtProperty *>::ConstIterator itEditor = m_editorToEnum.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QWidget *editor = itEditor.key();
            QtProperty *enumProp = itEditor.value();
            m_editorToEnum.remove(editor);
            m_enumToEditors[enumProp].removeAll(editor);
            if (m_enumToEditors[enumProp].isEmpty()) {
                m_enumToEditors.remove(enumProp);
                QtProperty *property = m_enumToProperty.value(enumProp);
                m_enumToProperty.remove(enumProp);
                m_propertyToEnum.remove(property);
                delete enumProp;
            }
            return;
        }
}

/*!
    \class QtCursorEditorFactory

    \brief The QtCursorEditorFactory class provides QComboBox widgets for
    properties created by QtCursorPropertyManager objects.

    \sa QtAbstractEditorFactory, QtCursorPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtCursorEditorFactory::QtCursorEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<QtCursorPropertyManager>(parent)
{
    d_ptr = new QtCursorEditorFactoryPrivate();
    d_ptr->q_ptr = this;

    d_ptr->m_enumEditorFactory = new QtEnumEditorFactory(this);
    d_ptr->m_enumPropertyManager = new QtEnumPropertyManager(this);
    connect(d_ptr->m_enumPropertyManager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(slotEnumChanged(QtProperty *, int)));
    d_ptr->m_enumEditorFactory->addPropertyManager(d_ptr->m_enumPropertyManager);
}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtCursorEditorFactory::~QtCursorEditorFactory()
{
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCursorEditorFactory::connectPropertyManager(QtCursorPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QCursor &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QCursor &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtCursorEditorFactory::createEditor(QtCursorPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QtProperty *enumProp = 0;
    if (d_ptr->m_propertyToEnum.contains(property)) {
        enumProp = d_ptr->m_propertyToEnum[property];
    } else {
        enumProp = d_ptr->m_enumPropertyManager->addProperty(property->propertyName());
        d_ptr->m_enumPropertyManager->setEnumNames(enumProp, cursorDatabase()->cursorShapeNames());
        d_ptr->m_enumPropertyManager->setEnumIcons(enumProp, cursorDatabase()->cursorShapeIcons());
#ifndef QT_NO_CURSOR
        d_ptr->m_enumPropertyManager->setValue(enumProp, cursorDatabase()->cursorToValue(manager->value(property)));
#endif
        d_ptr->m_propertyToEnum[property] = enumProp;
        d_ptr->m_enumToProperty[enumProp] = property;
    }
    QtAbstractEditorFactoryBase *af = d_ptr->m_enumEditorFactory;
    QWidget *editor = af->createEditor(enumProp, parent);
    d_ptr->m_enumToEditors[enumProp].append(editor);
    d_ptr->m_editorToEnum[editor] = enumProp;
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtCursorEditorFactory::disconnectPropertyManager(QtCursorPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QCursor &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QCursor &)));
}

// QtFileEditWidget
class QtFileEditWidget : public QWidget {
    Q_OBJECT
public:
    explicit QtFileEditWidget(QWidget *parent);

    bool eventFilter(QObject *obj, QEvent *ev);
    QString value() const;
    QStringList filters() const;

public Q_SLOTS:
    void setValue(const QString &value);
    void setFilters(const QStringList &filters);

Q_SIGNALS:
    void valueChanged(const QString &value);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void buttonClicked();

private:
    QString m_filepath;
    QStringList m_filters;
    QLabel *m_label;
    QToolButton *m_button;
};

QtFileEditWidget::QtFileEditWidget(QWidget *parent)
    : QWidget(parent),
      m_label(new QLabel),
      m_button(new QToolButton)
{
    QHBoxLayout *lt = new QHBoxLayout(this);
    setupTreeViewEditorMargin(lt);
    lt->setSpacing(0);
    lt->addWidget(m_label);
    lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    m_button->setFixedWidth(20);
    setFocusProxy(m_button);
    setFocusPolicy(m_button->focusPolicy());
    m_button->setText(tr("..."));
    m_button->installEventFilter(this);
    connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    lt->addWidget(m_button);
    m_label->setText(m_filepath);
}

void QtFileEditWidget::setValue(const QString &value)
{
    if (m_filepath != value) {
        m_filepath = value;
        m_label->setText(m_filepath);
    }
}

void QtFileEditWidget::setFilters(const QStringList &filters)
{
    if(m_filters != filters)
        m_filters = filters;
}

void QtFileEditWidget::buttonClicked()
{
    QFileInfo fileinfo(m_filepath);

    QString filter;
    for(int i = 0, iend = m_filters.count(); i < iend; i++)
        filter = filter + m_filters[i] + " ";

    QString newPath = QFileDialog::getOpenFileName(this, tr("Select Image"),
                                                   fileinfo.absolutePath(),
                                                   tr("Files (%1)").arg(filter));
    if (!newPath.isEmpty() && m_filepath != newPath) {
        setValue(newPath);
        emit valueChanged(m_filepath);
    }
    deleteLater();
}

bool QtFileEditWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_button) {
        switch (ev->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

QString QtFileEditWidget::value() const
{
    return m_filepath;
}

QStringList QtFileEditWidget::filters() const
{
    return m_filters;
}

void QtFileEditWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// QtRangeEditWidget
class QtRangeEditWidget : public QWidget {
    Q_OBJECT
public:
    explicit QtRangeEditWidget(QWidget *parent);

    Range value() const;
    bool readOnly() const;

public Q_SLOTS:
    void setValue(const Range &value);
    void setReadOnly(bool readOnly);

Q_SIGNALS:
    void valueChanged(const Range &value);
    void readOnlyChanged(bool readOnly);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void paintEvent(QPaintEvent *);

private:
    QSpinBox *m_fromEditor;
    QSpinBox *m_toEditor;
};

QtRangeEditWidget::QtRangeEditWidget(QWidget *parent)
    : QWidget(parent),
    m_fromEditor(new QSpinBox),
    m_toEditor(new QSpinBox)
{
    m_fromEditor->setMinimum(INT32_MIN);
    m_fromEditor->setMaximum(INT32_MAX);
    m_fromEditor->setFrame(false);
    m_fromEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_fromEditor->installEventFilter(this);
    m_toEditor->setFrame(false);
    m_toEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toEditor->setMinimum(INT32_MIN);
    m_toEditor->setMaximum(INT32_MAX);
    m_toEditor->installEventFilter(this);
    setFocusProxy(m_fromEditor);
    setFocusPolicy(m_fromEditor->focusPolicy());

    QHBoxLayout *lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(m_fromEditor);
    lt->addWidget(new QLabel(" ~ "));
    lt->addWidget(m_toEditor);

    connect(m_fromEditor, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val) {
        if (val > m_toEditor->value()) {
            m_toEditor->setValue(val);
        }
    });
    connect(m_toEditor, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val) {
        if (val < m_fromEditor->value()) {
            m_fromEditor->setValue(val);
        }
    });

    connect(m_fromEditor, &QSpinBox::editingFinished, this, [=](){
        if (!m_fromEditor->hasFocus() && !m_toEditor->hasFocus()){
            emit valueChanged(value());
        }
    });
    connect(m_toEditor, &QSpinBox::editingFinished, this, [=](){
        if (!m_fromEditor->hasFocus() && !m_toEditor->hasFocus()){
            emit valueChanged(value());
        }
    });
}
bool QtRangeEditWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_fromEditor || obj == m_toEditor) {
        switch (ev->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void QtRangeEditWidget::setValue(const Range &val)
{
    if (m_fromEditor->value() == val.from() && m_toEditor->value() == val.to()) {
        return ;
    }

    qDebug() << Q_FUNC_INFO << val;

    m_fromEditor->setValue(val.from());
    m_toEditor->setValue(val.to());
    emit valueChanged(val);
}

void QtRangeEditWidget::setReadOnly(bool readOnly)
{
    if (m_fromEditor->isReadOnly() == readOnly) {
        return ;
    }

    m_fromEditor->setReadOnly(readOnly);
    m_toEditor->setReadOnly(readOnly);
    emit readOnlyChanged(readOnly);
}

Range QtRangeEditWidget::value() const
{
    return Range(m_fromEditor->value(), m_toEditor->value());
}

bool QtRangeEditWidget::readOnly() const
{
    return m_fromEditor->isReadOnly();
}

void QtRangeEditWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// QtRangeFEditWidget
class QtRangeFEditWidget : public QWidget {
    Q_OBJECT
public:
    explicit QtRangeFEditWidget(QWidget *parent);

    RangeF value() const;
    bool readOnly() const;

public Q_SLOTS:
    void setValue(const RangeF &value);
    void setReadOnly(bool readOnly);

Q_SIGNALS:
    void valueChanged(const RangeF &value);
    void readOnlyChanged(bool readOnly);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void paintEvent(QPaintEvent *);

private:
    QDoubleSpinBox *m_fromEditor;
    QDoubleSpinBox *m_toEditor;
};

QtRangeFEditWidget::QtRangeFEditWidget(QWidget *parent)
    : QWidget(parent),
    m_fromEditor(new QDoubleSpinBox),
    m_toEditor(new QDoubleSpinBox)
{
    m_fromEditor->setFrame(false);
    m_fromEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_fromEditor->setMinimum(FLT_MIN);
    m_fromEditor->setMaximum(FLT_MAX);
    m_fromEditor->installEventFilter(this);
    m_toEditor->setFrame(false);
    m_toEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toEditor->setMinimum(FLT_MIN);
    m_toEditor->setMaximum(FLT_MAX);
    m_toEditor->installEventFilter(this);
    setFocusProxy(m_fromEditor);
    setFocusPolicy(m_fromEditor->focusPolicy());

    QHBoxLayout *lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(m_fromEditor);
    lt->addWidget(new QLabel(" ~ "));
    lt->addWidget(m_toEditor);

    connect(m_fromEditor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        if (val > m_toEditor->value()) {
            m_toEditor->setValue(val);
        }
    });
    connect(m_toEditor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val){
        if (val < m_fromEditor->value()) {
            m_fromEditor->setValue(val);
        }
    });

    connect(m_fromEditor, &QDoubleSpinBox::editingFinished, this, [=](){
        if (!m_fromEditor->hasFocus() && !m_toEditor->hasFocus()){
            emit valueChanged(value());
        }
    });
    connect(m_toEditor, &QDoubleSpinBox::editingFinished, this, [=](){
        if (!m_fromEditor->hasFocus() && !m_toEditor->hasFocus()){
            emit valueChanged(value());
        }
    });
}

void QtRangeFEditWidget::setValue(const RangeF &val)
{
    if (qFuzzyCompare(m_fromEditor->value(), val.from()) && qFuzzyCompare(m_toEditor->value(), val.to())) {
        return ;
    }

    m_fromEditor->setValue(val.from());
    m_toEditor->setValue(val.to());
    emit valueChanged(val);
}

void QtRangeFEditWidget::setReadOnly(bool readOnly)
{
    if (m_fromEditor->isReadOnly() == readOnly) {
        return ;
    }

    m_fromEditor->setReadOnly(readOnly);
    m_toEditor->setReadOnly(readOnly);
    emit readOnlyChanged(readOnly);
}

RangeF QtRangeFEditWidget::value() const
{
    return RangeF(m_fromEditor->value(), m_toEditor->value());
}

bool QtRangeFEditWidget::readOnly() const
{
    return m_fromEditor->isReadOnly();
}

bool QtRangeFEditWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_fromEditor || obj == m_toEditor) {
        switch (ev->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void QtRangeFEditWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// QtDialogEditWidget
class QtDialogWidget : public QWidget
{
    Q_OBJECT
public:
    QtDialogWidget(QWidget *parent);

    bool eventFilter(QObject *obj, QEvent *ev);
    QString key() const;
    QVariant value() const;
    QString valueText() const;
    QString className() const;
    bool readOnly() const;

public Q_SLOTS:
    void setKey(const QString &key);
    void setValue(const QVariant &value);
    void setValueText(const QString &valueText);
    void setClassName(const QString &className);
    void setReadOnly(bool readOnly);

Q_SIGNALS:
    void valueChanged(const QVariant &value);
    void valueTextChanged(const QString &valueText);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void buttonClicked();

private:
    QString m_key;
    QVariant m_value;
    QString m_valueText;
    QString m_className;

    QLineEdit *m_label;
    QToolButton *m_button;
    bool m_readOnly;

};

QtDialogWidget::QtDialogWidget(QWidget *parent)
    : QWidget(parent)
{
    m_label  = new QLineEdit(this);
    m_label->setReadOnly(true);
    m_button = new QToolButton(this);

    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    QHBoxLayout *lt = new QHBoxLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);
    lt->addWidget(m_label);

    m_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored);
    m_button->setFixedWidth(m_button->height());
    setFocusProxy(m_button);
    setFocusPolicy(m_button->focusPolicy());
    m_button->setText(tr("..."));
    m_button->installEventFilter(this);
    connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    lt->addWidget(m_button);
}

void QtDialogWidget::setValue(const QVariant &value)
{
    if (m_value != value) {
        m_value = value;
    }
}

void QtDialogWidget::setValueText(const QString &valueText)
{
    if (m_valueText != valueText) {
        m_valueText = valueText;
        m_label->setText(m_valueText);
    }
}

void QtDialogWidget::buttonClicked()
{
    int type = QMetaType::type(QStringLiteral("%1*").arg(m_className).toUtf8().data());
    const QMetaObject *metaObj = QMetaType::metaObjectForType(type);
    if (metaObj == nullptr)
    {
        QMessageBox::warning(0, tr("Warning"), tr("Cannot find the dialog: '%1', please using qRegisterMetatype() to register it first.").arg(className()));
        return ;
    }
    QObject *obj = metaObj->newInstance();
    QDialog *dialog = qobject_cast<QDialog*>(obj);

    if (dialog == nullptr)
    {
        QMessageBox::warning(0, tr("Warning"), tr("Cannot instance dialog: '%1', please add Q_INVOKABLE before the default constructor.").arg(className()));
        return ;
    }

    obj->setProperty("windowTitle", windowTitle());
    QMetaObject::invokeMethod(obj, "setValue", Q_ARG(QVariant, value()));
    if (QDialog::Accepted == dialog->exec())
    {
        QVariant data;
        QMetaObject::invokeMethod(obj, "value", Q_RETURN_ARG(QVariant, data));
        setValue(data);

        QString text;
        QMetaObject::invokeMethod(obj, "valueText", Q_RETURN_ARG(QString, text));
        setValueText(text);

        emit valueChanged(data);
        emit valueTextChanged(text);
    }

    dialog->deleteLater();
}

QString QtDialogWidget::key() const
{
    return m_key;
}

void QtDialogWidget::setKey(const QString &key)
{
    m_key = key;
}

bool QtDialogWidget::readOnly() const
{
    return m_readOnly;
}

void QtDialogWidget::setClassName(const QString &className)
{
    m_className = className;
    if (m_className.isEmpty())
        m_className = QStringLiteral("QDialog");
}

void QtDialogWidget::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
    m_button->setEnabled(!readOnly);
}

bool QtDialogWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_button)
    {
        switch (ev->type())
        {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key())
            {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

QVariant QtDialogWidget::value() const
{
    return m_value;
}

QString QtDialogWidget::valueText() const
{
    return m_valueText;
}

QString QtDialogWidget::className() const
{
    return m_className;
}

void QtDialogWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    opt.init(this);
#else
    opt.initFrom(this);
#endif
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


// QtColorEditWidget

class QtColorEditWidget : public QWidget {
    Q_OBJECT

public:
    explicit QtColorEditWidget(QWidget *parent);

    bool eventFilter(QObject *obj, QEvent *ev);

public Q_SLOTS:
    void setValue(const QColor &value);

Q_SIGNALS:
    void valueChanged(const QColor &value);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void buttonClicked();

private:
    QColor m_color;
    QLabel *m_pixmapLabel;
    QLabel *m_label;
    QToolButton *m_button;
};

QtColorEditWidget::QtColorEditWidget(QWidget *parent) :
    QWidget(parent),
    m_pixmapLabel(new QLabel),
    m_label(new QLabel),
    m_button(new QToolButton)
{
    QHBoxLayout *lt = new QHBoxLayout(this);
    setupTreeViewEditorMargin(lt);
    lt->setSpacing(0);
    lt->addWidget(m_pixmapLabel);
    lt->addWidget(m_label);
    lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    m_button->setFixedWidth(20);
    setFocusProxy(m_button);
    setFocusPolicy(m_button->focusPolicy());
    m_button->setText(tr("..."));
    m_button->installEventFilter(this);
    connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    lt->addWidget(m_button);
    m_pixmapLabel->setPixmap(QtPropertyBrowserUtils::brushValuePixmap(QBrush(m_color)));
    m_label->setText(QtPropertyBrowserUtils::colorValueText(m_color));
}

void QtColorEditWidget::setValue(const QColor &c)
{
    if (m_color != c) {
        m_color = c;
        m_pixmapLabel->setPixmap(QtPropertyBrowserUtils::brushValuePixmap(QBrush(c)));
        m_label->setText(QtPropertyBrowserUtils::colorValueText(c));
    }
}

void QtColorEditWidget::buttonClicked()
{
    bool ok = false;
    QRgb oldRgba = m_color.rgba();
    QRgb newRgba = QColorDialog::getRgba(oldRgba, &ok, this);
    if (ok && newRgba != oldRgba) {
        setValue(QColor::fromRgba(newRgba));
        emit valueChanged(m_color);
    }
}

bool QtColorEditWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_button) {
        switch (ev->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void QtColorEditWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// QtColorEditorFactoryPrivate

class QtColorEditorFactoryPrivate : public EditorFactoryPrivate<QtColorEditWidget>
{
    QtColorEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtColorEditorFactory)
public:
    QtColorEditorFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QColor &value);
    void slotSetValue(const QColor &value);
};

void QtColorEditorFactoryPrivate::slotPropertyChanged(QtProperty *property,
                const QColor &value)
{
    const PropertyToEditorListMap::iterator it = m_createdEditors.find(property);
    if (it == m_createdEditors.end())
        return;
    QListIterator<QtColorEditWidget *> itEditor(it.value());

    while (itEditor.hasNext())
        itEditor.next()->setValue(value);
}

void QtColorEditorFactoryPrivate::slotSetValue(const QColor &value)
{
    QObject *object = q_ptr->sender();
    const EditorToPropertyMap::ConstIterator ecend = m_editorToProperty.constEnd();
    for (EditorToPropertyMap::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtColorPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtColorEditorFactory

    \brief The QtColorEditorFactory class provides color editing  for
    properties created by QtColorPropertyManager objects.

    \sa QtAbstractEditorFactory, QtColorPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtColorEditorFactory::QtColorEditorFactory(QObject *parent) :
    QtAbstractEditorFactory<QtColorPropertyManager>(parent),
    d_ptr(new QtColorEditorFactoryPrivate())
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtColorEditorFactory::~QtColorEditorFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtColorEditorFactory::connectPropertyManager(QtColorPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*,QColor)),
            this, SLOT(slotPropertyChanged(QtProperty*,QColor)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtColorEditorFactory::createEditor(QtColorPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QtColorEditWidget *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));
    connect(editor, SIGNAL(valueChanged(QColor)), this, SLOT(slotSetValue(QColor)));
    connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtColorEditorFactory::disconnectPropertyManager(QtColorPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(slotPropertyChanged(QtProperty*,QColor)));
}

// QtFontEditWidget

class QtFontEditWidget : public QWidget {
    Q_OBJECT

public:
    explicit QtFontEditWidget(QWidget *parent);

    bool eventFilter(QObject *obj, QEvent *ev);

public Q_SLOTS:
    void setValue(const QFont &value);

Q_SIGNALS:
    void valueChanged(const QFont &value);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void buttonClicked();

private:
    QFont m_font;
    QLabel *m_pixmapLabel;
    QLabel *m_label;
    QToolButton *m_button;
};

QtFontEditWidget::QtFontEditWidget(QWidget *parent) :
    QWidget(parent),
    m_pixmapLabel(new QLabel),
    m_label(new QLabel),
    m_button(new QToolButton)
{
    QHBoxLayout *lt = new QHBoxLayout(this);
    setupTreeViewEditorMargin(lt);
    lt->setSpacing(0);
    lt->addWidget(m_pixmapLabel);
    lt->addWidget(m_label);
    lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    m_button->setFixedWidth(20);
    setFocusProxy(m_button);
    setFocusPolicy(m_button->focusPolicy());
    m_button->setText(tr("..."));
    m_button->installEventFilter(this);
    connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    lt->addWidget(m_button);
    m_pixmapLabel->setPixmap(QtPropertyBrowserUtils::fontValuePixmap(m_font));
    m_label->setText(QtPropertyBrowserUtils::fontValueText(m_font));
}

void QtFontEditWidget::setValue(const QFont &f)
{
    if (m_font != f) {
        m_font = f;
        m_pixmapLabel->setPixmap(QtPropertyBrowserUtils::fontValuePixmap(f));
        m_label->setText(QtPropertyBrowserUtils::fontValueText(f));
    }
}

void QtFontEditWidget::buttonClicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, m_font, this, tr("Select Font"));
    if (ok && newFont != m_font) {
        QFont f = m_font;
        // prevent mask for unchanged attributes, don't change other attributes (like kerning, etc...)
        if (m_font.family() != newFont.family())
            f.setFamily(newFont.family());
        if (m_font.pointSize() != newFont.pointSize())
            f.setPointSize(newFont.pointSize());
        if (m_font.bold() != newFont.bold())
            f.setBold(newFont.bold());
        if (m_font.italic() != newFont.italic())
            f.setItalic(newFont.italic());
        if (m_font.underline() != newFont.underline())
            f.setUnderline(newFont.underline());
        if (m_font.strikeOut() != newFont.strikeOut())
            f.setStrikeOut(newFont.strikeOut());
        setValue(f);
        emit valueChanged(m_font);
    }
}

bool QtFontEditWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_button) {
        switch (ev->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void QtFontEditWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// QtFontEditorFactoryPrivate

class QtFontEditorFactoryPrivate : public EditorFactoryPrivate<QtFontEditWidget>
{
    QtFontEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtFontEditorFactory)
public:
    QtFontEditorFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QFont &value);
    void slotSetValue(const QFont &value);
};

void QtFontEditorFactoryPrivate::slotPropertyChanged(QtProperty *property,
                const QFont &value)
{
    const PropertyToEditorListMap::iterator it = m_createdEditors.find(property);
    if (it == m_createdEditors.end())
        return;
    QListIterator<QtFontEditWidget *> itEditor(it.value());

    while (itEditor.hasNext())
        itEditor.next()->setValue(value);
}

void QtFontEditorFactoryPrivate::slotSetValue(const QFont &value)
{
    QObject *object = q_ptr->sender();
    const EditorToPropertyMap::ConstIterator ecend = m_editorToProperty.constEnd();
    for (EditorToPropertyMap::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtFontPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/*!
    \class QtFontEditorFactory

    \brief The QtFontEditorFactory class provides font editing for
    properties created by QtFontPropertyManager objects.

    \sa QtAbstractEditorFactory, QtFontPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtFontEditorFactory::QtFontEditorFactory(QObject *parent) :
    QtAbstractEditorFactory<QtFontPropertyManager>(parent),
    d_ptr(new QtFontEditorFactoryPrivate())
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtFontEditorFactory::~QtFontEditorFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtFontEditorFactory::connectPropertyManager(QtFontPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*,QFont)),
            this, SLOT(slotPropertyChanged(QtProperty*,QFont)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtFontEditorFactory::createEditor(QtFontPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QtFontEditWidget *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));
    connect(editor, SIGNAL(valueChanged(QFont)), this, SLOT(slotSetValue(QFont)));
    connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtFontEditorFactory::disconnectPropertyManager(QtFontPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty*,QFont)), this, SLOT(slotPropertyChanged(QtProperty*,QFont)));
}


class QtFileEditFactoryPrivate : public EditorFactoryPrivate<QtFileEditWidget>
{
    QtFileEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtFileEditFactory)
public:
    QtFileEditFactoryPrivate(){q_ptr = NULL;}
    void slotPropertyChanged(QtProperty *property, const QString &value);
    void slotFiltersChanged(QtProperty *property, const QStringList &filters);
    void slotSetValue(const QString &value);
};

void QtFileEditFactoryPrivate::slotPropertyChanged(QtProperty *property,
                const QString &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtFileEditWidget *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtFileEditWidget *editor = itEditor.next();
        if (editor->value() != value) {
            editor->blockSignals(true);
            editor->setValue(value);
            editor->blockSignals(false);
        }
    }
}

void QtFileEditFactoryPrivate::slotFiltersChanged(QtProperty *property, const QStringList &filters)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtFileEditWidget *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtFileEditWidget *editor = itEditor.next();
        if(editor->filters() != filters)
        {
            editor->blockSignals(true);
            editor->setFilters(filters);
            editor->blockSignals(false);
        }
    }
}

void QtFileEditFactoryPrivate::slotSetValue(const QString &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QtFileEditWidget *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtFileEditWidget *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtFilePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
    }
}


QtFileEditFactory::QtFileEditFactory(QObject *parent)
    : QtAbstractEditorFactory<QtFilePropertyManager>(parent)
{
    d_ptr = new QtFileEditFactoryPrivate();
    d_ptr->q_ptr = this;
}

QtFileEditFactory::~QtFileEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

void QtFileEditFactory::connectPropertyManager(QtFilePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
            this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    connect(manager, SIGNAL(filtersChanged(QtProperty*, const QStringList &)),
            this, SLOT(slotFiltersChanged(QtProperty*, const QStringList &)));
}

QWidget *QtFileEditFactory::createEditor(QtFilePropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QtFileEditWidget *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));
    editor->setFilters(manager->filters(property));

    connect(editor, SIGNAL(valueChanged(const QString &)),
                this, SLOT(slotSetValue(const QString &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void QtFileEditFactory::disconnectPropertyManager(QtFilePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
               this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    disconnect(manager, SIGNAL(filtersChanged(QtProperty*, const QStringList &)),
               this, SLOT(slotFiltersChanged(QtProperty*, const QStringList &)));
}


//-------------------------add by luox-------------------------------
// QtTextEditFactory
#include "qthistoricalrecordtextedit.h"
class QtTextEditFactoryPrivate : public EditorFactoryPrivate<QHistoricalRecordTextEdit>
{
    QtTextEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtTextEditFactory)
public:
    QtTextEditFactoryPrivate()
    {
        q_ptr = NULL;
    }
    void setAllFuncName(const QList<QString> &funcName);
    QList<QString> m_allFunctionNameList;
    void setFuncNameInKeyWord(QtProperty *property);
    void slotPropertyChanged(QtProperty *property, const QString &value);
    void slotRegExpChanged(QtProperty *property, const QRegExp &regExp);
    void slotSetValue();
    void slotEditingFinished();
};

void QtTextEditFactoryPrivate::setAllFuncName(const QList<QString> &funcName)
{
    m_allFunctionNameList = funcName;
}

void QtTextEditFactoryPrivate::setFuncNameInKeyWord(QtProperty *property)
{
    if (!m_createdEditors.contains(property))
        return;
    QList<QString> t_funcName;
    t_funcName = q_ptr->getAllFuncName();
    QListIterator<QHistoricalRecordTextEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QHistoricalRecordTextEdit *editor = itEditor.next();
        editor->setAllFuncName(t_funcName);
    }
}

void QtTextEditFactoryPrivate::slotPropertyChanged(QtProperty *property, const QString &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QList<QString> t_funcName;
    t_funcName = q_ptr->getAllFuncName();
    QListIterator<QHistoricalRecordTextEdit *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QHistoricalRecordTextEdit *editor = itEditor.next();
//        editor->setAllFuncName(t_funcName);
        if (editor->toPlainText() != value) {
            editor->blockSignals(true);
            editor->setText(value);
            editor->blockSignals(false);
        }
    }
}

void QtTextEditFactoryPrivate::slotRegExpChanged(QtProperty *property,const QRegExp &regExp)
{
    if (!m_createdEditors.contains(property))
        return;

    CustomStringTextPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

//    QListIterator<QTextEdit *> itEditor(m_createdEditors[property]);
//    while (itEditor.hasNext()) {
//        QTextEdit *editor = itEditor.next();
//        editor->blockSignals(true);
//        const QValidator *oldValidator = editor->validator();
//        QValidator *newValidator = 0;
//        if (regExp.isValid()) {
//            newValidator = new QRegExpValidator(regExp, editor);
//        }
//        editor->setValidator(newValidator);
//        if (oldValidator)
//            delete oldValidator;
//        editor->blockSignals(false);
//    }
}

void QtTextEditFactoryPrivate::slotSetValue()
{
    QObject *object = q_ptr->sender();
    const QMap<QHistoricalRecordTextEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QHistoricalRecordTextEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();

            //add by luox
			QHistoricalRecordTextEdit *textEdit = itEditor.key();
            QString value = textEdit->toPlainText();
            //-

            CustomStringTextPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

void QtTextEditFactoryPrivate::slotEditingFinished()
{
    QObject *object = q_ptr->sender();
    const QMap<QHistoricalRecordTextEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QHistoricalRecordTextEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            CustomStringTextPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            return;
        }
    }
}


QtTextEditFactory::QtTextEditFactory(QObject *parent)
                  :QtAbstractEditorFactory<CustomStringTextPropertyManager>(parent)
{
    d_ptr = new QtTextEditFactoryPrivate();
    d_ptr->q_ptr = this;
}

QtTextEditFactory::~QtTextEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

void QtTextEditFactory::setAllFuncName(const QList<QString> &funcName)
{
    m_allFunctionNameList = funcName;
    d_ptr->setAllFuncName(m_allFunctionNameList);
}

QList<QString> QtTextEditFactory::getAllFuncName()
{
    return m_allFunctionNameList;
}

void QtTextEditFactory::connectPropertyManager(CustomStringTextPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
            this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    connect(manager, SIGNAL(regExpChanged(QtProperty *, const QRegExp &)),
            this, SLOT(slotRegExpChanged(QtProperty *, const QRegExp &)));
}

QWidget *QtTextEditFactory::createEditor(CustomStringTextPropertyManager *manager, QtProperty *property, QWidget *parent)
{
    qDebug()<<Q_FUNC_INFO;
    QTextEdit *editor = d_ptr->createEditor(property, parent);
    d_ptr->setFuncNameInKeyWord(property);

    //设置正则表达式
//    QRegExp regExp = manager->regExp(property);
//    if (regExp.isValid()) {
//        QValidator *validator = new QRegExpValidator(regExp, editor);
//        editor->setValidator(validator);
//    }
    editor->setText(manager->value(property));

    connect(editor, SIGNAL(textChanged()),
                this, SLOT(slotSetValue()));
    connect(editor, SIGNAL(destroyed(QObject*)),
            this,SLOT(slotEditingFinished()));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void QtTextEditFactory::disconnectPropertyManager(CustomStringTextPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    disconnect(manager, SIGNAL(regExpChanged(QtProperty *, const QRegExp &)),
                this, SLOT(slotRegExpChanged(QtProperty *, const QRegExp &)));
}

class QtDialogFactoryPrivate : public EditorFactoryPrivate<QtDialogWidget>
{
    QtDialogFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtDialogFactory)
public:
    void slotPropertyChanged(QtProperty *property, const QVariant &value);
    void slotClassNameChanged(QtProperty *property, const QString &className);
    void slotSetValue(const QVariant &value);
    void slotSetValueText(const QString &valueTxt);
    void slotReadOnlyChanged(QtProperty *property, bool readOnly);
};

void QtDialogFactoryPrivate::slotPropertyChanged(QtProperty *property, const QVariant &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtDialogWidget *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtDialogWidget *editor = itEditor.next();
        if (editor->value() != value) {
            editor->blockSignals(true);
            editor->setValue(value);
            editor->blockSignals(false);
        }
    }
}

void QtDialogFactoryPrivate::slotClassNameChanged(QtProperty *property, const QString &className)
{
    const PropertyToEditorListMap::const_iterator it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.constEnd())
        return;

    for (QtDialogWidget *editor : it.value()) {
        if (editor->className() != className) {
            editor->blockSignals(true);
            editor->setClassName(className);
            editor->blockSignals(false);
        }
    }
}

void QtDialogFactoryPrivate::slotSetValue(const QVariant &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QtDialogWidget *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtDialogWidget *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtDialogPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager) {
                return;
            }
            manager->setValue(property, value);
            return;
        }
    }
}

void QtDialogFactoryPrivate::slotSetValueText(const QString &valueTxt)
{
    QObject *object = q_ptr->sender();
    const QMap<QtDialogWidget *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtDialogWidget *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtDialogPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager) {
                return;
            }
            manager->setValueText(property, valueTxt);
            return;
        }
    }
}

void QtDialogFactoryPrivate::slotReadOnlyChanged(QtProperty *property, bool readOnly)
{
    const PropertyToEditorListMap::const_iterator it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.constEnd())
        return;

    for (QtDialogWidget *editor : it.value()) {
        if (editor->readOnly() != readOnly) {
            editor->blockSignals(true);
            editor->setReadOnly(readOnly);
            editor->blockSignals(false);
        }
    }
}


QtDialogFactory::QtDialogFactory(QObject *parent)
    : QtAbstractEditorFactory<QtDialogPropertyManager>(parent)
{
    d_ptr = new QtDialogFactoryPrivate();
    d_ptr->q_ptr = this;
}

QtDialogFactory::~QtDialogFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

void QtDialogFactory::connectPropertyManager(QtDialogPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
        this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
    connect(manager, SIGNAL(classNameChanged(QtProperty*, QString)),
        this, SLOT(slotClassNameChanged(QtProperty*, QString)));
    connect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

QWidget *QtDialogFactory::createEditor(QtDialogPropertyManager *manager,
    QtProperty *property, QWidget *parent)
{

    QtDialogWidget *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));
    editor->setClassName(manager->className(property));
    editor->setReadOnly(manager->isReadOnly(property));
    editor->setValueText(manager->valueString(property));
    editor->setWindowTitle(property->propertyName());

    connect(editor, SIGNAL(valueChanged(const QVariant &)),
        this, SLOT(slotSetValue(const QVariant &)));
    connect(editor, SIGNAL(valueTextChanged(const QString &)),
        this, SLOT(slotSetValueText(const QString &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
        this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void QtDialogFactory::disconnectPropertyManager(QtDialogPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
        this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
    disconnect(manager, SIGNAL(classNameChanged(QtProperty*, QString)),
        this, SLOT(slotClassNameChanged(QtProperty*, QString)));
    disconnect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

class QtRangeFactoryPrivate : public EditorFactoryPrivate<QtRangeEditWidget>
{
    QtRangeFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtRangeFactory)
public:
    void slotPropertyChanged(QtProperty *property, const Range &value);
    void slotSetValue(const Range &value);
    void slotReadOnlyChanged(QtProperty *property, bool readOnly);
};

void QtRangeFactoryPrivate::slotPropertyChanged(QtProperty *property, const Range &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtRangeEditWidget *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtRangeEditWidget *editor = itEditor.next();
        if (editor->value() != value) {
            editor->blockSignals(true);
            editor->setValue(value);
            editor->blockSignals(false);
        }
    }
}

void QtRangeFactoryPrivate::slotSetValue(const Range &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QtRangeEditWidget *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtRangeEditWidget *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtRangePropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager) {
                return;
            }
            manager->setValue(property, value);
            return;
        }
    }
}

void QtRangeFactoryPrivate::slotReadOnlyChanged(QtProperty *property, bool readOnly)
{
    const PropertyToEditorListMap::const_iterator it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.constEnd())
        return;

    for (QtRangeEditWidget *editor : it.value()) {
        if (editor->readOnly() != readOnly) {
            editor->blockSignals(true);
            editor->setReadOnly(readOnly);
            editor->blockSignals(false);
        }
    }
}


QtRangeFactory::QtRangeFactory(QObject *parent)
    : QtAbstractEditorFactory<QtRangePropertyManager>(parent)
{
    d_ptr = new QtRangeFactoryPrivate();
    d_ptr->q_ptr = this;
}

QtRangeFactory::~QtRangeFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

void QtRangeFactory::connectPropertyManager(QtRangePropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const Range &)),
        this, SLOT(slotPropertyChanged(QtProperty *, const Range &)));
    connect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

QWidget *QtRangeFactory::createEditor(QtRangePropertyManager *manager,
    QtProperty *property, QWidget *parent)
{

    QtRangeEditWidget *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));
    editor->setReadOnly(manager->isReadOnly(property));

    connect(editor, SIGNAL(valueChanged(const Range &)),
        this, SLOT(slotSetValue(const Range &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
        this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void QtRangeFactory::disconnectPropertyManager(QtRangePropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const Range &)),
        this, SLOT(slotPropertyChanged(QtProperty *, const Range &)));
    disconnect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

class QtRangeFFactoryPrivate : public EditorFactoryPrivate<QtRangeFEditWidget>
{
    QtRangeFFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtRangeFFactory)
public:
    void slotPropertyChanged(QtProperty *property, const RangeF &value);
    void slotSetValue(const RangeF &value);
    void slotReadOnlyChanged(QtProperty *property, bool readOnly);
};

void QtRangeFFactoryPrivate::slotPropertyChanged(QtProperty *property, const RangeF &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QtRangeFEditWidget *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QtRangeFEditWidget *editor = itEditor.next();
        if (editor->value() != value) {
            editor->blockSignals(true);
            editor->setValue(value);
            editor->blockSignals(false);
        }
    }
}

void QtRangeFFactoryPrivate::slotSetValue(const RangeF &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QtRangeFEditWidget *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QtRangeFEditWidget *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin();
         itEditor != ecend; ++itEditor)
    {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtRangeFPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager) {
                return;
            }
            manager->setValue(property, value);
            return;
        }
    }
}

void QtRangeFFactoryPrivate::slotReadOnlyChanged(QtProperty *property, bool readOnly)
{
    const PropertyToEditorListMap::const_iterator it = m_createdEditors.constFind(property);
    if (it == m_createdEditors.constEnd())
        return;

    for (QtRangeFEditWidget *editor : it.value()) {
        if (editor->readOnly() != readOnly) {
            editor->blockSignals(true);
            editor->setReadOnly(readOnly);
            editor->blockSignals(false);
        }
    }
}


QtRangeFFactory::QtRangeFFactory(QObject *parent)
    : QtAbstractEditorFactory<QtRangeFPropertyManager>(parent)
{
    d_ptr = new QtRangeFFactoryPrivate();
    d_ptr->q_ptr = this;
}

QtRangeFFactory::~QtRangeFFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

void QtRangeFFactory::connectPropertyManager(QtRangeFPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const RangeF &)),
        this, SLOT(slotPropertyChanged(QtProperty *, const RangeF &)));
    connect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

QWidget *QtRangeFFactory::createEditor(QtRangeFPropertyManager *manager,
    QtProperty *property, QWidget *parent)
{

    QtRangeFEditWidget *editor = d_ptr->createEditor(property, parent);
    editor->setValue(manager->value(property));
    editor->setReadOnly(manager->readOnly(property));

    connect(editor, SIGNAL(valueChanged(const Range &)),
        this, SLOT(slotSetValue(const Range &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
        this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void QtRangeFFactory::disconnectPropertyManager(QtRangeFPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const RangeF &)),
        this, SLOT(slotPropertyChanged(QtProperty *, const RangeF &)));
    disconnect(manager, SIGNAL(readOnlyChanged(QtProperty *, bool)),
        this, SLOT(slotReadOnlyChanged(QtProperty *, bool)));
}

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#include "moc_qteditorfactory.cpp"
#include "qteditorfactory.moc"
