#ifndef _BACKUP_MANAGER_HXX_
#define _BACKUP_MANAGER_HXX_

#include <filesystem>
#include <string>
#include <vector>

#include <qcontainerfwd.h>
#include <qobject.h>

#include "file_content.hxx"

class BackupManager {
public:
    BackupManager(const std::string &backupDirectory);

    std::vector<int> backup_files(const std::vector<FileContent> &files) const;

    bool backup_file(const QString &fileName, const QString &creationDate, const QByteArray &content) const;

    QString get_backup_directory() const;

    QString get_real_path(const QString &fileName, const QString &creationDate) const;

private:
    enum class Format : char {
        HEIC = 0,
        JPEG = 1,
        PNG = 2,
    };

    static Format get_format(const QString &formatString);

    std::filesystem::path _backupDirectory;
};

#endif // _BACKUP_MANAGER_HXX_
