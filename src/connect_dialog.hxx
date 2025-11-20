#ifndef _CONNECT_DIALOG_HXX_
#define _CONNECT_DIALOG_HXX_

#include <qdialog.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qwidget.h>

class ConnectDialog : public QDialog {
public:
    ConnectDialog(const QString &clientName, QWidget *parent = nullptr);

    QString get_download_path() const;

private:
    QLineEdit *_lineEdit;
};

#endif
