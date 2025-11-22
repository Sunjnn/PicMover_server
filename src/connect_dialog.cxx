#include "connect_dialog.hxx"

#include <qboxlayout.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qobject.h>
#include <qwidget.h>

#include "config.hxx"

ConnectDialog::ConnectDialog(const QString &clientName, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Connect Request");

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Client " + clientName + " wants to connect.\nEnter download path:");
    layout->addWidget(label);

    Config &config = Config::get_instance();

    _lineEdit = new QLineEdit(this);
    _lineEdit->setPlaceholderText(config.get_default_save_dir());
    layout->addWidget(_lineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString ConnectDialog::get_download_path() const {
    if (_lineEdit->text().isEmpty()) {
        Config &config = Config::get_instance();
        return config.get_default_save_dir();
    }
    return _lineEdit->text();
}
