#ifndef EDITORBASE_H
#define EDITORBASE_H

#include <QWidget>
#include <QVariantHash>
#include <QMessageBox>
#include "httpclient.h"

class EditorBase : public QWidget
{
    Q_OBJECT
public:
    enum EditorMode{
        New,
        Update,
        Delete
    };

public:
    explicit EditorBase(QWidget *parent = nullptr,  QVariantHash d = QVariantHash(),  EditorMode m = EditorMode::New);
    virtual ~EditorBase();

    bool compressDir(const QString& fileCompressed, const QString &dir);

    QStringList extractDir(const QString &fileCompressed, const QString &dir);

signals:
    void editorDataChanged();
    void editorDataUpdated(const QVariantHash &);
    void editorDataDelete(const QVariantHash &);

public slots:
    virtual void on_result(bool state, const QString& respons);

public:
    QVariantHash        m_data;
    EditorMode          m_mode;
    HttpClient          *m_httpClient;
};

#endif // EDITORBASE_H
