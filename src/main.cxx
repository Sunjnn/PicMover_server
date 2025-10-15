#include <QApplication>

#include "config.hxx"
#include "main_window.hxx"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
