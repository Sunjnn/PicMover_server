#ifndef _CONFIG_HXX_
#define _CONFIG_HXX_

#include <qcontainerfwd.h>
#include <qmap.h>
#include <qobject.h>
#include <qvariant.h>

constexpr const char *APPLICATION_NAME = "PicMover";

class Config {
private:
    explicit Config(const QString &filePath);
    ~Config();

public:
    static Config &get_instance();

    void load();

    void save() const;

    const QString &get_config_file_path() const;

    const QString &get_server_name() const;

    void set_server_name(const QString &serverName);
    void set_server_name(QString &&serverName);

    const QString &get_default_save_dir() const;

    void set_default_save_dir(const QString &dir);
    void set_default_save_dir(QString &&dir);

private:
    QString _configFilePath;
    QString _serverName;
    QString _defaultSaveDir;
};

#endif // _CONFIG_HXX_
