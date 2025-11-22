#ifndef _FILE_CONTENT_HXX_
#define _FILE_CONTENT_HXX_

#include <utility>

#include <qcontainerfwd.h>
#include <qobject.h>

class FileContent {
public:
    FileContent(const QString &fileName, const QString &creationDate, const QString &content)
        : _fileName(fileName), _creationDate(creationDate), _content(content) {
    }

    FileContent(QString &&fileName, QString &&creationDate, QString &&content)
        : _fileName(std::move(fileName)), _creationDate(std::move(creationDate)), _content(std::move(content)) {
    }

    const QString &get_file_name() const {
        return _fileName;
    }
    const QString &get_creation_date() const {
        return _creationDate;
    }
    const QString &get_content() const {
        return _content;
    }

private:
    QString _fileName = "";
    QString _creationDate = "";
    QString _content = "";
};

#endif // _FILE_CONTENT_HXX_
