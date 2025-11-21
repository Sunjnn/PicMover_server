#ifndef _RECEIVE_PAGE_HXX_
#define _RECEIVE_PAGE_HXX_

#include <qboxlayout.h>
#include <qlabel.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ReceivePage : public QWidget {
    Q_OBJECT

private slots:
    void on_client_changed();

public:
    ReceivePage(const QString &serverName, QWidget *parent = nullptr);

    static void clean_layout(QLayout *layout);
    void update_client_list();

private:
    QVBoxLayout *_layout;
    QLabel *_serverNameLabel;
    QScrollArea *_scrollArea;
    QVBoxLayout *_clientListLayout;
};

#endif // _RECEIVE_PAGE_HXX_
