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


#include "qtpropertybrowserutils_p.h"
#include <QGuiApplication>
#include <QPainter>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QCheckBox>
#include <QLineEdit>
#include <QMenu>
#include <QStyleOption>
#include <QTimer>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QListView>
#include <QMouseEvent>
#include <QStyleHints>
#include <QApplication>
#include <QTimer>

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

QtCursorDatabase::QtCursorDatabase()
{
    appendCursor(Qt::ArrowCursor, QCoreApplication::translate("QtCursorDatabase", "Arrow"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-arrow.png")));
    appendCursor(Qt::UpArrowCursor, QCoreApplication::translate("QtCursorDatabase", "Up Arrow"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-uparrow.png")));
    appendCursor(Qt::CrossCursor, QCoreApplication::translate("QtCursorDatabase", "Cross"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-cross.png")));
    appendCursor(Qt::WaitCursor, QCoreApplication::translate("QtCursorDatabase", "Wait"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-wait.png")));
    appendCursor(Qt::IBeamCursor, QCoreApplication::translate("QtCursorDatabase", "IBeam"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-ibeam.png")));
    appendCursor(Qt::SizeVerCursor, QCoreApplication::translate("QtCursorDatabase", "Size Vertical"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizev.png")));
    appendCursor(Qt::SizeHorCursor, QCoreApplication::translate("QtCursorDatabase", "Size Horizontal"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizeh.png")));
    appendCursor(Qt::SizeFDiagCursor, QCoreApplication::translate("QtCursorDatabase", "Size Backslash"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizef.png")));
    appendCursor(Qt::SizeBDiagCursor, QCoreApplication::translate("QtCursorDatabase", "Size Slash"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizeb.png")));
    appendCursor(Qt::SizeAllCursor, QCoreApplication::translate("QtCursorDatabase", "Size All"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizeall.png")));
    appendCursor(Qt::BlankCursor, QCoreApplication::translate("QtCursorDatabase", "Blank"),
                 QIcon());
    appendCursor(Qt::SplitVCursor, QCoreApplication::translate("QtCursorDatabase", "Split Vertical"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-vsplit.png")));
    appendCursor(Qt::SplitHCursor, QCoreApplication::translate("QtCursorDatabase", "Split Horizontal"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-hsplit.png")));
    appendCursor(Qt::PointingHandCursor, QCoreApplication::translate("QtCursorDatabase", "Pointing Hand"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-hand.png")));
    appendCursor(Qt::ForbiddenCursor, QCoreApplication::translate("QtCursorDatabase", "Forbidden"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-forbidden.png")));
    appendCursor(Qt::OpenHandCursor, QCoreApplication::translate("QtCursorDatabase", "Open Hand"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-openhand.png")));
    appendCursor(Qt::ClosedHandCursor, QCoreApplication::translate("QtCursorDatabase", "Closed Hand"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-closedhand.png")));
    appendCursor(Qt::WhatsThisCursor, QCoreApplication::translate("QtCursorDatabase", "What's This"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-whatsthis.png")));
    appendCursor(Qt::BusyCursor, QCoreApplication::translate("QtCursorDatabase", "Busy"),
                 QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-busy.png")));
}

void QtCursorDatabase::clear()
{
    m_cursorNames.clear();
    m_cursorIcons.clear();
    m_valueToCursorShape.clear();
    m_cursorShapeToValue.clear();
}

void QtCursorDatabase::appendCursor(Qt::CursorShape shape, const QString &name, const QIcon &icon)
{
    if (m_cursorShapeToValue.contains(shape))
        return;
    const int value = m_cursorNames.count();
    m_cursorNames.append(name);
    m_cursorIcons.insert(value, icon);
    m_valueToCursorShape.insert(value, shape);
    m_cursorShapeToValue.insert(shape, value);
}

QStringList QtCursorDatabase::cursorShapeNames() const
{
    return m_cursorNames;
}

QMap<int, QIcon> QtCursorDatabase::cursorShapeIcons() const
{
    return m_cursorIcons;
}

QString QtCursorDatabase::cursorToShapeName(const QCursor &cursor) const
{
    int val = cursorToValue(cursor);
    if (val >= 0)
        return m_cursorNames.at(val);
    return QString();
}

QIcon QtCursorDatabase::cursorToShapeIcon(const QCursor &cursor) const
{
    int val = cursorToValue(cursor);
    return m_cursorIcons.value(val);
}

int QtCursorDatabase::cursorToValue(const QCursor &cursor) const
{
    Qt::CursorShape shape = cursor.shape();
    if (m_cursorShapeToValue.contains(shape))
        return m_cursorShapeToValue[shape];
    return -1;
}

QCursor QtCursorDatabase::valueToCursor(int value) const
{
    if (m_valueToCursorShape.contains(value))
        return QCursor(m_valueToCursorShape[value]);
    return QCursor();
}

QPixmap QtPropertyBrowserUtils::brushValuePixmap(const QBrush &b)
{
    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);

    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(0, 0, img.width(), img.height(), b);
    QColor color = b.color();
    if (color.alpha() != 255) { // indicate alpha by an inset
        QBrush  opaqueBrush = b;
        color.setAlpha(255);
        opaqueBrush.setColor(color);
        painter.fillRect(img.width() / 4, img.height() / 4,
                         img.width() / 2, img.height() / 2, opaqueBrush);
    }
    painter.end();
    return QPixmap::fromImage(img);
}

QIcon QtPropertyBrowserUtils::brushValueIcon(const QBrush &b)
{
    return QIcon(brushValuePixmap(b));
}

QString QtPropertyBrowserUtils::colorValueText(const QColor &c)
{
    return QCoreApplication::translate("QtPropertyBrowserUtils", "[%1, %2, %3] (%4)")
           .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
}

QPixmap QtPropertyBrowserUtils::fontValuePixmap(const QFont &font)
{
    QFont f = font;
    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    QPainter p(&img);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setRenderHint(QPainter::Antialiasing, true);
    f.setPointSize(13);
    p.setFont(f);
    QTextOption t;
    t.setAlignment(Qt::AlignCenter);
    p.drawText(QRect(0, 0, 16, 16), QString(QLatin1Char('A')), t);
    return QPixmap::fromImage(img);
}

QIcon QtPropertyBrowserUtils::fontValueIcon(const QFont &f)
{
    return QIcon(fontValuePixmap(f));
}

QString QtPropertyBrowserUtils::fontValueText(const QFont &f)
{
    return QCoreApplication::translate("QtPropertyBrowserUtils", "[%1, %2]")
           .arg(f.family()).arg(f.pointSize());
}


QtBoolEdit::QtBoolEdit(QWidget *parent) :
    QWidget(parent),
    m_checkBox(new QCheckBox(this)),
    m_textVisible(true)
{
    QHBoxLayout *lt = new QHBoxLayout;
    if (QApplication::layoutDirection() == Qt::LeftToRight)
        lt->setContentsMargins(4, 0, 0, 0);
    else
        lt->setContentsMargins(0, 0, 4, 0);
    lt->addWidget(m_checkBox);
    setLayout(lt);
    connect(m_checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    setFocusProxy(m_checkBox);
    m_checkBox->setText(tr("True"));
}

void QtBoolEdit::setTextVisible(bool textVisible)
{
    if (m_textVisible == textVisible)
        return;

    m_textVisible = textVisible;
    if (m_textVisible)
        m_checkBox->setText(isChecked() ? tr("True") : tr("False"));
    else
        m_checkBox->setText(QString());
}

Qt::CheckState QtBoolEdit::checkState() const
{
    return m_checkBox->checkState();
}

void QtBoolEdit::setCheckState(Qt::CheckState state)
{
    m_checkBox->setCheckState(state);
}

bool QtBoolEdit::isChecked() const
{
    return m_checkBox->isChecked();
}

void QtBoolEdit::setChecked(bool c)
{
    m_checkBox->setChecked(c);
    if (!m_textVisible)
        return;
    m_checkBox->setText(isChecked() ? tr("True") : tr("False"));
}

bool QtBoolEdit::blockCheckBoxSignals(bool block)
{
    return m_checkBox->blockSignals(block);
}

void QtBoolEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        m_checkBox->click();
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void QtBoolEdit::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}



QtKeySequenceEdit::QtKeySequenceEdit(QWidget *parent)
    : QWidget(parent), m_num(0), m_lineEdit(new QLineEdit(this))
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

bool QtKeySequenceEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_lineEdit && e->type() == QEvent::ContextMenu) {
        QContextMenuEvent *c = static_cast<QContextMenuEvent *>(e);
        QMenu *menu = m_lineEdit->createStandardContextMenu();
        const QList<QAction *> actions = menu->actions();
        QListIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            action->setShortcut(QKeySequence());
            QString actionString = action->text();
            const int pos = actionString.lastIndexOf(QLatin1Char('\t'));
            if (pos > 0)
                actionString.remove(pos, actionString.length() - pos);
            action->setText(actionString);
        }
        QAction *actionBefore = 0;
        if (actions.count() > 0)
            actionBefore = actions[0];
        QAction *clearAction = new QAction(tr("Clear Shortcut"), menu);
        menu->insertAction(actionBefore, clearAction);
        menu->insertSeparator(actionBefore);
        clearAction->setEnabled(!m_keySequence.isEmpty());
        connect(clearAction, SIGNAL(triggered()), this, SLOT(slotClearShortcut()));
        menu->exec(c->globalPos());
        delete menu;
        e->accept();
        return true;
    }

    return QWidget::eventFilter(o, e);
}

void QtKeySequenceEdit::slotClearShortcut()
{
    if (m_keySequence.isEmpty())
        return;
    setKeySequence(QKeySequence());
    emit keySequenceChanged(m_keySequence);
}

void QtKeySequenceEdit::handleKeyEvent(QKeyEvent *e)
{
    int nextKey = e->key();
    if (nextKey == Qt::Key_Control || nextKey == Qt::Key_Shift ||
            nextKey == Qt::Key_Meta || nextKey == Qt::Key_Alt ||
            nextKey == Qt::Key_Super_L || nextKey == Qt::Key_AltGr)
        return;

    nextKey |= translateModifiers(e->modifiers(), e->text());
    int k0 = m_keySequence[0];
    int k1 = m_keySequence[1];
    int k2 = m_keySequence[2];
    int k3 = m_keySequence[3];
    switch (m_num) {
        case 0: k0 = nextKey; k1 = 0; k2 = 0; k3 = 0; break;
        case 1: k1 = nextKey; k2 = 0; k3 = 0; break;
        case 2: k2 = nextKey; k3 = 0; break;
        case 3: k3 = nextKey; break;
        default: break;
    }
    ++m_num;
    if (m_num > 3)
        m_num = 0;
    m_keySequence = QKeySequence(k0, k1, k2, k3);
    m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
    e->accept();
    emit keySequenceChanged(m_keySequence);
}

void QtKeySequenceEdit::setKeySequence(const QKeySequence &sequence)
{
    if (sequence == m_keySequence)
        return;
    m_num = 0;
    m_keySequence = sequence;
    m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
}

QKeySequence QtKeySequenceEdit::keySequence() const
{
    return m_keySequence;
}

int QtKeySequenceEdit::translateModifiers(Qt::KeyboardModifiers state, const QString &text) const
{
    int result = 0;
    if ((state & Qt::ShiftModifier) && (text.size() == 0 || !text.at(0).isPrint() || text.at(0).isLetter() || text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}

void QtKeySequenceEdit::focusInEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    m_lineEdit->selectAll();
    QWidget::focusInEvent(e);
}

void QtKeySequenceEdit::focusOutEvent(QFocusEvent *e)
{
    m_num = 0;
    m_lineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void QtKeySequenceEdit::keyPressEvent(QKeyEvent *e)
{
    handleKeyEvent(e);
    e->accept();
}

void QtKeySequenceEdit::keyReleaseEvent(QKeyEvent *e)
{
    m_lineEdit->event(e);
}

void QtKeySequenceEdit::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool QtKeySequenceEdit::event(QEvent *e)
{
    if (e->type() == QEvent::Shortcut ||
            e->type() == QEvent::ShortcutOverride  ||
            e->type() == QEvent::KeyRelease) {
        e->accept();
        return true;
    }
    return QWidget::event(e);
}

class CheckableComboBoxPrivate
{
    Q_DECLARE_PUBLIC(CheckableComboBox)
public:
    CheckableComboBoxPrivate();
    ~CheckableComboBoxPrivate();

    void init();
    void _q_itemSelected(const QModelIndex &index = QModelIndex());
    void showPopupFromMouseEvent(QMouseEvent *e);

    CheckableComboBox *q_ptr = nullptr;
    QMap<int, QString> selectedItems;
    bool isPopupShown = false;
    bool maybeIgnoreMouseButtonRelease = false;

    static const QString ItemSplitter;
};

const QString CheckableComboBoxPrivate::ItemSplitter = QStringLiteral(" | ");

CheckableComboBox::CheckableComboBox(QWidget *parent)
    : QComboBox(parent)
    , d_ptr(new CheckableComboBoxPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->init();
}

CheckableComboBox::~CheckableComboBox()
{
}

void CheckableComboBox::showPopup()
{
    Q_D(CheckableComboBox);
    if (d->isPopupShown) {
        return ;
    }

    QStringList items(lineEdit()->text().split(CheckableComboBoxPrivate::ItemSplitter, QString::SkipEmptyParts));
    for (int i=0; i<model()->rowCount(); ++i) {
        QCheckBox *checkBox = qobject_cast<QCheckBox*> (view()->indexWidget(model()->index(i, 0)));
        if (checkBox != nullptr) {
            checkBox->blockSignals(true);
            checkBox->setChecked(d->selectedItems.contains(i));
            checkBox->blockSignals(false);
        }
    }

    QComboBox::showPopup();
    d->isPopupShown = true;
}

void CheckableComboBox::hidePopup()
{
    Q_D(CheckableComboBox);
    QComboBox::hidePopup();
    QTimer::singleShot(10, [=](){ d->isPopupShown = false; });
}

void CheckableComboBox::addItem(const QString &text, const QVariant &userData)
{
    Q_D(CheckableComboBox);
    QComboBox::addItem(text, userData);
    QStandardItemModel *listModel = qobject_cast<QStandardItemModel*>(model());
    QStandardItem *item = listModel->item(model()->rowCount()-1);

    QCheckBox *checkBox = new QCheckBox(text);
    checkBox->setAutoFillBackground(true);
    checkBox->setStyleSheet(QStringLiteral("padding-left: 5px; background: %1;").arg(view()->palette().background().color().name()));

    qobject_cast<QListView*>(view())->setIndexWidget(model()->index(model()->rowCount() - 1, 0), checkBox);

    connect(checkBox, &QCheckBox::clicked, [=](bool checked){
        int row = -1;
        for (int i=0; i<model()->rowCount(); ++i) {
            if (item->text() == model()->data(model()->index(i, 0)).toString()) {
                row = i;
                break;
            }
        }

        if (row >= 0) {
            if (checked) {
                d->selectedItems.insert(row, item->text());
            } else {
                d->selectedItems.remove(row);
            }

            if (lineEdit() != nullptr) {
                lineEdit()->setText(d->selectedItems.values().join(CheckableComboBoxPrivate::ItemSplitter));
            }
            emit currentIndexesChanged(currentIndexes());
            emit currentTextsChanged(currentTexts());
            emit currentDatasChanged(currentDatas());
        }
    });
}

void CheckableComboBox::addItems(const QStringList &texts, const QVariantList &userDatas)
{
    for (int i=0; i<texts.size(); ++i) {
        addItem(texts.at(i), userDatas.value(i, QVariant()));
    }
}

void CheckableComboBox::setCurrentIndexes(const CheckableEnumValue &values)
{
    Q_D(CheckableComboBox);
    d->selectedItems.clear();
    for (int row : values) {
        d->selectedItems.insert(row, model()->data(model()->index(row, 0)).toString());
    }

    if (lineEdit() != nullptr) {
        lineEdit()->setText(d->selectedItems.values().join(CheckableComboBoxPrivate::ItemSplitter));
    }

    emit currentIndexesChanged(d->selectedItems.keys());
    emit currentTextsChanged(d->selectedItems.values());
    emit currentDatasChanged(currentDatas());
}

QStringList CheckableComboBox::currentTexts() const
{
    Q_D(const CheckableComboBox);
    return d->selectedItems.values();
}

QVariantList CheckableComboBox::currentDatas() const
{
    Q_D(const CheckableComboBox);
    QStandardItemModel *listModel = qobject_cast<QStandardItemModel*>(model());
    QVariantList datas;
    for (int row : d->selectedItems.keys()) {
        QStandardItem *item = listModel->item(row, 0);
        if (item != nullptr) {
            datas << item->data();
        }
    }
    return datas;
}

bool CheckableComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(CheckableComboBox);
    if (watched == lineEdit()) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            if (!QGuiApplication::styleHints()->setFocusOnTouchRelease() && hasFocus()) {
                d->showPopupFromMouseEvent(dynamic_cast<QMouseEvent*>(event));
                d->maybeIgnoreMouseButtonRelease = true;
                QTimer::singleShot(QApplication::doubleClickInterval(), [=](){ d->maybeIgnoreMouseButtonRelease = false; });
                return true;
            }
        } break;
        case QEvent::MouseButtonRelease: {
            if (QGuiApplication::styleHints()->setFocusOnTouchRelease() && hasFocus() && !d->maybeIgnoreMouseButtonRelease) {
                d->showPopupFromMouseEvent(dynamic_cast<QMouseEvent*>(event));
                return true;
            }
        } break;
        default: break;
        }
    }

    return QComboBox::eventFilter(watched, event);
}

void CheckableComboBoxPrivate::showPopupFromMouseEvent(QMouseEvent *e)
{
    Q_Q(CheckableComboBox);

    if (isPopupShown) {
        q->hidePopup();
        e->ignore();
        return;
    }

    if (e->button() == Qt::LeftButton && q->lineEdit()->isReadOnly()) {
        q->showPopup();
    } else {
        e->ignore();
    }
}


QList<int> CheckableComboBox::currentIndexes() const
{
    Q_D(const CheckableComboBox);
    return d->selectedItems.keys();
}

CheckableComboBoxPrivate::CheckableComboBoxPrivate()
{
}

CheckableComboBoxPrivate::~CheckableComboBoxPrivate()
{

}

void CheckableComboBoxPrivate::init()
{
    Q_Q(CheckableComboBox);
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setReadOnly(true);
    lineEdit->setFrame(false);
    lineEdit->setStyleSheet(QStringLiteral("border: 0px;"));
    lineEdit->installEventFilter(q);

    q->setLineEdit(lineEdit);
    q->setView(new QListView());

    QStandardItemModel *listModel = qobject_cast<QStandardItemModel*>(q->model());
    QObject::connect(listModel, &QStandardItemModel::itemChanged, [=](QStandardItem *item){
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(q->model());
        if (model == nullptr) {
            return ;
        }
        QModelIndex index = model->indexFromItem(item);
        if (index.isValid()) {
            QCheckBox *checkBox = qobject_cast<QCheckBox*> (q->view()->indexWidget(index));
            if (checkBox != nullptr) {
                checkBox->setText(item->text());
            }
        }
    });
}

void CheckableComboBoxPrivate::_q_itemSelected(const QModelIndex &index)
{
    Q_Q(CheckableComboBox);
    if (!index.isValid()) {
        return ;
    }

    if (selectedItems.contains(index.row())) {
        selectedItems.remove(index.row());
    } else {
        selectedItems.insert(index.row(), q->model()->data(index).toString());
    }

    QCheckBox *checkBox = qobject_cast<QCheckBox*> (q->view()->indexWidget(index));
    if (checkBox != nullptr) {
        checkBox->setChecked(selectedItems.contains(index.row()));
    }
    if (q->lineEdit() != nullptr) {
        q->lineEdit()->setText(selectedItems.values().join(CheckableComboBoxPrivate::ItemSplitter));
    }

    emit q->currentIndexesChanged(selectedItems.keys());
    emit q->currentTextsChanged(selectedItems.values());
    emit q->currentDatasChanged(q->currentDatas());
}

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#include "moc_qtpropertybrowserutils_p.cpp"

