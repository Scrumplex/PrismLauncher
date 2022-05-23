#include "FileResolvingTask.h"
#include "Json.h"
#include "modplatform/flame/PackManifest.h"
#include "net/Upload.h"

Flame::FileResolvingTask::FileResolvingTask(shared_qobject_ptr<QNetworkAccessManager> network, Flame::Manifest& toProcess)
    : m_network(network), m_toProcess(toProcess)
{}

void Flame::FileResolvingTask::executeTask()
{
    setStatus(tr("Resolving mod IDs..."));
    setProgress(0, m_toProcess.files.size());
    m_dljob = new NetJob("Mod id resolver", m_network);
    modsResult.reset(new QByteArray());
    filesResult.reset(new QByteArray());
    //build json data to send

    QJsonObject modsObject;
    modsObject["modIds"] = QJsonArray::fromVariantList(std::accumulate(m_toProcess.files.begin(), m_toProcess.files.end(), QVariantList(), [](QVariantList& l, const File& s) {
        l.push_back(s.projectId);
        return l;
    }));

    QByteArray modsData = QJsonDocument(modsObject).toJson();
    auto modsFetch = Net::Upload::makeByteArray(QUrl("https://api.curseforge.com/v1/mods"), modsResult.get(), modsData);
    m_dljob->addNetAction(modsFetch);

    //build json data to send
    QJsonObject filesObject;
    filesObject["fileIds"] = QJsonArray::fromVariantList(std::accumulate(m_toProcess.files.begin(), m_toProcess.files.end(), QVariantList(), [](QVariantList& l, const File& s) {
        l.push_back(s.fileId);
        return l;
    }));

    QByteArray filesData = QJsonDocument(filesObject).toJson();
    auto filesFetch = Net::Upload::makeByteArray(QUrl("https://api.curseforge.com/v1/mods/files"), filesResult.get(), filesData);
    m_dljob->addNetAction(filesFetch);
    connect(m_dljob.get(), &NetJob::finished, this, &Flame::FileResolvingTask::netJobFinished);
    m_dljob->start();
}

void Flame::FileResolvingTask::netJobFinished()
{
    bool failed = false;
    int index = 0;
    QHash<int, QString> projectIdSlugMap;

    qDebug() << modsResult.get();

    auto doc = Json::requireDocument(*modsResult);
    auto array = doc.object()["data"].toArray();
    projectIdSlugMap.reserve(array.count());
    for (QJsonValueRef mod : array) {
        Flame::Mod out;
        try {
            failed &= out.parseFromObject(mod.toObject());
        } catch (const JSONValidationError& e) {
            qCritical() << "Resolving failed because of a parsing error:";
            qCritical() << e.cause();
            qCritical() << "JSON:";
            qCritical() << mod;
            failed = true;
        }
        if (!failed) {
            projectIdSlugMap.insert(out.projectId, out.slug);
        }
        index++;
    }

    doc = Json::requireDocument(*filesResult);
    array = doc.object()["data"].toArray();
    for (QJsonValueRef file : array) {
        auto& out = m_toProcess.files[file.toObject()["id"].toInt()];
        out.projectSlug = projectIdSlugMap.value(out.projectId, QString());
        try {
            failed &= (!out.parseFromObject(file.toObject()));
        } catch (const JSONValidationError& e) {
            qCritical() << "Resolving failed because of a parsing error:";
            qCritical() << e.cause();
            qCritical() << "JSON:";
            qCritical() << file;
            failed = true;
        }
        index++;
    }
    if (!failed) {
        emitSucceeded();
    } else {
        emitFailed(tr("Some mod ID resolving tasks failed."));
    }
}
