#include "http_server.hxx"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QtConcurrent/qtconcurrentrun.h>
#include <qcontainerfwd.h>
#include <qframe.h>
#include <qfuture.h>
#include <qhostaddress.h>
#include <qhttpserverrequest.h>
#include <qhttpserverresponse.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstringview.h>
#include <qthread.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qurlquery.h>
#include <qwidget.h>

#include "backup_manager.hxx"
#include "config.hxx"
#include "file_content.hxx"
#include "utility.hxx"

using std::make_unique;
using std::mt19937;
using std::numeric_limits;
using std::random_device;
using std::runtime_error;
using std::string;
using std::uint16_t;
using std::uniform_int_distribution;
using std::unordered_map;
using std::vector;

HttpServer &HttpServer::get_instance() {
    static HttpServer instance(54321);
    return instance;
}

void HttpServer::set_is_approved(ConnectId connectId, bool approved, const QString &savePath) {
    if (!approved) {
        _connectionMetas.erase(connectId);
        return;
    }

    Config &config = Config::get_instance();

    const string backupDirectory = savePath.toStdString();
    _connectionMetas[connectId].backupManager = make_unique<BackupManager>(backupDirectory);
    _connectionMetas[connectId].frame = make_unique<QFrame>();
    _connectionMetas[connectId].timer = make_unique<QTimer>();
    _connectionMetas[connectId].timer->setSingleShot(true);
    _connectionMetas[connectId].timer->setInterval(60 * 1000);
    _connectionMetas[connectId].timer->start();
    _connectionMetas[connectId].approved = true;

    connect(_connectionMetas[connectId].timer.get(), &QTimer::timeout, this,
            [this, connectId]() { remove_connection(connectId); });

    emit signal_connection_approved(_connectionMetas[connectId].frame.get(), _connectionMetas[connectId]);
}

HttpServer::HttpServer(uint16_t port) {
    _server.route("/ping", [this]() { return on_ping(); });

    _server.route("/connect", QHttpServerRequest::Method::Post,
                  [this](const QHttpServerRequest &request) { return on_connect(request); });

    _server.route("/status", QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request) { return on_status(request); });

    _server.route("/upload", QHttpServerRequest::Method::Post,
                  [this](const QHttpServerRequest &request) { return on_upload(request); });

    if (!_tcpServer.listen(QHostAddress::Any, port) || !_server.bind(&_tcpServer)) {
        throw runtime_error("Failed to start HTTP server " + _tcpServer.errorString().toStdString());
    }
}

size_t HttpServer::get_connection_count() const {
    return _connectionMetas.size();
}

unordered_map<HttpServer::ConnectId, ConnectionMeta>::const_iterator HttpServer::get_connection_begin() const {
    return _connectionMetas.cbegin();
}

unordered_map<HttpServer::ConnectId, ConnectionMeta>::const_iterator HttpServer::get_connection_end() const {
    return _connectionMetas.cend();
}

void HttpServer::remove_connection(ConnectId connectId) {
    if (_connectionMetas.find(connectId) != _connectionMetas.end()) {
        QFrame *frame = _connectionMetas[connectId].frame.get();
        emit signal_connection_removed(frame);
        _connectionMetas.erase(connectId);
    }
}

QHttpServerResponse HttpServer::on_ping() {
    QJsonObject response;

    const Config &config = Config::get_instance();
    response["Name"] = config.get_server_name();

    return QHttpServerResponse(response);
}

