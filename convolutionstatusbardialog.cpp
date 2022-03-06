#include "convolutionstatusbardialog.h"
#include "ui_convolutionstatusbardialog.h"

ConvolutionStatusBarDialog::ConvolutionStatusBarDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvolutionStatusBarDialog)
{
    ui->setupUi(this);


    ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid grey; border-radius: 10px; text-align: center;}");
    ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: #05B8CC; width: 1px;}");
}

ConvolutionStatusBarDialog::~ConvolutionStatusBarDialog()
{
    delete ui;
}
