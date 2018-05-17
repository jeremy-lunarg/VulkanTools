
#include "override_settings.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTextStream>

// This is only needed to keep the manifest version up to date. Other than that, it shouldn't be needed
#include <vulkan/vulkan.h>

OverrideSettings::OverrideSettings()
{
    // Load the override layer (if found)
    QFile layer_file(LayerFile());
    if (layer_file.exists() && layer_file.open(QFile::ReadOnly)) {
        QString data = layer_file.readAll();
        layer_file.close();

        QJsonDocument document = QJsonDocument::fromJson(data.toLocal8Bit());
        if (document.isObject()) {
            QJsonValue layer = document.object().value("layer");
            if (layer.isObject()) {
                QJsonValue components = layer.toObject().value("component_layers");
                if (components.isArray()) {
                    for(const QJsonValue& component : components.toArray()) {
                        if (!component.isString()) {
                            continue;
                        }
                        layers.append(component.toString());
                    }
                }
            }
        }
    }

    // Load the settings file (if found)
    QFile settings_file(LayerSettingsFile());
    if (settings_file.exists() && settings_file.open(QFile::ReadOnly)) {
        QString data = settings_file.readAll();
        settings_file.close();

        for (const QString& line : data.split(QRegularExpression("\n|\r\n|\r"))) {
            if (QRegularExpression("\\s*#").match(line).hasMatch()) {
                continue;
            }

            QList<QString> sections = line.split(".");
            if (sections.size() != 2) {
                continue;
            }
            QString layer_abbrev = sections[0];
            QList<QString> subsections = sections[1].split("=");
            if (subsections.size() != 2) {
                continue;
            }
            QString setting_name = subsections[0];
            QString setting_value = subsections[1];

            QList<QString> layer_sections = layer_abbrev.split("_");
            layer_sections[0] = layer_sections[0].toUpper();
            layer_sections.prepend("LAYER");
            layer_sections.prepend("VK");
            QString layer_name = layer_sections.join("_");

            layer_settings[layer_name][setting_name] = setting_value;
        }
    }
}

QList<QString> OverrideSettings::Layers() const
{
    return layers;
}

QHash<QString, QHash<QString, QString>> OverrideSettings::LayerSettings() const
{
    return layer_settings;
}

bool OverrideSettings::SaveLayers(const QList<QPair<QString, LayerType>> &paths, const QList<LayerManifest> &layers, int expiration)
{
    QDateTime now = QDateTime::currentDateTime();
    now = now.addSecs(expiration);

    QJsonArray json_paths;
    for (const QPair<QString, LayerType> &pair : paths) {
        json_paths.append(pair.first);
    }
    QJsonArray json_layers;
    for (const LayerManifest &manifest : layers) {
        json_layers.append(manifest.name);
    }
    QJsonObject disable;
    disable.insert("DISABLE_VK_LAYER_LUNARG_override", "1");

    QJsonObject layer;
    layer.insert("name", "VK_LAYER_LUNARG_override");
    layer.insert("type", "GLOBAL");
    layer.insert("api_version", "1.1." + QString::number(VK_HEADER_VERSION));
    layer.insert("implementation_version", "1");
    layer.insert("description", "LunarG Override Layer");
    if (expiration >= 0) {
        layer.insert("expiration_date", now.toString("yyyy-MM-dd-hh-mm"));
    }
    layer.insert("override_paths", json_paths);
    layer.insert("component_layers", json_layers);
    layer.insert("disable_environment", disable);

    QJsonObject root;
    root.insert("file_format_version", QJsonValue("1.1.2"));
    root.insert("layer", layer);
    QJsonDocument doc(root);

    // TODO: Create directory?
    QFile layer_file(LayerFile());
    if (!layer_file.open(QFile::WriteOnly)) {
        return true;
    }
    layer_file.write(doc.toJson());
    layer_file.close();

    return false;
}

bool OverrideSettings::SaveSettings(const QHash<QString, QHash<QString, LayerValue>> &settings)
{
    // TODO: Create directory?
    //settings_path.mkpath(".");
    QFile settings_file(LayerSettingsFile());
    if (!settings_file.open(QFile::WriteOnly)) {
        return true;
    }

    QTextStream stream(&settings_file);
    for (const QString &layer : settings.keys()) {
        if (!layer.startsWith("VK_LAYER_")) {
            continue;
        }
        QString short_layer = layer.right(layer.size() - 9).toLower();

        stream << "\n# " << layer << "\n";
        for (const QString &option : settings[layer].keys()) {
            QString string_value = settings[layer][option].values.values().join(",");
            stream << short_layer << "." << option << " = " << string_value << "\n";
        }
    }
    settings_file.close();

    return false;
}

QString OverrideSettings::LayerFile() const
{
#if defined(_WIN32)

#else
    return QDir(QDir::homePath() + "/vulkan/implicit_layer.d").absoluteFilePath("VkLayer_override.json");
#endif
}

QString OverrideSettings::LayerSettingsFile() const
{
#if defined(_WIN32)

#else
    return QDir(QDir::homePath() + "/vulkan/settings.d").absoluteFilePath("vk_layer_settings.txt");
#endif
}
