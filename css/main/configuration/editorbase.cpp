#include "editorbase.h"
#include <QDebug>
#include <QFile>
#include "quazip/JlCompress.h"
#include "configurationglobal.h"

EditorBase::EditorBase(QWidget *parent, QVariantHash d, EditorMode m)
    :
      QWidget{parent},
      m_data(d),
      m_mode(m),
      m_httpClient(NULL)
{
    m_httpClient = new HttpClient(ConfigGlobalInterface->serverAddr(), 8080);
    connect(m_httpClient, &HttpClient::emitData, this, &EditorBase::on_result);
}

EditorBase::~EditorBase()
{
    if (m_httpClient)
    {
        delete m_httpClient;
        m_httpClient = nullptr;
    }
}

QStringList EditorBase::extractDir(const QString &fileCompressed, const QString &dir)
{
    if (!QFile::exists(fileCompressed)){
        qDebug() << u8"待解压文件不存在!";
    }

    return JlCompress::extractDir(fileCompressed, dir);
}

bool EditorBase::compressDir(const QString &fileCompressed, const QString &dir)
{
    if (fileCompressed.isEmpty()){
        qDebug() << u8"目标文件无效, 请重新设置!";
    }

    return  JlCompress::compressDir(fileCompressed, dir, true);
}

void EditorBase::on_result(bool state, const QString &respons)
{
    qDebug() << "EditorBase" << state << respons;
}
