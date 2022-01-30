#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showMaximized();
    this->setWindowTitle("CVProc");
    this->setWindowIcon(QIcon(":/new/prefix1/mainIcon.ico"));

    settings = new QSettings("ORG335a", "CVProc", this);
    loadSettings();
    
    // Appearance configuration
    ui->checkBox_particularCycle->setEnabled(false);
    ui->spinBox_numberOfCycle->setEnabled(false);
    ui->CrossButton->setCheckable(true);
    ui->DeleteDataButton->setEnabled(false);
    ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid grey; border-radius: 10px; text-align: center;}");
    ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: #05B8CC; width: 1px;}");

    // Dialog with general settings
    generalSettingsDialog = new GeneralSettingsDialog(this);
    generalSettingsDialog->setWindowModality(Qt::WindowModal);
    connect(generalSettingsDialog, SIGNAL(applyButtonClicked()),
            this, SLOT(applyGraphicalSettings()));

    // Dialog for smooth's settings configuration
    smoothSettingsDialog = new SmoothSettingsDialog(this);
    smoothSettingsDialog->setWindowModality(Qt::NonModal);
    connect(smoothSettingsDialog, SIGNAL(sendFilterParameters(int,int)),
            this, SLOT(SavGolFilterApply(int,int)));
    connect(smoothSettingsDialog, SIGNAL(ButtonClickedSignal(bool)),
            this, SLOT(smoothSettingsDialogWasClosed(bool)));

    // Dialog for convolution's settings configuration
    convolutionSettingsDialog = new ConvolutionSettingsDialog(this);

    // plot preferences
    customPlot = new QCustomPlot(this);
    CVCurves = new QVector<CVCurveData*>;
    customPlot->setMinimumHeight(300);
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    customPlot->xAxis->setLabel("E, V");
    customPlot->yAxis->setLabel("I, mA");
    ui->gridLayout->addWidget(customPlot, 1, 0, 1, 1);
    customPlot->setFocus();
    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    customPlot->setInteraction(QCP::iSelectPlottables);
    selectionRect = new QCPSelectionRect(customPlot);
    //selectionRect->setVisible(false);

    // curves preferences
    curvePlot = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    curvePlot->setSelectable(QCP::stMultipleDataRanges);
    smoothedCurvePlot = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    applyGraphicalSettings();

    // connect QCustomPlot signals and MainWindow slots
    connect(customPlot, SIGNAL(mouseDoubleClick(QMouseEvent*)),
            this, SLOT(plotDoubleClicked(QMouseEvent*)));
    connect(customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
            this, SLOT(axisLabelDoubleClicked(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));
    connect(customPlot, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(plotContextMenuRequested(QPoint)));
    connect(customPlot, SIGNAL(mouseMove(QMouseEvent*)),
            this, SLOT(mousePositionChanged(QMouseEvent*)));
    connect(customPlot, SIGNAL(mousePress(QMouseEvent*)),
            this, SLOT(mousePressedOnGraph(QMouseEvent*)));
    connect(customPlot, SIGNAL(mouseRelease(QMouseEvent*)),
            this, SLOT(mouseReleasedOnGraph(QMouseEvent*)));
    connect(curvePlot, SIGNAL(selectionChanged(QCPDataSelection)),
            this, SLOT(graphSelectionChanged(QCPDataSelection)));

    // Cross for graph
    cross_1_vert = new QCPItemStraightLine(customPlot);
    cross_1_hor = new QCPItemStraightLine(customPlot);
    cross_1_vert->point1->setCoords(0, 0);
    cross_1_vert->point2->setCoords(0, 1);
    cross_1_hor->point1->setCoords(0, 0);
    cross_1_hor->point2->setCoords(1, 0);
    cross_2_vert = new QCPItemStraightLine(customPlot);
    cross_2_hor = new QCPItemStraightLine(customPlot);
    cross_2_vert->setPen(QPen(QColor(255,0,0)));
    cross_2_hor->setPen(QPen(QColor(255,0,0)));
    cross_2_vert->point1->setCoords(0, 0);
    cross_2_vert->point2->setCoords(0, 1);
    cross_2_hor->point1->setCoords(0, 0);
    cross_2_hor->point2->setCoords(1, 0);

    // treeWidget preferences
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList() << "Filename" << "Scan rate" << "Points");
    ui->treeWidget->setColumnWidth(0, 193);
    ui->treeWidget->setColumnWidth(1, 60);
    ui->treeWidget->setColumnWidth(2, 45);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget->setDragEnabled(true);
    ui->treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(treeContextMenuRequested(QPoint)));

    // Context menu for plot
    plotContextMenu = new QMenu(this);
    openGraphProperties = new QAction("Graph properties", this); // добавить connect
    savePDF = new QAction("Save plot as .pdf file", this);
    saveJPEG = new QAction("Save plot as .jpg file", this);
    savePNG = new QAction("Save plot as .png file", this);
    exportTXT = new QAction("Export as .txt file", this);

    connect(openGraphProperties, SIGNAL(triggered()),
            this, SLOT(on_actionSettings_triggered()));
    connect(savePDF, SIGNAL(triggered()),
            this, SLOT(savePlotAsPDF()));
    connect(saveJPEG, SIGNAL(triggered()),
            this, SLOT(savePlotAsJPEG()));
    connect(savePNG, SIGNAL(triggered()),
            this, SLOT(savePlotAsPNG()));
    connect(exportTXT, SIGNAL(triggered()),
            this, SLOT(exportPlotAsTXT()));
    initializePlotContextMenu();

    // Context menu for tree widget
    treeContextMenu = new QMenu(this);
    renameItem = new QAction("Rename", this);
    showInExplorer = new QAction("Show in explorer", this);
    convolute = new QAction("Convolute", this);
    deleteItem = new QAction("Delete", this);
    deleteAllItems = new QAction("Delete all", this);

    connect(renameItem, SIGNAL(triggered()),
            this, SLOT(renameTableWidgetItem()));
    connect(convolute, SIGNAL(triggered()),
            this, SLOT(on_ConvolutionButton_clicked()));
    connect(deleteItem, SIGNAL(triggered()),
            this, SLOT(deleteSingleTreeWidgetItem()));
    connect(deleteAllItems, SIGNAL(triggered()),
            this, SLOT(clearTreeWidget()));
    initializeTreeContextMenu();

}

