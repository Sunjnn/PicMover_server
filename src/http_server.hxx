#ifndef _HTTP_SERVER_HXX_
#define _HTTP_SERVER_HXX_

#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>

#include <qcontainerfwd.h>
#include <qfuture.h>
#include <qhostaddress.h>
#include <qhttpserver.h>
#include <qhttpserverresponse.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qtcpserver.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>

#include "backup_manager.hxx"

struct ConnectionMeta {
    QString clientName;
    std::unique_ptr<BackupManager> backupManager;
    bool approved;
};

class MainWindow;

class HttpServer : public QObject {
    Q_OBJECT

public:
    using ConnectId = qsizetype;
    using TaskId = qsizetype;

    ConnectId convert_to_connect_id(const QString &connectIdStr, bool *ok) {
        return connectIdStr.toLongLong(ok);
    }
    TaskId convert_to_task_od(const QString &taskIdStr, bool *ok) {
        return taskIdStr.toLongLong(ok);
    }

    static HttpServer &get_instance();

    void set_is_approved(ConnectId connectId, bool approved, const QString &savePath);

    size_t get_connection_count() const;
    std::unordered_map<ConnectId, ConnectionMeta>::const_iterator get_connection_begin() const;
    std::unordered_map<ConnectId, ConnectionMeta>::const_iterator get_connection_end() const;

signals:
    void signal_connect_request(QString clientName, ConnectId connectId);

private:
    HttpServer(std::uint16_t port);

    QHttpServerResponse on_ping();
    QHttpServerResponse on_connect(const QHttpServerRequest &request);
    QHttpServerResponse on_status(const QHttpServerRequest &request);
    QHttpServerResponse on_upload(const QHttpServerRequest &request);

    ConnectId generate_connect_id();

    TaskId generate_task_id();

    QString generate_name();

    QHttpServer _server;
    QTcpServer _tcpServer;

    std::unordered_map<ConnectId, ConnectionMeta> _connectionMetas;
    std::unordered_map<TaskId, QFuture<std::vector<int>>> _taskFutures;
};

#endif // _HTTP_SERVER_HXX_
