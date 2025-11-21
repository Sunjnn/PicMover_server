#ifndef _MAIN_WINDOW_HXX_
#define _MAIN_WINDOW_HXX_

#include <qhostaddress.h>
#include <qmainwindow.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "http_server.hxx"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void on_connect_request(QString clientName, HttpServer::ConnectId connectId);

private:
    void setup_ui();
};

#endif // _MAIN_WINDOW_HXX_