MainWindow::~MainWindow()
{
    saveSettings();
    delete CVCurves;
    delete customPlot;
    delete ui;
}

void MainWindow::drawPlot(CVCurveData *CVData, QString plotType)
{
    if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E))
    {
        curvePlot->setData(CVData->E(), CVData->I());
        customPlot->xAxis->setLabel("E, V");
        customPlot->yAxis->setLabel("I, mA");
    }
    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T))
    {
        curvePlot->setData(CVData->T(), CVData->E());
        customPlot->xAxis->setLabel("t, s");
        customPlot->yAxis->setLabel("E, V");
    }
    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint))
    {
        curvePlot->setData(CVData->Esmoothed(), CVData->Isi());
        customPlot->xAxis->setLabel("E, V");
        customPlot->yAxis->setLabel("I (convoluted)");
    }
    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff))
    {
        curvePlot->setData(CVData->Esmoothed(), CVData->Isd());
        customPlot->xAxis->setLabel("E, V");
        customPlot->yAxis->setLabel("I (semidifferential)");
    }
    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_smoothed))
    {
        curvePlot->setData(CVData->E(), CVData->Ismoothed());
        customPlot->xAxis->setLabel("E, V");
        customPlot->yAxis->setLabel("I, mA");
    }
    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T_smoothed))
    {
        curvePlot->setData(CVData->T(), CVData->Esmoothed());
        customPlot->xAxis->setLabel("t, s");
        customPlot->yAxis->setLabel("E, V");
    }

    customPlot->xAxis->setRange(CVData->axisRanges.value(plotType).x_min(), CVData->axisRanges.value(plotType).x_max());
    customPlot->yAxis->setRange(CVData->axisRanges.value(plotType).y_min(), CVData->axisRanges.value(plotType).y_max());
    customPlot->replot();
}

