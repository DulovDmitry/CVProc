#ifndef CONVOLUTEMULTIPLEFILESDIALOG_H
#define CONVOLUTEMULTIPLEFILESDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

#include "cvcurvedata.h"

namespace Ui {
class ConvoluteMultipleFilesDialog;
}

class ConvoluteMultipleFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvoluteMultipleFilesDialog(QStringList *namesList, QWidget *parent = nullptr);
    ~ConvoluteMultipleFilesDialog();

private slots:
    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_SelectAllButton_clicked();

private:
    // methods
    void fillListWidget();

    QStringList *fileNamesList;

    Ui::ConvoluteMultipleFilesDialog *ui;
};

#endif // CONVOLUTEMULTIPLEFILESDIALOG_H
