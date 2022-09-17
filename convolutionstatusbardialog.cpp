#include "convolutionstatusbardialog.h"
#include "ui_convolutionstatusbardialog.h"

ConvolutionStatusBarDialog::ConvolutionStatusBarDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvolutionStatusBarDialog)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    setWindowTitle("Convolution progress");

    ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid black; border-radius: 10px; text-align: center;}");
    ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: #05B8CC; width: 1px;}");
}

ConvolutionStatusBarDialog::~ConvolutionStatusBarDialog()
{
    delete ui;
}

void ConvolutionStatusBarDialog::progressBarUpdate(int progress)
{
    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}

void ConvolutionStatusBarDialog::setFileName(QString filename)
{
    ui->labelWithFilename->setText("Convolution of " + filename);
}