void MainWindow::on_actionImport_ASCII_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Add files", lastOpenDir, "TXT file; *.txt");

    if (files.isEmpty()) return;

    for (int i = 0; i < files.size(); i++)
    {
        QFile file(files.at(i), this);
        QFileInfo fi(files.at(i));
        lastOpenDir = fi.absolutePath();

        if (file.open(QIODevice::ReadOnly))
        {
            CVCurves->append(new CVCurveData(files.at(i)));

            QTextStream streamFromFile(&file);
            while (!file.atEnd())
                CVCurves->last()->addLineInFileBody(streamFromFile.readLine());

            CVCurves->last()->processFileBody();

            connect(CVCurves->last(), SIGNAL(progressWasChanged(int)),
                    this, SLOT(progressBarUpdate(int)));

            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, CVCurves->last()->fileName());
            item->setText(1, QString::number(CVCurves->last()->scanRate()));
            item->setText(2, QString::number(CVCurves->last()->E().size()));
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

            if (CVCurves->last()->EIsAvaliable() && CVCurves->last()->IIsAvaliable())
            {
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
                item->addChild(child);
            }
            if (CVCurves->last()->EIsAvaliable() && CVCurves->last()->TIsAvaliable())
            {
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setText(0, CVCurveData::PlotTypes.at(CVCurveData::E_vs_T));
                item->addChild(child);
            }

            ui->treeWidget->addTopLevelItem(item);

            ui->plainTextEdit->appendPlainText("The file " + file.fileName() + " has been imported\n"
                                               "Size: " + QString::number(file.size()) + " bytes\n"
                                               "Number of lines: " + QString::number(CVCurves->last()->numberOfLinesInFileBody()) + "\n"
                                               "Number of data points: " + QString::number(CVCurves->last()->sizeOfE()) + "\n"
                                               "Avaliable parameters: " + CVCurves->last()->avaliableParameters() + "\n");

            curvePlot->setData(CVCurves->last()->E(), CVCurves->last()->I());
            customPlot->xAxis->setRange(CVCurves->last()->ERangeMin(), CVCurves->last()->ERangeMax());
            customPlot->yAxis->setRange(CVCurves->last()->IRangeMin(), CVCurves->last()->IRangeMax());
            customPlot->replot();
        }
    }

    int itemsCount = ui->treeWidget->topLevelItemCount() - 1;
    ui->treeWidget->topLevelItem(itemsCount)->setSelected(true);
    ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(itemsCount));
    ui->treeWidget->setFocus();
}

void MainWindow::initializePlotContextMenu()
{
    plotContextMenu->addAction(openGraphProperties);
    plotContextMenu->addSeparator();
    plotContextMenu->addAction(savePDF);
    plotContextMenu->addAction(saveJPEG);
    plotContextMenu->addAction(savePNG);
    plotContextMenu->addSeparator();
    plotContextMenu->addAction(exportTXT);
}

void MainWindow::SavGolFilterApply(int m, int pol_order)
{
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    CVCurveData::PlotType plotType = CVCurveData::I_vs_E;
    if (!currentItem->childCount())
    {
        plotType = static_cast<CVCurveData::PlotType>(CVCurveData::PlotTypes.indexOf(currentItem->text(0)));
        currentItem = currentItem->parent();
    }

    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
    {
        if (currentItem->text(0) == (*it)->fileName())
        {
            (*it)->SavGolFilter(m, pol_order, plotType);

            if (plotType == CVCurveData::I_vs_E)
                smoothedCurvePlot->setData((*it)->E(), (*it)->Ismoothed());
            else if (plotType == CVCurveData::E_vs_T)
                smoothedCurvePlot->setData((*it)->T(), (*it)->Esmoothed());

            customPlot->replot();
            break;
        }
    }
}

