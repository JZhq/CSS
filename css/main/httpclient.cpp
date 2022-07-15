#include "httpclient.h"

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfoList>
#include <QHttpPart>



HttpClient::HttpClient(const QString &ip, int port, QObject *reciver, const char *slot, QObject *parent)
    : QObject(parent)
    , m_ip(ip)
    , m_port(port)
    , m_slot(slot)
    , m_manager(new QNetworkAccessManager)
{
    connect(m_manager.data(), &QNetworkAccessManager::finished, this, &HttpClient::replyFinished);
    if (reciver && slot)
        connect(this, SIGNAL(emitData(bool, QString)), reciver, slot);
}

void HttpClient::userLogin(const QString &user, const QString &password)
{
    QString url = QString("http://%1:%2/usermanager/login?user=%3&password=%4").arg(m_ip).arg(m_port).arg(user).arg(password);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::opSystemConfigAdd(const QString &systemname, const QString &systemver, const QString &systemdesc)
{
    QString url = QString("http://%1:%2/opsystemconfig/add?systemname=%3&systemver=%4&systemdesc=%5")
            .arg(m_ip).arg(m_port).arg(systemname).arg(systemver)
            .arg(systemdesc);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::opSystemConfigDelete(const QString &systemname, const QString &systemver)
{
    QString url = QString("http://%1:%2/opsystemconfig/delete?systemname=%3&systemver=%4")
            .arg(m_ip).arg(m_port).arg(systemname).arg(systemver);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::opSystemConfigUpdate(const QString &oldsystemname, const QString &oldsystemver, const QString &systemname, const QString &systemver, const QString &systemdesc)
{
    QString url = QString("http://%1:%2/opsystemconfig/update?oldsystemname=%3&oldsystemver=%4&systemname=%5&systemver=%6&systemdesc=%7")
            .arg(m_ip).arg(m_port).arg(oldsystemname).arg(oldsystemver).arg(systemname).arg(systemver).arg(systemdesc);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::opSystemConfigQueryList()
{
    QString url = QString("http://%1:%2/opsystemconfig/query")
            .arg(m_ip).arg(m_port);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::cpuConfigAdd(const QString &cpuname, const QString cpudesc)
{
    QString url = QString("http://%1:%2/cpuconfig/add?cpuname=%3&cpudesc=%4")
            .arg(m_ip).arg(m_port).arg(cpuname).arg(cpudesc);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::cpuConfigDelete(const QString &cpuname)
{
    QString url = QString("http://%1:%2/cpuconfig/delete?cpuname=%3")
            .arg(m_ip).arg(m_port).arg(cpuname);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::cpuConfigUpdate(const QString &oldcpuname, const QString &cpuname, const QString &cpudesc)
{
    QString url = QString("http://%1:%2/cpuconfig/update?oldcpuname=%3&cpuname=%4&cpudesc=%5")
            .arg(m_ip).arg(m_port).arg(oldcpuname).arg(cpuname).arg(cpudesc);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::cpuConfigQueryList()
{
    QString url = QString("http://%1:%2/cpuconfig/query")
            .arg(m_ip).arg(m_port);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::frameworkInfoAdd(const QString &name, const QString &ver, const QString &systemname, const QString &systemver, const QString &cpuname, int type, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << tr("%1 文件不存在!").arg(fileName);
        file.close();
        return;
    }
    QFileInfo upload_file_info(file);

    QString url = QString("http://%1:%2/frameworkinfo/add")
            .arg(m_ip).arg(m_port);

    QHttpPart part_name;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_name.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
    part_name.setBody(name.toUtf8());

    QHttpPart part_ver;
    //    part_ver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_ver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"ver\""));
    part_ver.setBody(ver.toUtf8());

    QHttpPart part_systemname;
    //    part_systemname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemname\""));
    part_systemname.setBody(systemname.toUtf8());


    QHttpPart part_systemver;
    //    part_systemver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemver\""));
    part_systemver.setBody(systemver.toUtf8());


    QHttpPart part_cpuname;
    //    part_cpuname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_cpuname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"cpuname\""));
    part_cpuname.setBody(cpuname.toUtf8());


    QHttpPart part_type;
    //    part_type.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_type.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"type\""));
    part_type.setBody(QString::number(type).toUtf8());


    QHttpPart part_file;

    QString fidd = upload_file_info.fileName();

    //    part_file.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";"
                                                                            " filename=\"" + upload_file_info.fileName() + "\""));
    //    part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\";filename=\""+file.fileName()+"\""));
    part_file.setBodyDevice(&file);


    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(part_name);
    multiPart->append(part_ver);
    multiPart->append(part_systemname);
    multiPart->append(part_systemver);
    multiPart->append(part_cpuname);
    multiPart->append(part_type);
    multiPart->append(part_file);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data;boundary="+multiPart->boundary());
    qDebug()<<"multiPart->boundary()"<<multiPart->boundary();


    QNetworkReply* reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply);
    QEventLoop eventLoop;
    QObject::connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    reply->deleteLater();
}

void HttpClient::frameworkInfoQueryList()
{
    QString url = QString("http://%1:%2/frameworkinfo/querylist")
            .arg(m_ip).arg(m_port);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::frameworkInfoDelete(const QString &name, const QString &ver)
{
    QString url = QString("http://%1:%2/frameworkinfo/delete?name=%3&ver=%3")
            .arg(m_ip).arg(m_port).arg(name).arg(ver);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::frameworkInfoUpdate(const QString &oldname, const QString &oldver, const QString &name, const QString &ver, const QString &systemname, const QString &systemver, const QString &cpuname, int type, const QString &fileName)
{

    QString url = QString("http://%1:%2/frameworkinfo/update")
            .arg(m_ip).arg(m_port);

    QHttpPart part_oldname;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_oldname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"oldname\""));
    part_oldname.setBody(oldname.toUtf8());

    QHttpPart part_oldver;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_oldver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"oldver\""));
    part_oldver.setBody(oldver.toUtf8());

    QHttpPart part_name;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_name.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
    part_name.setBody(name.toUtf8());

    QHttpPart part_ver;
    //    part_ver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_ver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"ver\""));
    part_ver.setBody(ver.toUtf8());

    QHttpPart part_systemname;
    //    part_systemname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemname\""));
    part_systemname.setBody(systemname.toUtf8());


    QHttpPart part_systemver;
    //    part_systemver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemver\""));
    part_systemver.setBody(systemver.toUtf8());


    QHttpPart part_cpuname;
    //    part_cpuname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_cpuname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"cpuname\""));
    part_cpuname.setBody(cpuname.toUtf8());


    QHttpPart part_type;
    //    part_type.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_type.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"type\""));
    part_type.setBody(QString::number(type).toUtf8());


    QHttpPart part_file;
    //    part_file.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    QFile file(fileName);
    if(!fileName.isEmpty())
    {
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("%1 文件不存在!").arg(fileName);
            file.close();
            return;
        }
        QFileInfo upload_file_info(file);
        part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";"
                                                                                " filename=\"" + upload_file_info.fileName() + "\""));
        part_file.setBodyDevice(&file);
    }
    else {
        part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";"
                                                                                " filename=\"""\""));
    }


    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(part_oldname);
    multiPart->append(part_oldver);
    multiPart->append(part_name);
    multiPart->append(part_ver);
    multiPart->append(part_systemname);
    multiPart->append(part_systemver);
    multiPart->append(part_cpuname);
    multiPart->append(part_type);
    multiPart->append(part_file);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data;boundary="+multiPart->boundary());
    qDebug()<<"multiPart->boundary()"<<multiPart->boundary();


    QNetworkReply* reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply);
    QEventLoop eventLoop;
    QObject::connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    reply->deleteLater();
}

