#ifndef _FILE_CONTENT_HXX_
#define _FILE_CONTENT_HXX_

#include <utility>

#include <qobject.h>

class FileContent {
public:
    FileContent(const QString &fileName, const QString &content) : _fileName(fileName), _content(content) {
    }

    FileContent(QString &&fileName, QString &&content) : _fileName(std::move(fileName)), _content(std::move(content)) {
    }

    const QString &get_file_name() const {
        return _fileName;
    }
    const QString &get_content() const {
        return _content;
    }

private:
    QString _fileName = "";
    QString _content = "";
};

#endif // _FILE_CONTENT_HXX_
