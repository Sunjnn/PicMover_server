#include "config.hxx"

#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qlogging.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qvariant.h>

#include "utility.hxx"

Config::Config(const QString &filePath) : _configFilePath(filePath) {
    qInfo() << "Config file path: " << filePath;
    load();
}

Config::~Config() {
    save();
}

Config &Config::get_instance() {
    QString configFilePath =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/PicMover_config.ini";
    static Config config(configFilePath);
    return config;
}

void Config::load() {
    QSettings settings(_configFilePath, QSettings::IniFormat);

    _serverName = settings.value("server_name").toString();
    if (_serverName.isEmpty()) {
        _serverName = generate_name();
    }

    _defaultSaveDir = settings.value("default_save_directory").toString();
    if (_defaultSaveDir.isEmpty()) {
        _defaultSaveDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + APPLICATION_NAME;
    }
    qInfo() << "Default save directory: " << _defaultSaveDir;
}

void Config::save() const {
    QSettings settings(_configFilePath, QSettings::IniFormat);
    settings.clear();

    settings.setValue("server_name", _serverName);
}

const QString &Config::get_server_name() const {
    return _serverName;
}

void Config::set_server_name(const QString &serverName) {
    _serverName = serverName;
}

void Config::set_server_name(QString &&serverName) {
    _serverName = std::move(serverName);
}

const QString &Config::get_config_file_path() const {
    return _configFilePath;
}

const QString &Config::get_default_save_dir() const {
    return _defaultSaveDir;
}

void Config::set_default_save_dir(const QString &dir) {
    _defaultSaveDir = dir;
}

void Config::set_default_save_dir(QString &&dir) {
    _defaultSaveDir = dir;
}