void MainWindow::saveSettings()
{
    settings->setValue("LAST_SAVE_DIR", lastSaveDir);
    settings->setValue("LAST_OPEN_DIR", lastOpenDir);
}

void MainWindow::loadSettings()
{
    lastSaveDir = settings->value("LAST_SAVE_DIR", "C:\\").toString();
    lastOpenDir = settings->value("LAST_OPEN_DIR", "C:\\").toString();
}

void MainWindow::on_DragAndDropButton_clicked()
{
    deactivateAllControlButtons();
    ui->DragAndDropButton->setChecked(true);

    customPlot->setInteraction(QCP::iRangeDrag);
    customPlot->setInteraction(QCP::iRangeZoom);
}

void MainWindow::on_DefaultViewButton_clicked()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();

    if (currentItem->childCount())
    {
        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        {
            if (currentItem->text(0) == (*it)->fileName())
            {
                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->yAxis->setRange((*it)->IRangeMin(), (*it)->IRangeMax());
                customPlot->replot();
                break;
            }
        }
        return;
    }

    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        if ((*it)->fileName() == currentItem->parent()->text(0))
        {
            if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E))
            {
                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->yAxis->setRange((*it)->IRangeMin(), (*it)->IRangeMax());
                customPlot->replot();
                break;
            }
            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T))
            {
                customPlot->xAxis->setRange((*it)->TRangeMin(), (*it)->TRangeMax());
                customPlot->yAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->replot();
                break;
            }
            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint))
            {
                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->yAxis->setRange((*it)->IsiRangeMin(), (*it)->IsiRangeMax());
                customPlot->replot();
                break;
            }
            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff))
            {
                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->yAxis->setRange((*it)->IsdRangeMin(), (*it)->IsdRangeMax());
                customPlot->replot();
                break;
            }
            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_smoothed))
            {
                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->yAxis->setRange((*it)->IRangeMin(), (*it)->IRangeMax());
                customPlot->replot();
                break;
            }
            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T_smoothed))
            {
                customPlot->xAxis->setRange((*it)->TRangeMin(), (*it)->TRangeMax());
                customPlot->yAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
                customPlot->replot();
                break;
            }
        }
}

void MainWindow::treeWidgetItemPressed(QTreeWidgetItem *item, int column)
{
    ui->ConvolutionButton->setEnabled(false);
    fileNameBuffer = item->text(0);

    if (item->childCount())
    {
        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        {
            if (item->text(0) == (*it)->fileName())
            {
                drawPlot(*it, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
                ui->ConvolutionButton->setEnabled(true);
                break;
            }
        }
        return;
    }

    QString parentName = item->parent()->text(0);
    QString itemName = item->text(0);

    if (itemName.contains(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E))) ui->ConvolutionButton->setEnabled(true);

    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        if ((*it)->fileName() == parentName) drawPlot(*it, itemName);
}

void MainWindow::updateFileNamesListFromTreeWidget()
{
    fileNamesListFromTreeWidget.clear();
    int count = ui->treeWidget->topLevelItemCount();
    for (int i = 0; i < count; i++)
        fileNamesListFromTreeWidget.append(ui->treeWidget->topLevelItem(i)->text(0));
}

void MainWindow::treeContextMenuRequested(QPoint pos)
{
    if (ui->treeWidget->itemAt(pos) == NULL)
    {
        renameItem->setDisabled(true);
        showInExplorer->setDisabled(true);
        exportTXT->setDisabled(true);
        deleteItem->setDisabled(true);
    }
    else
    {
        renameItem->setDisabled(false);
        showInExplorer->setDisabled(false);
        exportTXT->setDisabled(false);
        deleteItem->setDisabled(false);

        if (!ui->treeWidget->itemAt(pos)->childCount())
        {
            renameItem->setDisabled(true);
            showInExplorer->setDisabled(true);
        }
    }
    convolute->setEnabled(ui->ConvolutionButton->isEnabled());

    pos.setY(pos.y() + ui->treeWidget->header()->size().height());
    treeContextMenu->popup(ui->treeWidget->mapToGlobal(pos));
}

