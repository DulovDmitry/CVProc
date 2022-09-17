#ifndef CONVOLUTEMULTIPLEFILESDIALOG_H
#define CONVOLUTEMULTIPLEFILESDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include "cvcurvedata.h"

namespace Ui {
class ConvoluteMultipleFilesDialog;
}

class ConvoluteMultipleFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvoluteMultipleFilesDialog(QList<QTreeWidgetItem*> treeItemsList, QWidget *parent = nullptr);
    ~ConvoluteMultipleFilesDialog();

signals:
    void convoluteButtonClicked(QList<QTreeWidgetItem*>);

private slots:
    void on_listWidget_itemChanged(QListWidgetItem *item);
    void on_SelectAllButton_clicked();
    void on_ConvoluteButton_clicked();

private:
    // methods
    void fillListWidget();
    void fillSelectedTreeItemsList();

    QList<QTreeWidgetItem*> parentsTreeItems;
    QList<QTreeWidgetItem*> selectedTreeItems;
    QMap<QListWidgetItem*, QTreeWidgetItem*> mapWithListItemsAndTreeItems;

    Ui::ConvoluteMultipleFilesDialog *ui;
};

#endif // CONVOLUTEMULTIPLEFILESDIALOG_H
