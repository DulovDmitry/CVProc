#ifndef CONVOLUTIONSETTINGSDIALOG_H
#define CONVOLUTIONSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class ConvolutionSettingsDialog;
}

class ConvolutionSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvolutionSettingsDialog(QWidget *parent = nullptr);
    ~ConvolutionSettingsDialog();

private slots:
    void on_OkButton_clicked();

    void on_CancelButton_clicked();

signals:
    void okButtonClicked(int Ru, int Cd);

private:
    Ui::ConvolutionSettingsDialog *ui;
    QSettings *settings;
};

#endif // CONVOLUTIONSETTINGSDIALOG_H