void MainWindow::progressBarUpdate(int progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::plotDoubleClicked(QMouseEvent* event)
{
    on_DefaultViewButton_clicked();
}

void MainWindow::axisLabelDoubleClicked(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent* event)
{
    if (part == QCPAxis::spAxisLabel)
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "CVProc", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
        if (ok && !newLabel.isEmpty())
        {
            axis->setLabel(newLabel);
            customPlot->replot();
        }
    }
}

void MainWindow::graphSelectionChanged(QCPDataSelection selection)
{
    ui->label_SelectedPointsCount->setText(QString::number(selection.dataRangeCount()) + "/" + QString::number(selection.dataPointCount()));

    if (selection.dataPointCount())
        ui->DeleteDataButton->setEnabled(true);
    else
        ui->DeleteDataButton->setEnabled(false);
}

void MainWindow::plotContextMenuRequested(QPoint pos)
{
    plotContextMenu->popup(customPlot->mapToGlobal(pos));
}

void MainWindow::initializeTreeContextMenu()
{
    treeContextMenu->addAction(renameItem);
    treeContextMenu->addAction(showInExplorer);
    treeContextMenu->addAction(exportTXT);
    treeContextMenu->addAction(convolute);
    treeContextMenu->addAction(deleteItem);
    treeContextMenu->addAction(deleteAllItems);
}

void MainWindow::savePlotAsPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as PDF", lastSaveDir, pdfFilter);

    if (fileName.isEmpty()) return;

    QFileInfo fi(fileName);
    lastSaveDir = fi.absolutePath();

    if (customPlot->savePdf(fileName))
        ui->plainTextEdit->appendPlainText(QString("The file %1 has been saved").arg(fileName));
    else
        QMessageBox::warning(this, "Error", "Something went wrong");
}

void MainWindow::savePlotAsJPEG()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as JPEG", lastSaveDir, jpegFilter);

    if (fileName.isEmpty()) return;

    QFileInfo fi(fileName);
    lastSaveDir = fi.absolutePath();

    if (customPlot->saveJpg(fileName, 0, 0, 3.0, -1, 300))
        ui->plainTextEdit->appendPlainText(QString("The file %1 has been saved").arg(fileName));
    else
        QMessageBox::warning(this, "Error", "Something went wrong");
}

void MainWindow::savePlotAsPNG()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as PNG", lastSaveDir, pngFilter);

    if (fileName.isEmpty()) return;

    QFileInfo fi(fileName);
    lastSaveDir = fi.absolutePath();

    if (customPlot->savePng(fileName, 0, 0, 3.0, -1, 300))
        ui->plainTextEdit->appendPlainText(QString("The file %1 has been saved").arg(fileName));
    else
        QMessageBox::warning(this, "Error", "Something went wrong");
}

void MainWindow::exportPlotAsTXT()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save as TXT", lastSaveDir, txtFilter);

    if (fileName.isEmpty()) return;

    QFileInfo fi(fileName);
    lastSaveDir = fi.absolutePath();

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (!currentItem->childCount())
        currentItem = currentItem->parent();

    QVector<CVCurveData*>::iterator it = CVCurves->begin();
    for (; it != CVCurves->end(); it++)
        if (currentItem->text(0) == (*it)->fileName())
            break;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write((*it)->exportParametersAsText().toUtf8());
        ui->plainTextEdit->appendPlainText(QString("The file %1 has been saved").arg(fileName));
    }
    file.close();
}

