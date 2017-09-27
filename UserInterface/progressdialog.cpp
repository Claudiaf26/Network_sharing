#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QString path, bool sending,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog),
    closed(false)
{
    ui->setupUi(this);
    QObject::connect(ui->endButton, SIGNAL(pressed()), this, SLOT(close()));
    if(sending)
        ui->textLabel->setText("Stai inviando il file" + path);
    else
        ui->textLabel->setText("Stai ricevendo un file");
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgress(int value){
    ui->progressBar->setValue(value);
    if (value >= 100){
        ui->textLabel->setText("Il trasferimento del file è terminato");
        ui->endButton->setText("Esci");
    }
}


void ProgressDialog::closeEvent(QCloseEvent* cEvent){
    if (!closed){
        closed = true;
        cEvent->ignore();
    }
    else
        cEvent->accept();
}