QHttpServerResponse HttpServer::on_connect(const QHttpServerRequest &request) {
    QByteArray rawBody = request.body();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawBody, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return QHttpServerResponse("Invalid JSON: " + parseError.errorString(),
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonObject json = doc.object();
    QString clientName = json["ClientName"].toString();

    ConnectId connectId = generate_connect_id();

    _connectionMetas.try_emplace(connectId);
    _connectionMetas[connectId].clientName = std::move(clientName);

    emit signal_connect_request(_connectionMetas[connectId].clientName, connectId);

    QJsonObject response;
    response["ConnectId"] = connectId;
    return QHttpServerResponse(response);
}

QHttpServerResponse HttpServer::on_status(const QHttpServerRequest &request) {
    const QUrlQuery query(request.url());
    const QString connectIdString = query.queryItemValue("ConnectId");
    const QString taskIdString = query.queryItemValue("TaskId");

    if (connectIdString.isEmpty() && taskIdString.isEmpty()) {
        return QHttpServerResponse("ConnectId or TaskId is required", QHttpServerResponse::StatusCode::BadRequest);
    }
    else if (!connectIdString.isEmpty()) {
        bool ok = false;
        ConnectId connectId = convert_to_connect_id(connectIdString, &ok);
        if (!ok) {
            return QHttpServerResponse("Invalid ConnectId. ConnectId must be integer",
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        if (_connectionMetas.find(connectId) == _connectionMetas.end()) {
            return QHttpServerResponse("Invalid ConnectId. ConnectionId not found.",
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        const ConnectionMeta &meta = _connectionMetas[connectId];

        QJsonObject response;
        response["ConnectId"] = connectId;
        response["Approved"] = meta.approved;
        return QHttpServerResponse(response);
    }
    else {
        bool ok = false;
        TaskId taskId = convert_to_task_od(taskIdString, &ok);
        if (!ok) {
            return QHttpServerResponse("Invalid TaskId. TaskId must be integer",
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        if (_taskFutures.find(taskId) == _taskFutures.end()) {
            return QHttpServerResponse("Invalid TaskId. TaskId not found.",
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        QJsonObject response;
        response["TaskId"] = taskId;

        const QFuture<vector<int>> &future = _taskFutures[taskId];
        if (future.isRunning()) {
            response["Status"] = "Running";
        }
        else if (future.isFinished()) {
            response["Status"] = "Finished";

            const vector<int> &failedIndexes = future.result();
            QJsonArray resultArray;
            for (int failedIndex : failedIndexes) {
                resultArray.append(failedIndex);
            }
            response["Result"] = resultArray;

            _taskFutures.erase(taskId);
        }
        else {
            response["Status"] = "Not running and not finished";
        }

        return QHttpServerResponse(response);
    }
}

QHttpServerResponse HttpServer::on_upload(const QHttpServerRequest &request) {
    QByteArray rawBody = request.body();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawBody, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return QHttpServerResponse("Invalid JSON: " + parseError.errorString(),
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonObject json = doc.object();

    if (!json.contains("ConnectId")) {
        return QHttpServerResponse("Missing 'ConnectId' field", QHttpServerResponse::StatusCode::BadRequest);
    }

    ConnectId connectId = json["ConnectId"].toInteger();
    if (_connectionMetas.find(connectId) == _connectionMetas.end()) {
        return QHttpServerResponse("Invalid connectId", QHttpServerResponse::StatusCode::BadRequest);
    }

    if (!json.contains("Data") || !json["Data"].isArray()) {
        return QHttpServerResponse("Missing or invalid 'Data' field", QHttpServerResponse::StatusCode::BadRequest);
    }

    vector<FileContent> files;

    QJsonArray dataArray = json["Data"].toArray();
    for (const QJsonValue &node : dataArray) {
        if (!node.isObject()) {
            return QHttpServerResponse("Invalid file Data format in: " + node.toString(),
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        QJsonObject fileObject = node.toObject();
        if (!fileObject.contains("FileName") || !fileObject.contains("CreationDate") ||
            !fileObject.contains("Content")) {
            return QHttpServerResponse("Missing 'FileName', 'CreationDate' or 'Content' field in: " + node.toString(),
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        const QString &fileName = fileObject["FileName"].toString();
        const QString &creationDate = fileObject["CreationDate"].toString();
        const QString &content = fileObject["Content"].toString();

        files.emplace_back(std::move(fileName), std::move(creationDate), std::move(content));
    }

    const BackupManager *backupManager = _connectionMetas[connectId].backupManager.get();

    TaskId taskId = generate_task_id();

    _taskFutures[taskId] = QtConcurrent::run(&BackupManager::backup_files, backupManager, std::move(files));

    QJsonObject response;
    response["TaskId"] = taskId;
    return QHttpServerResponse(response);
}

HttpServer::ConnectId HttpServer::generate_connect_id() {
    ConnectId connectId = generate_random_number(static_cast<ConnectId>(0), numeric_limits<ConnectId>::max());
    while (_connectionMetas.find(connectId) != _connectionMetas.end()) {
        connectId = generate_random_number(static_cast<ConnectId>(0), numeric_limits<ConnectId>::max());
    }
    return connectId;
}

HttpServer::TaskId HttpServer::generate_task_id() {
    TaskId taskId = generate_random_number(static_cast<TaskId>(0), numeric_limits<TaskId>::max());
    while (_taskFutures.find(taskId) != _taskFutures.end()) {
        taskId = generate_random_number(static_cast<TaskId>(0), numeric_limits<TaskId>::max());
    }
    return taskId;
}

QString HttpServer::generate_name() {
    const vector<QString> adjectives = {"quick",  "lazy",  "happy", "sad",    "brave",
                                        "clever", "witty", "calm",  "bright", "gentle"};
    const vector<QString> nouns = {"fox", "dog", "cat", "mouse", "lion", "tiger", "bear", "wolf", "eagle", "shark"};

    int intMax = numeric_limits<int>::max();
    int adjectiveIndex = generate_random_number(0, static_cast<int>(adjectives.size() - 1));
    int nounIndex = generate_random_number(0, static_cast<int>(nouns.size() - 1));
    int number = generate_random_number(1, 9999);

    QString name = adjectives[adjectiveIndex] + "_" + nouns[nounIndex] + "_" + QString::number(number);
    return name;
}
