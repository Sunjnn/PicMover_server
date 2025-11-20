#ifndef _HTTP_SERVER_HXX_
#define _HTTP_SERVER_HXX_

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

class MainWindow;

class HttpServer : public QObject {
    Q_OBJECT

public:
    using ConnectId = qsizetype;
    using TaskId = qsizetype;

    HttpServer(MainWindow *parent, std::uint16_t port);

    ConnectId convert_to_connect_id(const QString &connectIdStr, bool *ok) {
        return connectIdStr.toLongLong(ok);
    }
    TaskId convert_to_task_od(const QString &taskIdStr, bool *ok) {
        return taskIdStr.toLongLong(ok);
    }

    void set_is_approved(ConnectId connectId, bool approved, const QString &savePath);

signals:
    void signal_connect_request(QString clientName, ConnectId connectId);

private:
    QHttpServerResponse on_ping();
    QHttpServerResponse on_connect(const QHttpServerRequest &request);
    QHttpServerResponse on_status(const QHttpServerRequest &request);
    QHttpServerResponse on_upload(const QHttpServerRequest &request);

    ConnectId generate_connect_id();

    TaskId generate_task_id();

    QString generate_name();

    struct ConnectionMeta {
        bool approved;
        std::unique_ptr<BackupManager> backupManager;
    };

    QHttpServer _server;
    QTcpServer _tcpServer;

    std::unordered_map<ConnectId, ConnectionMeta> _connectionMetas;
    std::unordered_map<TaskId, QFuture<std::vector<int>>> _taskFutures;
};

#endif // _HTTP_SERVER_HXX_
