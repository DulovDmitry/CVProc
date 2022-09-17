#ifndef SMOOTHMULTIPLEFILESDIALOG_H
#define SMOOTHMULTIPLEFILESDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTreeWidget>

#include "cvcurvedata.h"
#include "graphdata.h"

namespace Ui {
class SmoothMultipleFilesDialog;
}

class SmoothMultipleFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SmoothMultipleFilesDialog(QHash<QTreeWidgetItem*, GraphData*> *_hash, QWidget *parent = nullptr);
    ~SmoothMultipleFilesDialog();

private slots:
    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_SelectAllButton_clicked();

    void on_comboBox_activated(int index);

private:
    //methods
    void fillComboBox();
    void fillListWidget();

    QVector<CVCurveData*> *CVCurves;
    QStringList *fileNamesList;

    Ui::SmoothMultipleFilesDialog *ui;
};

#endif // SMOOTHMULTIPLEFILESDIALOG_H
