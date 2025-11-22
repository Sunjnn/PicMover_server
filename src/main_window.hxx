#ifndef _MAIN_WINDOW_HXX_
#define _MAIN_WINDOW_HXX_

#include <qboxlayout.h>
#include <qhostaddress.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <qobject.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "http_server.hxx"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void on_connect_request(const QString &clientName, HttpServer::ConnectId connectId);

private:
    void setup_ui();
};

#endif // _MAIN_WINDOW_HXX_
