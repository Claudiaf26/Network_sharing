#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QString t_path, QString t_username, bool t_sending,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog),
    m_closed(false)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    ui->progressBar->setValue(0);
    ui->speedText->setText("N/A");
    ui->timeText->setText("N/A");

    QObject::connect(ui->endButton, SIGNAL(pressed()), this, SLOT(close()));

    if(t_sending)
        ui->textLabel->setText("Stai inviando " + t_path + " a " + t_username);
    else{
        ui->textLabel->setText("Stai ricevendo " + t_path +" da " + t_username);
    }
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgress(int t_value, QString t_speed, QString t_time){
    ui->progressBar->setValue(t_value);
    ui->speedText->setText(t_speed);
    ui->timeText->setText(t_time);
    if (t_value >= 100){
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


