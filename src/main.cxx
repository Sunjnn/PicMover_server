#include <qapplication.h>
#include <qdir.h>

#include "config.hxx"
#include "main_window.hxx"

void custom_message_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    static QFile file(QCoreApplication::applicationDirPath() + "/PicMover_log.txt");

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

    qInstallMessageHandler(custom_message_handler);

    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
