#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ProgressDialog;
}

//UI che mostra la barra progresso con informazioni sul trasferimento in corso
class ProgressDialog : public QDialog
{
    Q_OBJECT
private:
    Ui::ProgressDialog* ui;
    bool m_closed;

public:
    explicit ProgressDialog(QString path, QString user, bool sending, QWidget *parent = 0);
    ~ProgressDialog();

    bool isClosed() {return m_closed;}
    void setProgress(int, QString, QString);

    void closeEvent(QCloseEvent*);
};

#endif // PROGRESSDIALOG_H
