#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QString path, QString user, bool sending,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog),
    m_closed(false)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->speedText->setText(0);
    ui->timeText->setText(0);
    QObject::connect(ui->endButton, SIGNAL(pressed()), this, SLOT(close()));
    if(sending)
        ui->textLabel->setText("Stai inviando " + path + " a " + user);
    else{
        ui->speedLabel->hide();ui->speedText->hide();
        ui->timeLabel->hide(); ui->timeText->hide();

        ui->textLabel->setText("Stai ricevendo " + path +" da " + user);
    }
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgress(int value, QString speed, QString time){
    ui->progressBar->setValue(value);
    ui->speedText->setText(speed);
    ui->timeText->setText(time);
    if (value >= 100){
        ui->textLabel->setText("Il trasferimento del file è terminato");
        ui->speedText->setText("N/A");
        ui->timeText->setText("N/A");
        ui->endButton->setText("Esci");
    }
}


void ProgressDialog::closeEvent(QCloseEvent* cEvent){
    if (!m_closed){
        m_closed = true;
        cEvent->ignore();
    }
    else
        cEvent->accept();
}