void MainWindow::smoothSettingsDialogWasClosed(bool buttonType)
{
    if (buttonType) // if OK clicked
    {
        QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
        CVCurveData::PlotType plotType = CVCurveData::I_vs_E;
        if (!currentItem->childCount())
        {
            plotType = static_cast<CVCurveData::PlotType>(CVCurveData::PlotTypes.indexOf(currentItem->text(0)));
            currentItem = currentItem->parent();
        }

        smoothSettingsDialog->hide();
        smoothedCurvePlot->setData(QVector<double>(), QVector<double>());
        customPlot->replot();

        QTreeWidgetItem *item = new QTreeWidgetItem();

        if (plotType == CVCurveData::I_vs_E)
            item->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_smoothed));
        else if (plotType == CVCurveData::E_vs_T)
            item->setText(0, CVCurveData::PlotTypes.at(CVCurveData::E_vs_T_smoothed));


        if (item->text(0) == QString()) return;

        currentItem->addChild(item);
    }
    else            // if Cancel clicked
    {
        smoothSettingsDialog->hide();
        smoothedCurvePlot->setData(QVector<double>(), QVector<double>());
        customPlot->replot();
    }
}

void MainWindow::on_ZoomInButton_clicked()
{
    deactivateAllControlButtons();
    ui->ZoomInButton->setChecked(true);

    customPlot->setSelectionRectMode(QCP::srmZoom);
}

void MainWindow::deactivateAllControlButtons()
{
    customPlot->setInteraction(QCP::iRangeDrag, false);
    customPlot->setInteraction(QCP::iRangeZoom, false);
    customPlot->setSelectionRectMode(QCP::srmNone);

    ui->ZoomInButton->setDown(false);
    ui->ZoomInButton->setChecked(false);
    ui->DragAndDropButton->setDown(false);
    ui->DragAndDropButton->setChecked(false);
    ui->SelectionButton->setDown(false);
    ui->SelectionButton->setChecked(false);
}


void MainWindow::on_ConvolutionButton_clicked()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    convolutionSettingsDialog->show();

    QTreeWidgetItem *newChild_si = new QTreeWidgetItem();
    newChild_si->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint));
    QTreeWidgetItem *newChild_sd = new QTreeWidgetItem();
    newChild_sd->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff));

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (!currentItem->childCount())
        currentItem = currentItem->parent();

    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
    {
        if (currentItem->text(0) == (*it)->fileName())
        {
            if ((*it)->convolute())
            {
                currentItem->addChild(newChild_si);
                currentItem->addChild(newChild_sd);
                ui->treeWidget->currentItem()->setSelected(false);
                currentItem->setExpanded(true);
                newChild_sd->setSelected(true);
                ui->treeWidget->setCurrentItem(newChild_sd);
                ui->treeWidget->setFocus();
                drawPlot(*it, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff));
            }
            else
                QMessageBox::warning(this, "", "Something went wrong!");
            break;
        }
    }
}

void MainWindow::on_SmoothButton_clicked()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    smoothSettingsDialog->show();
    smoothSettingsDialog->on_pol_order_spinBox_valueChanged(0);
}

