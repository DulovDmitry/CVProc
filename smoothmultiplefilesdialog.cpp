#include "smoothmultiplefilesdialog.h"
#include "ui_smoothmultiplefilesdialog.h"

SmoothMultipleFilesDialog::SmoothMultipleFilesDialog(QVector<CVCurveData *> *vector, QStringList *fileNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SmoothMultipleFilesDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Smoothing");

    CVCurves = new QVector<CVCurveData*>;
    CVCurves = vector;
    fileNamesList = fileNames;

    fillComboBox();
    fillListWidget();
}

SmoothMultipleFilesDialog::~SmoothMultipleFilesDialog()
{
    delete ui;
}

void SmoothMultipleFilesDialog::fillComboBox()
{
    bool IvsE_isAbaliable = false;
    bool EvsT_isAbaliable = false;
    bool IvsE_sd_isAbaliable = false;
    bool IvsE_si_isAbaliable = false;

    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
    {
        (((*it)->EIsAvaliable() && (*it)->IIsAvaliable()) || IvsE_isAbaliable) ? IvsE_isAbaliable = true : IvsE_isAbaliable = false;
        (((*it)->EIsAvaliable() && (*it)->TIsAvaliable()) || EvsT_isAbaliable) ? EvsT_isAbaliable = true : EvsT_isAbaliable = false;
        ((*it)->IsdIsAvaliable() || IvsE_sd_isAbaliable) ? IvsE_sd_isAbaliable = true : IvsE_sd_isAbaliable = false;
        ((*it)->IsiIsAvaliable() || IvsE_si_isAbaliable) ? IvsE_si_isAbaliable = true : IvsE_si_isAbaliable = false;
    }

    if (IvsE_isAbaliable)
        ui->comboBox->addItem(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));

    if (EvsT_isAbaliable)
        ui->comboBox->addItem(CVCurveData::PlotTypes.at(CVCurveData::E_vs_T));

    if (IvsE_sd_isAbaliable)
        ui->comboBox->addItem(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff));

    if (IvsE_si_isAbaliable)
        ui->comboBox->addItem(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint));
}

void SmoothMultipleFilesDialog::fillListWidget()
{
    short plotType = CVCurveData::PlotTypes.indexOf(ui->comboBox->currentText());
    if (plotType == -1) return;


    for (int i = 0; i < fileNamesList->size(); i++)
    {
        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        {
            if (fileNamesList->at(i) != (*it)->fileName())
                continue;

            switch (plotType)
            {
            case 0: // I vs E
                if ((*it)->EIsAvaliable() && (*it)->IIsAvaliable())
                {
                    QListWidgetItem *item = new QListWidgetItem((*it)->fileName(), ui->listWidget);
                    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                    item->setCheckState(Qt::Unchecked);
                    ui->listWidget->addItem(item);
                }
                break;

            case 1: // E vs t
                if ((*it)->EIsAvaliable() && (*it)->TIsAvaliable())
                {
                    QListWidgetItem *item = new QListWidgetItem((*it)->fileName(), ui->listWidget);
                    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                    item->setCheckState(Qt::Unchecked);
                    ui->listWidget->addItem(item);
                }
                break;

            case 2: // I (semiintegral) vs E
                if ((*it)->IsiIsAvaliable())
                {
                    QListWidgetItem *item = new QListWidgetItem((*it)->fileName(), ui->listWidget);
                    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                    item->setCheckState(Qt::Unchecked);
                    ui->listWidget->addItem(item);
                }
                break;

            case 3: // I (semidifferential) vs E
                if ((*it)->IsdIsAvaliable())
                {
                    QListWidgetItem *item = new QListWidgetItem((*it)->fileName(), ui->listWidget);
                    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                    item->setCheckState(Qt::Unchecked);
                    ui->listWidget->addItem(item);
                }
                break;
            }
        }
    }
}

void SmoothMultipleFilesDialog::on_listWidget_itemChanged(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
        item->setBackground(QColor(255, 255, 178));
    else
        item->setBackground(QColor(255, 255, 255));
}

void SmoothMultipleFilesDialog::on_SelectAllButton_clicked()
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

void SmoothMultipleFilesDialog::on_comboBox_activated(int index)
{
    ui->listWidget->clear();
    fillListWidget();
}