void HttpClient::frameworkInfoDownload(const QString &fileName, const QString &saveFilePath)
{
    QDir dir(saveFilePath);
    if(!dir.exists())
        return;

    QString url = QString("http://%1:%2/frameworkinfo/download?filename=%3")
            .arg(m_ip).arg(m_port).arg(fileName);
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop eventLoop;
    QObject::connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QFile file(dir.absolutePath() + "/" + fileName);
    if(file.open(QFile::WriteOnly))
    {
        file.write(reply->readAll());
        file.close();
    }

    reply->deleteLater();
    //    connect(reply, QNetworkReply::downloadProgress())
}

void HttpClient::moduleInfoAdd(const QString &name, const QString &ver, const QString &systemname, const QString &systemver, const QString &cpuname, int type, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << tr("%1 文件不存在!").arg(fileName);
        file.close();
        return;
    }
    QFileInfo upload_file_info(file);

    QString url = QString("http://%1:%2/moduleinfo/add")
            .arg(m_ip).arg(m_port);

    QHttpPart part_name;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_name.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"modu_name\""));
    part_name.setBody(name.toUtf8());

    QHttpPart part_ver;
    //    part_ver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_ver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"ver\""));
    part_ver.setBody(ver.toUtf8());

    QHttpPart part_systemname;
    //    part_systemname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemname\""));
    part_systemname.setBody(systemname.toUtf8());


    QHttpPart part_systemver;
    //    part_systemver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemver\""));
    part_systemver.setBody(systemver.toUtf8());


    QHttpPart part_cpuname;
    //    part_cpuname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_cpuname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"cpuname\""));
    part_cpuname.setBody(cpuname.toUtf8());


    QHttpPart part_type;
    //    part_type.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_type.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"type\""));
    part_type.setBody(QString::number(type).toUtf8());


    QHttpPart part_file;

    QString fidd = upload_file_info.fileName();

    //    part_file.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";"
                                                                            " filename=\"" + upload_file_info.fileName() + "\""));
    //    part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\";filename=\""+file.fileName()+"\""));
    part_file.setBodyDevice(&file);


    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(part_name);
    multiPart->append(part_ver);
    multiPart->append(part_systemname);
    multiPart->append(part_systemver);
    multiPart->append(part_cpuname);
    multiPart->append(part_type);
    multiPart->append(part_file);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data;boundary="+multiPart->boundary());
    qDebug()<<"multiPart->boundary()"<<multiPart->boundary();


    QNetworkReply* reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply);
    QEventLoop eventLoop;
    QObject::connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    reply->deleteLater();
}