void MainWindow::applyGraphicalSettings()
{
    double axisWidth = generalSettingsDialog->axisWidth;
    int fontSize = generalSettingsDialog->axisFontSize;
    QString fontName = generalSettingsDialog->axisFontFamily;
    bool ticksIn = generalSettingsDialog->ticksIn;
    bool gridVisible = generalSettingsDialog->gridIsVisible;

    // plot X axis
    customPlot->xAxis->setTickLabelFont(QFont(fontName, fontSize));
    customPlot->xAxis->setLabelFont(QFont(fontName, fontSize));
    QPen axisPen = customPlot->xAxis->basePen();
    axisPen.setWidthF(axisWidth);
    customPlot->xAxis->setBasePen(axisPen);
    customPlot->xAxis->setTickPen(axisPen);
    customPlot->xAxis->setSubTickPen(axisPen);
    customPlot->xAxis->setTickLength(ticksIn ? (5 + axisWidth) : 0, !ticksIn ? (5 + axisWidth) : 0);
    customPlot->xAxis->setSubTickLength(ticksIn ? (2 + axisWidth) : 0, !ticksIn ? (2 + axisWidth) : 0);
    customPlot->xAxis->grid()->setVisible(gridVisible);

    // plot Y axis
    customPlot->yAxis->setTickLabelFont(QFont(fontName, fontSize));
    customPlot->yAxis->setLabelFont(QFont(fontName, fontSize));
    customPlot->yAxis->setBasePen(axisPen);
    customPlot->yAxis->setTickPen(axisPen);
    customPlot->yAxis->setSubTickPen(axisPen);
    customPlot->yAxis->setTickLength(ticksIn ? (5 + axisWidth) : 0, !ticksIn ? (5 + axisWidth) : 0);
    customPlot->yAxis->setSubTickLength(ticksIn ? (2 + axisWidth) : 0, !ticksIn ? (2 + axisWidth) : 0);
    customPlot->yAxis->grid()->setVisible(gridVisible);

    // Curves' color
    QPen pen = curvePlot->pen();
    pen.setWidth(generalSettingsDialog->plotLineWidth);
    pen.setColor(*generalSettingsDialog->curveColor);
    curvePlot->setPen(pen);
    pen.setColor(*generalSettingsDialog->selectionColor);
    curvePlot->selectionDecorator()->setPen(pen);
    pen.setColor(*generalSettingsDialog->smoothColor);
    smoothedCurvePlot->setPen(pen);

    customPlot->replot();
}

void MainWindow::on_actionSettings_triggered()
{
    generalSettingsDialog->show();
}

void MainWindow::mousePositionChanged(QMouseEvent *event)
{
    double mouse_X = customPlot->xAxis->pixelToCoord(event->pos().x());
    double mouse_Y = customPlot->yAxis->pixelToCoord(event->pos().y());

    if(mouse_X > customPlot->xAxis->range().lower && mouse_X < customPlot->xAxis->range().upper && mouse_Y > customPlot->yAxis->range().lower && mouse_Y < customPlot->yAxis->range().upper)
    {
        ui->label_X->setText(QString::number(mouse_X, 'g', 4));
        ui->label_Y->setText(QString::number(mouse_Y, 'g', 4));
    }

    if (ui->CrossButton->isChecked())
    {
        if (mouseIsPressedOnGraph)
        {
            cross_2_hor->point1->setCoords(mouse_X - 1, mouse_Y);
            cross_2_hor->point2->setCoords(mouse_X, mouse_Y);
            cross_2_vert->point1->setCoords(mouse_X, mouse_Y - 1);
            cross_2_vert->point2->setCoords(mouse_X, mouse_Y);
            customPlot->replot();
        }
        else
        {
            cross_1_hor->point1->setCoords(mouse_X - 1, mouse_Y);
            cross_1_hor->point2->setCoords(mouse_X, mouse_Y);
            cross_1_vert->point1->setCoords(mouse_X, mouse_Y - 1);
            cross_1_vert->point2->setCoords(mouse_X, mouse_Y);
            customPlot->replot();
        }
    }
}

void MainWindow::mousePressedOnGraph(QMouseEvent *event)
{
    mouseIsPressedOnGraph = true;
    if (ui->CrossButton->isChecked())
    {
        cross_2_hor->setVisible(true);
        cross_2_vert->setVisible(true);
    }
}

void MainWindow::mouseReleasedOnGraph(QMouseEvent *event)
{
    mouseIsPressedOnGraph = false;
    if (ui->CrossButton->isChecked())
    {
        cross_2_hor->setVisible(false);
        cross_2_vert->setVisible(false);
    }
}

