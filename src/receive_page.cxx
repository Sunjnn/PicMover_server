#include "receive_page.hxx"

#include <qboxlayout.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscrollarea.h>

#include "config.hxx"
#include "http_server.hxx"

using Qt::AlignCenter;
using Qt::AlignTop;

ReceivePage::ReceivePage(const QString &serverName, QWidget *parent) : QWidget(parent) {
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(20, 20, 20, 20);

    Config &config = Config::get_instance();

    _serverNameLabel = new QLabel(serverName);
    _serverNameLabel->setStyleSheet("font-weight: bold; font-size: 20px;");
    _serverNameLabel->setAlignment(AlignCenter);
    _layout->addWidget(_serverNameLabel);

    auto *scrollWidget = new QWidget();
    auto *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setAlignment(AlignTop);

    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);
    scrollArea->hide();

    _layout->addWidget(scrollArea, 1);

    _scrollArea = scrollArea;
    _clientListLayout = scrollLayout;

    HttpServer &httpServer = HttpServer::get_instance();
    connect(&httpServer, &HttpServer::signal_connection_approved, this, &ReceivePage::on_connection_approved);
    connect(&httpServer, &HttpServer::signal_connection_removed, this, &ReceivePage::on_connection_removed);
}

void ReceivePage::on_connection_approved(QFrame *frame, const QString &clientName) {
    frame->setStyleSheet("QFrame {"
                         "  border: 5px solid #00ff00ff;"
                         "  border-radius: 6px;"
                         "  background: white;"
                         "}");

    auto *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *nameLabel = new QLabel(clientName);
    layout->addWidget(nameLabel);

    _clientListLayout->addWidget(frame);

    _scrollArea->show();
}

void ReceivePage::on_connection_removed(QFrame *frame) {
    _clientListLayout->removeWidget(frame);
    if (!_clientListLayout->count()) {
        _scrollArea->hide();
    }
}
