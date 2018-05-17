
#pragma once

#include <QHash>
#include <QList>
#include <QString>

#include "layer_manifest.h"

// Add a Paths() method?
class OverrideSettings
{
public:
    OverrideSettings();
    QList<QString> Layers() const;
    QHash<QString, QHash<QString, QString>> LayerSettings() const;
    bool SaveLayers(const QList<QPair<QString, LayerType>> &paths, const QList<LayerManifest> &layers, int expiration);
    bool SaveSettings(const QHash<QString, QHash<QString, LayerValue>> &settings);

private:
    QString LayerFile() const;
    QString LayerSettingsFile() const;

    QList<QString> layers;
    QHash<QString, QHash<QString, QString>> layer_settings;
};