void MainWindow::on_SelectionButton_clicked()
{
    deactivateAllControlButtons();
    ui->SelectionButton->setChecked(true);

    customPlot->setSelectionRectMode(QCP::srmSelect);
}


void MainWindow::on_actionConvolute_all_triggered()
{
    updateFileNamesListFromTreeWidget();
    convoluteMultipleFilesDialog = new ConvoluteMultipleFilesDialog(&fileNamesListFromTreeWidget, this);
    convoluteMultipleFilesDialog->show();
}


void MainWindow::on_actionSmooth_all_I_vs_E_triggered()
{
    updateFileNamesListFromTreeWidget();
    smoothMultipleFilesDialog = new SmoothMultipleFilesDialog(CVCurves, &fileNamesListFromTreeWidget, this);
    smoothMultipleFilesDialog->show();
}

void MainWindow::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
    {
        if (fileNameBuffer == (*it)->fileName())
        {
            (*it)->changeFileName(item->text(0));
            break;
        }
    }
}

void MainWindow::renameTableWidgetItem()
{
    ui->treeWidget->editItem(ui->treeWidget->currentItem(), 0);
}

void MainWindow::deleteSingleTreeWidgetItem()
{
    if (ui->treeWidget->currentItem()->childCount())
    {
        for (int i = 0; i < CVCurves->size(); i++)
            if (ui->treeWidget->currentItem()->text(0) == CVCurves->at(i)->fileName())
            {
                delete CVCurves->takeAt(i);
                break;
            }

        int itemIndex = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());

        if (itemIndex == ui->treeWidget->topLevelItemCount() - 1)
            ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(itemIndex - 1));
        else
            ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(itemIndex + 1));

        ui->treeWidget->takeTopLevelItem(itemIndex);
    }
    else
    {
        QTreeWidgetItem *item = ui->treeWidget->currentItem();
        ui->treeWidget->setCurrentItem(item->parent());
        delete item;
    }
}

void MainWindow::clearTreeWidget()
{
    currentItemChangedSlot_deactivator = true;

    while (!CVCurves->isEmpty())
        delete CVCurves->takeFirst();

    while (ui->treeWidget->topLevelItemCount())
        ui->treeWidget->takeTopLevelItem(0);

    curvePlot->setData(QVector<double>(), QVector<double>());

    currentItemChangedSlot_deactivator = false;
}

void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (previous == NULL) return;
    if (currentItemChangedSlot_deactivator) return;

    qDebug() << "/ current - " << current->text(0) << " // previous - " << previous->text(0) << " /";

    if(previous->childCount())
    {
        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        {
            if (previous->text(0) == (*it)->fileName())
            {
                (*it)->axisRanges.remove(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
                (*it)->axisRanges.insert(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E), Range(customPlot->xAxis->range().lower,
                                                                                               customPlot->xAxis->range().upper,
                                                                                               customPlot->yAxis->range().lower,
                                                                                               customPlot->yAxis->range().upper));
                break;
            }

        }
    }
    else
    {
        QString parentName = previous->parent()->text(0);
        QString plotType = previous->text(0);
        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
        {
            if (parentName == (*it)->fileName())
            {
                (*it)->axisRanges.remove(plotType);
                (*it)->axisRanges.insert(plotType, Range(customPlot->xAxis->range().lower,
                                                         customPlot->xAxis->range().upper,
                                                         customPlot->yAxis->range().lower,
                                                         customPlot->yAxis->range().upper));
                break;
            }
        }
    }

    treeWidgetItemPressed(current, 0);
}

void MainWindow::on_CrossButton_clicked()
{
    if (ui->CrossButton->isChecked())
    {
        cross_1_hor->setVisible(true);
        cross_1_vert->setVisible(true);
        customPlot->replot();
    }
    else
    {
        cross_1_hor->setVisible(false);
        cross_1_vert->setVisible(false);
        cross_2_hor->setVisible(false);
        cross_2_vert->setVisible(false);
        customPlot->replot();
    }
}

