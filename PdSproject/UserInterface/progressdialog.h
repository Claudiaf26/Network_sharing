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
    explicit ProgressDialog(QString path, bool sending, QWidget *parent = 0);
    ~ProgressDialog();

    bool isClosed() {return closed;}
    void setProgress(int);

    void closeEvent(QCloseEvent*);

private:
    Ui::ProgressDialog *ui;
    bool closed;
};

#endif // PROGRESSDIALOG_H
