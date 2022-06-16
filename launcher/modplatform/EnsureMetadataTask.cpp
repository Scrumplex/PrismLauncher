#include "EnsureMetadataTask.h"

#include <MurmurHash2.h>
#include <QDebug>

#include "FileSystem.h"
#include "Json.h"
#include "minecraft/mod/Mod.h"
#include "minecraft/mod/tasks/LocalModUpdateTask.h"
#include "modplatform/flame/FlameAPI.h"
#include "modplatform/flame/FlameModIndex.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "modplatform/modrinth/ModrinthPackIndex.h"
#include "net/NetJob.h"
#include "tasks/MultipleOptionsTask.h"

static ModPlatform::ProviderCapabilities ProviderCaps;

static ModrinthAPI modrinth_api;
static FlameAPI flame_api;

EnsureMetadataTask::EnsureMetadataTask(Mod& mod, QDir dir, ModPlatform::Provider prov) : m_index_dir(dir), m_provider(prov)
{
    auto hash = getHash(mod);
    if (hash.isEmpty())
        emitFail(mod);
    else
        m_mods.insert(hash, mod);
}

EnsureMetadataTask::EnsureMetadataTask(std::list<Mod>& mods, QDir dir, ModPlatform::Provider prov) : m_index_dir(dir), m_provider(prov)
{
    for (auto mod : mods) {
        if (!mod.valid()) {
            emitFail(mod);
            continue;
        }

        auto hash = getHash(mod);
        if (hash.isEmpty()) {
            emitFail(mod);
            continue;
        }

        m_mods.insert(hash, mod);
    }
}

QString EnsureMetadataTask::getHash(Mod& mod)
{
    /* Here we create a mapping hash -> mod, because we need that relationship to parse the API routes */
    QByteArray jar_data;
    try {
        jar_data = FS::read(mod.fileinfo().absoluteFilePath());
    } catch (FS::FileSystemException& e) {
        qCritical() << QString("Failed to open / read JAR file of %1").arg(mod.name());
        qCritical() << QString("Reason: ") << e.cause();

        return {};
    }

    switch (m_provider) {
        case ModPlatform::Provider::MODRINTH: {
            auto hash_type = ProviderCaps.hashType(ModPlatform::Provider::MODRINTH).first();

            return QString(ProviderCaps.hash(ModPlatform::Provider::MODRINTH, jar_data, hash_type).toHex());
        }
        case ModPlatform::Provider::FLAME: {
            QByteArray jar_data_treated;
            for (char c : jar_data) {
                // CF-specific
                if (!(c == 9 || c == 10 || c == 13 || c == 32))
                    jar_data_treated.push_back(c);
            }

            return QString::number(MurmurHash2(jar_data_treated, jar_data_treated.length()));
        }
    }

    return {};
}

bool EnsureMetadataTask::abort()
{
    // Prevent sending signals to a dead object
    disconnect(this, 0, 0, 0);

    if (m_task_handler)
        return m_task_handler->abort();
    return false;
}

void EnsureMetadataTask::executeTask()
{
    setStatus(tr("Checking if mods have metadata..."));

    for (auto mod : m_mods) {
        if (!mod.valid())
            continue;

        // They already have the right metadata :o
        if (mod.status() != ModStatus::NoMetadata && mod.metadata() && mod.metadata()->provider == m_provider) {
            qDebug() << "Mod" << mod.name() << "already has metadata!";
            emitReady(mod);
            return;
        }

        // Folders don't have metadata
        if (mod.type() == Mod::MOD_FOLDER) {
            emitReady(mod);
            return;
        }
    }

    NetJob::Ptr tsk;

    switch (m_provider) {
        case (ModPlatform::Provider::MODRINTH):
            tsk = modrinthEnsureMetadata();
            break;
        case (ModPlatform::Provider::FLAME):
            tsk = flameEnsureMetadata();
            break;
    }

    connect(tsk.get(), &Task::finished, this, [this]{
        QMutableHashIterator<QString, Mod> mods_iter (m_mods);
        while (mods_iter.hasNext()) {
            auto mod = mods_iter.next();
            emitFail(mod.value());
        }

        emitSucceeded();
    });

    m_task_handler = tsk;

    if (m_mods.size() > 1)
        setStatus(tr("Requesting metadata information from %1...").arg(ProviderCaps.readableName(m_provider)));
    else if (!m_mods.empty())
        setStatus(tr("Requesting metadata information from %1 for '%2'...")
                      .arg(ProviderCaps.readableName(m_provider), m_mods.begin().value().name()));

    tsk->start();
}

void EnsureMetadataTask::emitReady(Mod& m)
{
    qDebug() << QString("Generated metadata for %1").arg(m.name());
    emit metadataReady(m);

    m_mods.remove(getHash(m));
}

void EnsureMetadataTask::emitFail(Mod& m)
{
    qDebug() << QString("Failed to generate metadata for %1").arg(m.name());
    emit metadataFailed(m);

    m_mods.remove(getHash(m));
}

