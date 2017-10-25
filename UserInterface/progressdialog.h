#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProgressDialog(QString path, QString user, bool sending, QWidget *parent = 0);
    ~ProgressDialog();

    bool isClosed() {return m_closed;}
    void setProgress(int, QString, QString);

    void closeEvent(QCloseEvent*);

private:
    Ui::ProgressDialog* ui;
    bool m_closed;
};

#endif // PROGRESSDIALOG_H
