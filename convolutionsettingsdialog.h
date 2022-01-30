#ifndef CONVOLUTIONSETTINGSDIALOG_H
#define CONVOLUTIONSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class ConvolutionSettingsDialog;
}

class ConvolutionSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvolutionSettingsDialog(QWidget *parent = nullptr);
    ~ConvolutionSettingsDialog();

private:
    Ui::ConvolutionSettingsDialog *ui;
};

#endif // CONVOLUTIONSETTINGSDIALOG_H
