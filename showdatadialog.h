#ifndef SHOWDATADIALOG_H
#define SHOWDATADIALOG_H

#include <QDialog>
#include <QClipboard>

#include <cvcurvedata.h>

namespace Ui {
class ShowDataDialog;
}

class ShowDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowDataDialog(QWidget *parent = nullptr);
    ~ShowDataDialog();

    void setCVData(CVCurveData *incomingCVData);
    void fillTable();

private slots:

    void on_CopyButton_clicked();

private:
    Ui::ShowDataDialog *ui;
    CVCurveData *CVData;
    QSettings *settings;
};

#endif // SHOWDATADIALOG_H