void HttpClient::moduleInfoDownload(const QString &fileName, const QString &saveFilePath)
{
    QDir dir(saveFilePath);
    if(!dir.exists())
        return;

    QString url = QString("http://%1:%2/moduleinfo/download?filename=%3")
            .arg(m_ip).arg(m_port).arg(fileName);
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop eventLoop;
    QObject::connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QFile file(dir.absolutePath() + "/" + fileName);
    if(file.open(QFile::WriteOnly))
    {
        file.write(reply->readAll());
        file.close();
    }

    reply->deleteLater();
}

void HttpClient::moduleInfoQueryList()
{
    QString url = QString("http://%1:%2/moduleinfo/querylist")
            .arg(m_ip).arg(m_port);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::moduleInfoDelete(const QString &name, const QString &ver)
{
    QString url = QString("http://%1:%2/moduleinfo/delete?modu_name=%3&ver=%3")
            .arg(m_ip).arg(m_port).arg(name).arg(ver);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::moduleInfoUpdate(const QString &oldname, const QString &oldver, const QString &name, const QString &ver, const QString &systemname, const QString &systemver, const QString &cpuname, int type, const QString &fileName)
{
    QString url = QString("http://%1:%2/moduleinfo/update")
            .arg(m_ip).arg(m_port);

    QHttpPart part_oldname;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_oldname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"oldname\""));
    part_oldname.setBody(oldname.toUtf8());

    QHttpPart part_oldver;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_oldver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"oldver\""));
    part_oldver.setBody(oldver.toUtf8());

    QHttpPart part_name;
    //    part_name.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_name.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"modu_name\""));
    part_name.setBody(name.toUtf8());

    QHttpPart part_ver;
    //    part_ver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_ver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"ver\""));
    part_ver.setBody(ver.toUtf8());

    QHttpPart part_systemname;
    //    part_systemname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemname\""));
    part_systemname.setBody(systemname.toUtf8());


    QHttpPart part_systemver;
    //    part_systemver.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_systemver.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"systemver\""));
    part_systemver.setBody(systemver.toUtf8());


    QHttpPart part_cpuname;
    //    part_cpuname.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_cpuname.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"cpuname\""));
    part_cpuname.setBody(cpuname.toUtf8());


    QHttpPart part_type;
    //    part_type.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part_type.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"type\""));
    part_type.setBody(QString::number(type).toUtf8());


    QHttpPart part_file;
    //    part_file.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    QFile file(fileName);
    if(!fileName.isEmpty())
    {
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("%1 文件不存在!").arg(fileName);
            file.close();
            return;
        }
        QFileInfo upload_file_info(file);
        part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";"
                                                                                " filename=\"" + upload_file_info.fileName() + "\""));
        part_file.setBodyDevice(&file);
    }
    else {
        part_file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";"
                                                                                " filename=\"""\""));
    }


    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(part_oldname);
    multiPart->append(part_oldver);
    multiPart->append(part_name);
    multiPart->append(part_ver);
    multiPart->append(part_systemname);
    multiPart->append(part_systemver);
    multiPart->append(part_cpuname);
    multiPart->append(part_type);
    multiPart->append(part_file);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data;boundary="+multiPart->boundary());
    qDebug()<<"multiPart->boundary()"<<multiPart->boundary();


    QNetworkReply* reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply);
    QEventLoop eventLoop;
    QObject::connect(reply,&QNetworkReply::finished,&eventLoop,&QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    reply->deleteLater();
}

void HttpClient::addProject(const QString &projectname, const QString &value)
{
    QString url = QString("http://%1:%2/project/add?projectname=%3&value=%4").arg(m_ip).arg(m_port)
            .arg(projectname).arg(value);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::updateProject(const QString &oldprojectname, const QString &projectname, const QString &value)
{
    QString url = QString("http://%1:%2/project/update?oldprojectname=%3&projectname=%4&value=%5").arg(m_ip).arg(m_port)
            .arg(oldprojectname).arg(projectname).arg(value);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::deleteProject(const QString &projectname)
{
    QString url = QString("http://%1:%2/project/delete?projectname=%3").arg(m_ip).arg(m_port)
            .arg(projectname);
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void HttpClient::queryProjectList()
{
    QString url = QString("http://%1:%2/project/query").arg(m_ip).arg(m_port);
    m_manager->get(QNetworkRequest(QUrl(url)));
}


void HttpClient::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError){
        emit emitData(false, reply->errorString());
    }else{
        QByteArray reply_data = reply->readAll();
        emit emitData(true, reply_data.data());
    }
}
