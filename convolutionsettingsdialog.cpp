#include "convolutionsettingsdialog.h"
#include "ui_convolutionsettingsdialog.h"

ConvolutionSettingsDialog::ConvolutionSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvolutionSettingsDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Convolution settings");

    settings = new QSettings("ORG335a", "CVProc", this);
    ui->Ru_spinBox->setValue(settings->value("Ru_VALUE", 0).toInt());
    ui->Cd_spinBox->setValue(settings->value("Cd_VALUE", 0).toInt());
}

ConvolutionSettingsDialog::~ConvolutionSettingsDialog()
{
    delete ui;
}

void ConvolutionSettingsDialog::on_OkButton_clicked()
{
    this->hide();
    emit okButtonClicked(ui->Ru_spinBox->value(), ui->Cd_spinBox->value());
}


void ConvolutionSettingsDialog::on_CancelButton_clicked()
{
    this->hide();
}

