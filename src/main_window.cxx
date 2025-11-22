#include "main_window.hxx"

#include <memory>

#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qhostaddress.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "config.hxx"
#include "connect_dialog.hxx"
#include "http_server.hxx"
#include "receive_page.hxx"

using std::make_unique;

using Qt::AlignCenter;
using Qt::AlignTop;
using Qt::QueuedConnection;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setup_ui();

    HttpServer &httpServer = HttpServer::get_instance();
    connect(&httpServer, &HttpServer::signal_connect_request, this, &MainWindow::on_connect_request, QueuedConnection);
}

void MainWindow::setup_ui() {
    const Config &config = Config::get_instance();

    setWindowTitle(APPLICATION_NAME);
    resize(800, 500);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    // Set up main layout
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Left menu
    auto *leftMenu = new QFrame();
    leftMenu->setFixedWidth(180);
    leftMenu->setStyleSheet("background-color: #e8f0ed;");

    auto *menuLayout = new QVBoxLayout(leftMenu);
    menuLayout->setAlignment(AlignTop);
    menuLayout->setSpacing(20);
    menuLayout->setContentsMargins(20, 30, 20, 0);

    auto *title = new QLabel("PicMover");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");

    auto *btnReceive = new QPushButton("Receive");
    auto *btnSettings = new QPushButton("Settings");

    for (auto *btn : {btnReceive, btnSettings}) {
        btn->setStyleSheet("QPushButton {"
                           "  font-size: 16px;"
                           "  text-align: left;"
                           "  padding: 6px 8px;"
                           "  border: none;"
                           "  background: transparent;"
                           "}"
                           "QPushButton:hover { background-color: #dfe9e6; border-radius: 8px; }");
    }

    menuLayout->addWidget(title);
    menuLayout->addWidget(btnReceive);
    menuLayout->addWidget(btnSettings);
    menuLayout->addSpacing(30);
    menuLayout->addStretch();

    // Right Area with Stacked Pages
    auto *stackedWidget = new QStackedWidget();

    // Receive Page
    auto *receivePage = new ReceivePage(config.get_server_name());

    // Settings Page
    auto *settingsPage = new QWidget();
    auto *settingsLayout = new QVBoxLayout(settingsPage);
    settingsLayout->setAlignment(AlignTop);
    settingsLayout->setContentsMargins(40, 40, 40, 40);
    settingsLayout->setSpacing(30);

    auto *settingsTitle = new QLabel("Software Settings");
    settingsTitle->setStyleSheet("font-size: 22px; font-weight: bold;");

    auto *defaultSaveRowLayout = new QHBoxLayout();
    defaultSaveRowLayout->setSpacing(10);

    auto *defaultSaveDirLabel = new QLabel("Default Save Location:");
    defaultSaveDirLabel->setStyleSheet("font-size: 16px;");

    auto *defaultSaveDirEdit = new QLineEdit();
    defaultSaveDirEdit->setPlaceholderText(config.get_default_save_dir());
    defaultSaveDirEdit->setStyleSheet(
        "QLineEdit { font-size: 15px; padding: 6px; border: 1px solid #ccc; border-radius: 6px; }");

    auto *chooseButton = new QPushButton("Browse");
    chooseButton->setStyleSheet(
        "QPushButton { font-size: 15px; padding: 6px 12px; border-radius: 6px; background-color: #dfe9e6; }"
        "QPushButton:hover { background-color: #cfe0dc; }");

    defaultSaveRowLayout->addWidget(defaultSaveDirLabel);
    defaultSaveRowLayout->addWidget(defaultSaveDirEdit, 1);
    defaultSaveRowLayout->addWidget(chooseButton);

    settingsLayout->addWidget(settingsTitle);
    settingsLayout->addSpacing(20);
    settingsLayout->addLayout(defaultSaveRowLayout);
    settingsLayout->addStretch();

    // Add pages to stack
    stackedWidget->addWidget(receivePage);  // index 0
    stackedWidget->addWidget(settingsPage); // index 1

    // --- Combine ---
    mainLayout->addWidget(leftMenu);
    mainLayout->addWidget(stackedWidget);

    // --- Connect Buttons ---
    connect(btnReceive, &QPushButton::clicked, [stackedWidget]() { stackedWidget->setCurrentIndex(0); });
    connect(btnSettings, &QPushButton::clicked, [stackedWidget]() { stackedWidget->setCurrentIndex(1); });
    connect(chooseButton, &QPushButton::clicked, [defaultSaveDirEdit, this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Save Location");
        if (!dir.isEmpty()) {
            defaultSaveDirEdit->setText(dir);

            Config &config = Config::get_instance();
            config.set_default_save_dir(dir);
        }
    });
}

void MainWindow::on_connect_request(const QString &clientName, HttpServer::ConnectId connectId) {
    ConnectDialog connectDialog(clientName, this);
    HttpServer &httpServer = HttpServer::get_instance();
    if (connectDialog.exec() == QDialog::Accepted) {
        const QString &path = connectDialog.get_download_path();
        httpServer.set_is_approved(connectId, true, path);
    }
    else {
        httpServer.set_is_approved(connectId, false, "");
    }
}
