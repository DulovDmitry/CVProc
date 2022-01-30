#include "convolutionsettingsdialog.h"
#include "ui_convolutionsettingsdialog.h"

ConvolutionSettingsDialog::ConvolutionSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvolutionSettingsDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Convolution settings");
}

ConvolutionSettingsDialog::~ConvolutionSettingsDialog()
{
    delete ui;
}
