#include <qapplication.h>
#include <qdir.h>
#include <qstandardpaths.h>

#include "config.hxx"
#include "main_window.hxx"

void custom_message_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QFile file(Config::get_log_file_path());

    if (!file.isOpen()) {
        file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type) {
    case QtDebugMsg:
        out << "[DEBUG] ";
        break;
    case QtInfoMsg:
        out << "[INFO] ";
        break;
    case QtWarningMsg:
        out << "[WARN] ";
        break;
    case QtCriticalMsg:
        out << "[CRIT] ";
        break;
    case QtFatalMsg:
        out << "[FATAL] ";
        break;
    }

    out << msg << "\n";
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    Config::set_config_file_path(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
                                 "/PicMover_config.ini");
    Config::set_log_file_path(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
                              "/PicMover_log.txt");

    qInstallMessageHandler(custom_message_handler);

    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
