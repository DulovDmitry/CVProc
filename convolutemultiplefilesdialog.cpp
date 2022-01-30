#include "convolutemultiplefilesdialog.h"
#include "ui_convolutemultiplefilesdialog.h"

ConvoluteMultipleFilesDialog::ConvoluteMultipleFilesDialog(QStringList *namesList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvoluteMultipleFilesDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Convolution");

    fileNamesList = namesList;

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
    for (int i = 0; i < fileNamesList->size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(fileNamesList->at(i), ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        ui->listWidget->addItem(item);
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

