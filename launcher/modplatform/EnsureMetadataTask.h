#pragma once

#include "ModIndex.h"
#include "tasks/SequentialTask.h"
#include "net/NetJob.h"

class Mod;
class QDir;
class MultipleOptionsTask;

class EnsureMetadataTask : public Task {
    Q_OBJECT

   public:
    EnsureMetadataTask(Mod&, QDir, ModPlatform::Provider = ModPlatform::Provider::MODRINTH);
    EnsureMetadataTask(std::list<Mod>&, QDir, ModPlatform::Provider = ModPlatform::Provider::MODRINTH);

   public slots:
    bool abort() override;
   protected slots:
    void executeTask() override;

   private:
    // FIXME: Move to their own namespace
    auto modrinthEnsureMetadata() -> NetJob::Ptr;
    auto flameEnsureMetadata() -> NetJob::Ptr;

    // Helpers
    void emitReady(Mod&);
    void emitFail(Mod&);

    auto getHash(Mod&) -> QString;

   private slots:
    void modrinthCallback(QJsonObject&, Mod&);
    void flameCallback(QJsonObject&, Mod&);

   signals:
    void metadataReady(Mod&);
    void metadataFailed(Mod&);

   private:
    QHash<QString, Mod> m_mods;
    QDir m_index_dir;
    ModPlatform::Provider m_provider;

    NetJob::Ptr m_task_handler;
};
