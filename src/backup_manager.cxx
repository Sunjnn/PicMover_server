#include "backup_manager.hxx"

#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtypes.h>

#include "file_content.hxx"

using std::invalid_argument;
using std::ofstream;
using std::string;
using std::tolower;
using std::transform;
using std::vector;
using std::filesystem::path;

vector<int> BackupManager::backup_files(const vector<FileContent> &files) const {
    vector<int> failedIndexes;

    for (size_t i = 0; i < files.size(); ++i) {
        const FileContent &file = files[i];
        const QString &fileName = file.get_file_name();
        const QString &creationDate = file.get_creation_date();
        const QByteArray &content = QByteArray::fromBase64(file.get_content().toUtf8());

        if (!backup_file(fileName, creationDate, content)) {
            failedIndexes.push_back(i);
        }
    }

    return failedIndexes;
}

bool BackupManager::backup_file(const QString &fileName, const QString &creationDate, const QByteArray &content) const {
    qsizetype dotPos = fileName.lastIndexOf('.');
    if (dotPos == -1)
        return false;

    QString formatString = fileName.mid(dotPos + 1);
    try {
        get_format(formatString);
    }
    catch (const invalid_argument &e) {
        // Handle unsupported format
        return false;
    }

    const QString &baseName = fileName.left(dotPos);
    QString newFileName = baseName + "_" + creationDate + "." + formatString;

    path absolutePath = _backupDirectory / newFileName.toStdString();
    ofstream ofs(absolutePath, std::ios::binary);
    ofs.write(content.constData(), content.size());
    return ofs.good();
}

BackupManager::Format BackupManager::get_format(const QString &formatString) {
    QString lowerFormatString = formatString.toLower();

    if (lowerFormatString == "heic") {
        return Format::HEIC;
    }
    else if (lowerFormatString == "jpeg" || lowerFormatString == "jpg") {
        return Format::JPEG;
    }
    else if (lowerFormatString == "png") {
        return Format::PNG;
    }
    throw invalid_argument("Unknown image format: " + formatString.toStdString());
}
