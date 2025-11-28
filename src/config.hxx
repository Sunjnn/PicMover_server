#ifndef _CONFIG_HXX_
#define _CONFIG_HXX_

#include <qcontainerfwd.h>
#include <qmap.h>
#include <qobject.h>
#include <qstandardpaths.h>
#include <qvariant.h>

constexpr const char *APPLICATION_NAME = "PicMover";

class Config {
private:
    explicit Config();
    ~Config();

public:
    static Config &get_instance();

    void load();

    void save() const;

    static const QString &get_config_file_path();
    static void set_config_file_path(const QString &filePath);

    static const QString &get_log_file_path();
    static void set_log_file_path(const QString &filePath);

    const QString &get_server_name() const;

    void set_server_name(const QString &serverName);
    void set_server_name(QString &&serverName);

    const QString &get_default_save_dir() const;

    void set_default_save_dir(const QString &dir);
    void set_default_save_dir(QString &&dir);

private:
    static QString _configFilePath;
    static QString _logFilePath;

    QString _serverName;
    QString _defaultSaveDir;
};

#endif // _CONFIG_HXX_
