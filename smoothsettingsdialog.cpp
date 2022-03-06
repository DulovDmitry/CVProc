#include "smoothsettingsdialog.h"
#include "ui_smoothsettingsdialog.h"

SmoothSettingsDialog::SmoothSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SmoothSettingsDialog)
{
    ui->setupUi(this);


    this->setWindowTitle("Smooth parameters");

    setInitialParameters();
}

SmoothSettingsDialog::~SmoothSettingsDialog()
{
    delete ui;
}

void SmoothSettingsDialog::on_pol_order_spinBox_valueChanged(int arg1)
{
    emit sendFilterParameters(ui->m_spinBox->value(), ui->pol_order_spinBox->value());
}

void SmoothSettingsDialog::on_m_spinBox_valueChanged(int arg1)
{
    emit sendFilterParameters(ui->m_spinBox->value(), ui->pol_order_spinBox->value());
}

void SmoothSettingsDialog::on_OK_Button_clicked()
{
    emit ButtonClickedSignal(1);
    //setInitialParameters();
}

void SmoothSettingsDialog::on_Cancel_Button_clicked()
{
    emit ButtonClickedSignal(0);
    //setInitialParameters();
}

void SmoothSettingsDialog::setInitialParameters()
{
    ui->m_spinBox->setValue(15);
    ui->pol_order_spinBox->setValue(2);
}

void SmoothSettingsDialog::closeEvent(QCloseEvent *event)
{
    on_Cancel_Button_clicked();
}


