#ifndef _RECEIVE_PAGE_HXX_
#define _RECEIVE_PAGE_HXX_

#include <qboxlayout.h>
#include <qlabel.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ReceivePage : public QWidget {
    Q_OBJECT

public:
    ReceivePage(const QString &serverName, QWidget *parent = nullptr);

private slots:
    void on_connection_approved(QFrame *frame, const QString &clientName);
    void on_connection_removed(QFrame *frame);

private:
    QVBoxLayout *_layout;
    QLabel *_serverNameLabel;
    QScrollArea *_scrollArea;
    QVBoxLayout *_clientListLayout;
};

#endif // _RECEIVE_PAGE_HXX_
