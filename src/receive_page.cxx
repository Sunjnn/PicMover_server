#include "receive_page.hxx"

#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
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
    _layout->addWidget(scrollArea, 1);
    scrollArea->hide();

    _scrollArea = scrollArea;
    _clientListLayout = scrollLayout;
}

void ReceivePage::clean_layout(QLayout *layout) {
    if (!layout)
        return;

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (auto *w = item->widget()) {
            w->deleteLater();
        }
        else if (auto *childLayout = item->layout()) {
            clean_layout(childLayout);
            childLayout->deleteLater();
        }
        delete item;
    }
}

void ReceivePage::update_client_list() {
    clean_layout(_clientListLayout);

    HttpServer &httpServer = HttpServer::get_instance();
    if (!httpServer.get_connection_count()) {
        _serverNameLabel->setAlignment(AlignCenter);
        _scrollArea->hide();
        return;
    }

    _serverNameLabel->setAlignment(Qt::AlignTop);
    _scrollArea->show();

    for (auto iter = httpServer.get_connection_begin(); iter != httpServer.get_connection_end(); ++iter) {
        auto *frame = new QFrame();
        frame->setStyleSheet("QFrame {"
                             "  border: 5px solid #000000ff;"
                             "  border-radius: 6px;"
                             "  background: white;"
                             "}");

        auto *layout = new QVBoxLayout(frame);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        auto *nameLabel = new QLabel(iter->second.clientName, frame);

        layout->addWidget(nameLabel);

        _clientListLayout->addWidget(frame);
    }
    _clientListLayout->addStretch();
}

void ReceivePage::on_client_changed() {
    update_client_list();
}