NetJob::Ptr EnsureMetadataTask::modrinthEnsureMetadata()
{
    auto hash_type = ProviderCaps.hashType(ModPlatform::Provider::MODRINTH).first();

    auto* response = new QByteArray();
    auto ver_task = modrinth_api.currentVersions(m_mods.keys(), hash_type, response);

    // Prevents unfortunate timings when aborting the task
    if (!ver_task)
        return {};

    connect(ver_task.get(), &NetJob::succeeded, this, [this, response] {
        QJsonParseError parse_error{};
        QJsonDocument doc = QJsonDocument::fromJson(*response, &parse_error);
        if (parse_error.error != QJsonParseError::NoError) {
            qWarning() << "Error while parsing JSON response from Modrinth::CurrentVersions at " << parse_error.offset
                       << " reason: " << parse_error.errorString();
            qWarning() << *response;

            failed(parse_error.errorString());
            return;
        }

        try {
            auto entries = Json::requireObject(doc);
            for (auto& hash : m_mods.keys()) {
                auto mod = m_mods.find(hash).value();
                try {
                    auto entry = Json::requireObject(entries, hash);

                    setStatus(tr("Parsing API response from Modrinth for '%1'...").arg(mod.name()));
                    modrinthCallback(entry, mod);
                } catch (Json::JsonException& e) {
                    qDebug() << e.cause();
                    qDebug() << entries;

                    emitFail(mod);
                }
            }
        } catch (Json::JsonException& e) {
            qDebug() << e.cause();
            qDebug() << doc;
        }
    });

    return ver_task;
}

NetJob::Ptr EnsureMetadataTask::flameEnsureMetadata()
{
    auto* response = new QByteArray();

    std::list<uint> fingerprints;
    for (auto& murmur : m_mods.keys()) {
        fingerprints.push_back(murmur.toUInt());
    }

    auto ver_task = flame_api.matchFingerprints(fingerprints, response);

    connect(ver_task.get(), &Task::succeeded, this, [this, response] {
        QJsonParseError parse_error{};
        QJsonDocument doc = QJsonDocument::fromJson(*response, &parse_error);
        if (parse_error.error != QJsonParseError::NoError) {
            qWarning() << "Error while parsing JSON response from Modrinth::CurrentVersions at " << parse_error.offset
                       << " reason: " << parse_error.errorString();
            qWarning() << *response;

            failed(parse_error.errorString());
            return;
        }

        try {
            auto doc_obj = Json::requireObject(doc);
            auto data_obj = Json::requireObject(doc_obj, "data");
            auto data_arr = Json::requireArray(data_obj, "exactMatches");

            if (data_arr.isEmpty()) {
                qWarning() << "No matches found for fingerprint search!";

                return;
            }

            for (auto match : data_arr) {
                auto match_obj = Json::ensureObject(match, {});
                if (match_obj.isEmpty()) {
                    qWarning() << "Fingerprint match is empty!";

                    return;
                }

                auto fingerprint = QString::number(Json::ensureVariant(Json::ensureObject(match_obj, "file"), "fileFingerprint").toUInt());
                auto mod = m_mods.find(fingerprint);
                if (mod == m_mods.end()) {
                    qWarning() << "Invalid fingerprint from the API response.";
                    continue;
                }

                setStatus(tr("Parsing API response from CurseForge for '%1'...").arg(mod->name()));
                flameCallback(match_obj, mod.value());
            }

        } catch (Json::JsonException& e) {
            qDebug() << e.cause();
            qDebug() << doc;
        }
    });

    return ver_task;
}

void EnsureMetadataTask::modrinthCallback(QJsonObject& doc_obj, Mod& mod)
{
    auto ver = Modrinth::loadIndexedPackVersion(doc_obj, {}, mod.fileinfo().fileName());

    // Minimal IndexedPack to create the metadata
    ModPlatform::IndexedPack pack;
    pack.name = mod.name();
    pack.provider = ModPlatform::Provider::MODRINTH;
    pack.addonId = ver.addonId;

    // Prevent file name mismatch
    ver.fileName = mod.fileinfo().fileName();

    QDir tmp_index_dir(m_index_dir);

    {
        LocalModUpdateTask update_metadata(m_index_dir, pack, ver);
        QEventLoop loop;

        QObject::connect(&update_metadata, &Task::finished, &loop, &QEventLoop::quit);

        update_metadata.start();

        if (!update_metadata.isFinished())
            loop.exec();
    }

    auto mod_name = mod.name();
    auto meta = new Metadata::ModStruct(Metadata::get(tmp_index_dir, mod_name));
    mod.setMetadata(meta);

    emitReady(mod);
}

void EnsureMetadataTask::flameCallback(QJsonObject& match_obj, Mod& mod)
{
    QDir tmp_index_dir(m_index_dir);

    try {
        auto file_obj = Json::ensureObject(match_obj, "file");

        ModPlatform::IndexedPack pack;
        pack.name = mod.name();
        pack.provider = ModPlatform::Provider::FLAME;
        pack.addonId = Json::requireInteger(file_obj, "modId");

        ModPlatform::IndexedVersion ver = FlameMod::loadIndexedPackVersion(file_obj);

        // Prevent file name mismatch
        ver.fileName = mod.fileinfo().fileName();

        {
            LocalModUpdateTask update_metadata(m_index_dir, pack, ver);
            QEventLoop loop;

            QObject::connect(&update_metadata, &Task::finished, &loop, &QEventLoop::quit);

            update_metadata.start();

            if (!update_metadata.isFinished())
                loop.exec();
        }

        auto mod_name = mod.name();
        auto meta = new Metadata::ModStruct(Metadata::get(tmp_index_dir, mod_name));
        mod.setMetadata(meta);

        emitReady(mod);
    } catch (Json::JsonException& e) {
        qDebug() << e.cause();

        emitFail(mod);
    }
}
