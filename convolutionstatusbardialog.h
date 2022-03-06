#ifndef CONVOLUTIONSTATUSBARDIALOG_H
#define CONVOLUTIONSTATUSBARDIALOG_H

#include <QDialog>

namespace Ui {
class ConvolutionStatusBarDialog;
}

class ConvolutionStatusBarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvolutionStatusBarDialog(QWidget *parent = nullptr);
    ~ConvolutionStatusBarDialog();
    void updateProgressBar(int progress);
    void setFileName(QString filename);

private:
    Ui::ConvolutionStatusBarDialog *ui;
};

#endif // CONVOLUTIONSTATUSBARDIALOG_H
