#include "convolutemultiplefilesdialog.h"
#include "ui_convolutemultiplefilesdialog.h"

ConvoluteMultipleFilesDialog::ConvoluteMultipleFilesDialog(QList<QTreeWidgetItem*> treeItemsList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvoluteMultipleFilesDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Convolution");

    parentsTreeItems = treeItemsList;

    fillListWidget();

    ui->ConvoluteButton->setFocus();
    ui->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

ConvoluteMultipleFilesDialog::~ConvoluteMultipleFilesDialog()
{
    delete ui;
}

void ConvoluteMultipleFilesDialog::fillListWidget()
{
    foreach(QTreeWidgetItem *treeItem, parentsTreeItems)
    {
        QListWidgetItem *item = new QListWidgetItem(treeItem->text(0), ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        ui->listWidget->addItem(item);
        mapWithListItemsAndTreeItems.insert(item, treeItem);
    }
}

void ConvoluteMultipleFilesDialog::fillSelectedTreeItemsList()
{
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked)
        {
            selectedTreeItems.append(mapWithListItemsAndTreeItems.value(ui->listWidget->item(i)));
        }
    }
}

void ConvoluteMultipleFilesDialog::on_listWidget_itemChanged(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
        item->setBackground(QColor("#ffffb2"));
    else
        item->setBackground(QColor("#ffffff"));
}


void ConvoluteMultipleFilesDialog::on_SelectAllButton_clicked()
{
    if (ui->SelectAllButton->text() == "Select all")
    {
        for (int i = 0; i < ui->listWidget->count(); i++)
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        ui->SelectAllButton->setText("Deselect all");
    }
    else if (ui->SelectAllButton->text() == "Deselect all")
    {
        for (int i = 0; i < ui->listWidget->count(); i++)
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        ui->SelectAllButton->setText("Select all");
    }
}


void ConvoluteMultipleFilesDialog::on_ConvoluteButton_clicked()
{
    fillSelectedTreeItemsList();
    this->hide();
    emit convoluteButtonClicked(selectedTreeItems);
}

