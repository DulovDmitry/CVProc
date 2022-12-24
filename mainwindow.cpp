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

    QThread::currentThread()->setObjectName("Main thread");

    // Temporary restrictions of functionality
    ui->actionAbout_CVProc->setEnabled(false);
    ui->actionAdd_data_manually->setEnabled(false);
    ui->actionSave_file->setEnabled(false);
    ui->actionImport_CVProc_file->setEnabled(false);

    // Appearance configuration
    ui->checkBox_particularCycle->setEnabled(false);
    ui->spinBox_numberOfCycle->setEnabled(false);
    ui->CrossButton->setCheckable(true);
    ui->DeleteDataButton->setEnabled(false);
    ui->AnalyzePeakButton->setEnabled(false);
    //ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid grey; border-radius: 10px; text-align: center;}");
    //ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: #05B8CC; width: 1px;}");
    ui->splitter_2->setSizes(QList<int>() << 300 << 4000);
    ui->splitter->setSizes(QList<int>() << 2000 << 200);

    // Dialog with convolution progress bar
    convolutionStatusBarDialog = new ConvolutionStatusBarDialog();
    //convolutionStatusBarDialog->setWindowModality(Qt::WindowModal);
    //thread_convolutionStatusBarDialog = new QThread(this);
    //thread_convolutionStatusBarDialog->setObjectName("thread_convolutionStatusBarDialog");
    //convolutionStatusBarDialog->moveToThread(thread_convolutionStatusBarDialog);
    //thread_convolutionStatusBarDialog->start();
    //connect(convolutionStatusBarDialog, SIGNAL(destroyed()),
    //        thread_convolutionStatusBarDialog, SLOT(quit()));

    // Dialog with general settings
    generalSettingsDialog = new GeneralSettingsDialog(this);
    generalSettingsDialog->setWindowModality(Qt::WindowModal);
    connect(generalSettingsDialog, SIGNAL(applyButtonClicked()),
            this, SLOT(applyGraphicalSettings()));
    connect(generalSettingsDialog, SIGNAL(OKButtonClicked()),
            this, SLOT(generalSettingsDialogClosed()));

    // Axis captions
    IAxisCaption = "i, mA"; //"i, " + (generalSettingsDialog->currentUnits ? "A" : "mA");
    EAxisCaption = generalSettingsDialog->potentialAxisCaption + ", " + (generalSettingsDialog->potentialUnits ? "V" : "mV");
    TAxisCaption = "time, seconds";
    IsiAxisCaption = "I (semi-integral)";
    IsdAxisCaption = "I (semi-differential)";

    // Dialog for smooth's settings configuration
    smoothSettingsDialog = new SmoothSettingsDialog(this);
    smoothSettingsDialog->setWindowModality(Qt::NonModal);
    connect(smoothSettingsDialog, SIGNAL(sendFilterParameters(int,int)),
            this, SLOT(SavGolFilterApply(int,int)));
    connect(smoothSettingsDialog, SIGNAL(ButtonClickedSignal(bool)),
            this, SLOT(smoothSettingsDialogWasClosed(bool)));

    // Dialog for convolution's settings configuration
    convolutionSettingsDialog = new ConvolutionSettingsDialog(this);
    convolutionSettingsDialog->setWindowModality(Qt::WindowModal);
//    connect(convolutionSettingsDialog, SIGNAL(okButtonClicked(int,int)),
//            this, SLOT(convolutionApply(int,int)));

    // Dialog for CV data demonstration
    showDataDialog = new ShowDataDialog(this);
    showDataDialog->setWindowModality(Qt::WindowModal);

    // The vector with cyclic voltammetry data configuration
    CVCurves = new QVector<CVCurveData*>;

    // plot preferences
    customPlot = new QCustomPlot(this);
    customPlot->setMinimumHeight(300);
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    customPlot->xAxis->setLabel("E, V");
    customPlot->yAxis->setLabel("I, mA");
    ui->gridLayout->addWidget(customPlot, 1, 0, 1, 1);
    customPlot->setFocus();
    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    customPlot->setInteraction(QCP::iSelectPlottables);
    selectionRect = new QCPSelectionRect(customPlot);
    selectionRect->setVisible(false);

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
    cross_1_vert->setVisible(false);
    cross_1_hor->setVisible(false);
    cross_2_vert = new QCPItemStraightLine(customPlot);
    cross_2_hor = new QCPItemStraightLine(customPlot);
    cross_2_vert->setPen(QPen(QColor(255,0,0)));
    cross_2_hor->setPen(QPen(QColor(255,0,0)));
    cross_2_vert->point1->setCoords(0, 0);
    cross_2_vert->point2->setCoords(0, 1);
    cross_2_hor->point1->setCoords(0, 0);
    cross_2_hor->point2->setCoords(1, 0);
    cross_2_vert->setVisible(false);
    cross_2_hor->setVisible(false);

    // Text items for graph
    deltaXlabel = new QCPItemText(customPlot);
    deltaXlabel->setVisible(false);
    deltaXlabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    deltaXlabel->position->setCoords(0.02, 0.05);
    deltaXlabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    deltaXlabel->setTextAlignment(Qt::AlignLeft);
    deltaXlabel->setFont(QFont(generalSettingsDialog->axisFontFamily, 12));
    deltaYlabel = new QCPItemText(customPlot);
    deltaYlabel->setVisible(false);
    deltaYlabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    deltaYlabel->position->setCoords(0.02, 0.1);
    deltaYlabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    deltaYlabel->setTextAlignment(Qt::AlignLeft);
    deltaYlabel->setFont(QFont(generalSettingsDialog->axisFontFamily, 12));

    // treeWidget preferences
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList() << "Filename" << "Scan rate" << "Points");
    ui->treeWidget->setColumnWidth(0, 193);
    ui->treeWidget->setColumnWidth(1, 60);
    ui->treeWidget->setColumnWidth(2, 45);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget->setDragEnabled(true);
    ui->treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
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
    showData = new QAction("Show data", this);
    showInExplorer = new QAction("Show in explorer", this);
    convolute = new QAction("Convolute", this);
    deleteItem = new QAction("Delete", this);
    deleteAllItems = new QAction("Delete all", this);

    connect(renameItem, SIGNAL(triggered()),
            this, SLOT(renameTableWidgetItem()));
    connect(showData, SIGNAL(triggered()),
            this, SLOT(showCVDataInDialog()));
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

    while (!CVCurves->isEmpty())
        delete CVCurves->takeFirst();

    QList<GraphData*> graphData = hashWithTreeItemsAndGraphData.values();
    foreach(GraphData* data, graphData)
        delete data;

    hashWithTreeItemsAndGraphData.clear();
    delete CVCurves;
    delete customPlot;
    delete convolutionStatusBarDialog;
    delete ui;
}

void MainWindow::drawPlot(QTreeWidgetItem *item)
{
    makeAllGraphsInvisible();

    GraphData graphData = *hashWithTreeItemsAndGraphData.value(item);

    if (graphData.treeItemType == GraphData::Folder)
    {
        customPlot->xAxis->setLabel(QString());
        customPlot->yAxis->setLabel(QString());
        return;
    }

    customPlot->xAxis->setLabel(*(graphData.xAxisCaption));
    customPlot->yAxis->setLabel(*(graphData.yAxisCaption));
    customPlot->xAxis->setRange(graphData.xRange);
    customPlot->yAxis->setRange(graphData.yRange);

    if (graphData.treeItemType == GraphData::SinglePlot)
    {
        curvePlot->setVisible(true);
        smoothedCurvePlot->setVisible(true);
        curvePlot->setData(*(graphData.xValues), *(graphData.yValues));
    }
    else if (graphData.treeItemType == GraphData::Stack)
    {
        foreach(QCPCurve *plot, *(mapWithStacks.value(item)))
        {
            plot->setVisible(true);
        }
    }

    customPlot->replot();
}

void MainWindow::on_actionImport_ASCII_triggered()
{
    ui->treeWidget->clearSelection();

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
                    convolutionStatusBarDialog, SLOT(progressBarUpdate(int)));

            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, CVCurves->last()->fileName());
            item->setText(1, QString::number(CVCurves->last()->scanRate()));
            item->setText(2, QString::number(CVCurves->last()->E()->size()));
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

            hashWithTreeItemsAndGraphData.insert(item, new GraphData(CVCurves->last(),
                                                                     CVCurves->last()->E(), CVCurves->last()->I(),
                                                                     &EAxisCaption, &IAxisCaption,
                                                                     QCPRange(CVCurves->last()->ERangeMin(), CVCurves->last()->ERangeMax()),
                                                                     QCPRange(CVCurves->last()->IRangeMin(), CVCurves->last()->IRangeMax()),
                                                                     CVCurveData::I_vs_E,
                                                                     true));

            if (CVCurves->last()->EIsAvaliable() && CVCurves->last()->IIsAvaliable())
            {
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
                item->addChild(child);

                hashWithTreeItemsAndGraphData.insert(child, new GraphData(CVCurves->last(),
                                                                          CVCurves->last()->E(), CVCurves->last()->I(),
                                                                          &EAxisCaption, &IAxisCaption,
                                                                          QCPRange(CVCurves->last()->ERangeMin(), CVCurves->last()->ERangeMax()),
                                                                          QCPRange(CVCurves->last()->IRangeMin(), CVCurves->last()->IRangeMax()),
                                                                          CVCurveData::I_vs_E));
            }
            if (CVCurves->last()->EIsAvaliable() && CVCurves->last()->TIsAvaliable())
            {
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setText(0, CVCurveData::PlotTypes.at(CVCurveData::E_vs_T));
                item->addChild(child);

                hashWithTreeItemsAndGraphData.insert(child, new GraphData(CVCurves->last(),
                                                                          CVCurves->last()->T(), CVCurves->last()->E(),
                                                                          &TAxisCaption, &EAxisCaption,
                                                                          QCPRange(CVCurves->last()->TRangeMin(), CVCurves->last()->TRangeMax()),
                                                                          QCPRange(CVCurves->last()->ERangeMin(), CVCurves->last()->ERangeMax()),
                                                                          CVCurveData::E_vs_T));
            }
            if (CVCurves->last()->TIsAvaliable() && CVCurves->last()->IIsAvaliable())
            {
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_T));
                item->addChild(child);

                hashWithTreeItemsAndGraphData.insert(child, new GraphData(CVCurves->last(),
                                                                          CVCurves->last()->T(), CVCurves->last()->I(),
                                                                          &TAxisCaption, &IAxisCaption,
                                                                          QCPRange(CVCurves->last()->TRangeMin(), CVCurves->last()->TRangeMax()),
                                                                          QCPRange(CVCurves->last()->IRangeMin(), CVCurves->last()->IRangeMax()),
                                                                          CVCurveData::I_vs_T));
            }

            ui->treeWidget->addTopLevelItem(item);

            ui->plainTextEdit->appendPlainText("The file \"" + file.fileName() + "\" has been imported\n"
                                               "Size: " + QString::number(file.size()) + " bytes\n"
                                               "Number of lines: " + QString::number(CVCurves->last()->numberOfLinesInFileBody()) + "\n"
                                               "Number of data points: " + QString::number(CVCurves->last()->sizeOfE()) + "\n"
                                               "Avaliable parameters: " + CVCurves->last()->avaliableInputParameters() + "\n");

            curvePlot->setData(*(hashWithTreeItemsAndGraphData.value(item)->xValues), *(hashWithTreeItemsAndGraphData.value(item)->yValues));
            customPlot->xAxis->setRange(hashWithTreeItemsAndGraphData.value(item)->xRange);
            customPlot->yAxis->setRange(hashWithTreeItemsAndGraphData.value(item)->yRange);
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
    CVCurveData::PlotType plotType = hashWithTreeItemsAndGraphData.value(currentItem)->plotType;
    hashWithTreeItemsAndGraphData.value(currentItem)->CVData->SavGolFilter(m, pol_order, plotType);

    if (plotType == CVCurveData::I_vs_E)
        smoothedCurvePlot->setData(*(hashWithTreeItemsAndGraphData.value(currentItem)->CVData->E()), *(hashWithTreeItemsAndGraphData.value(currentItem)->CVData->Ismoothed()));
    else if (plotType == CVCurveData::E_vs_T)
        smoothedCurvePlot->setData(*(hashWithTreeItemsAndGraphData.value(currentItem)->CVData->T()), *(hashWithTreeItemsAndGraphData.value(currentItem)->CVData->Esmoothed()));

    customPlot->replot();



//    CVCurveData::PlotType plotType = CVCurveData::I_vs_E;
//    if (!currentItem->childCount())
//    {
//        plotType = static_cast<CVCurveData::PlotType>(CVCurveData::PlotTypes.indexOf(currentItem->text(0)));
//        currentItem = currentItem->parent();
//    }

//    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//    {
//        if (currentItem->text(0) == (*it)->fileName())
//        {
//            (*it)->SavGolFilter(m, pol_order, plotType);

//            if (plotType == CVCurveData::I_vs_E)
//                smoothedCurvePlot->setData((*it)->E(), (*it)->Ismoothed());
//            else if (plotType == CVCurveData::E_vs_T)
//                smoothedCurvePlot->setData((*it)->T(), (*it)->Esmoothed());

//            customPlot->replot();
//            break;
//        }
//    }

}

void MainWindow::convolutionApply(QTreeWidgetItem *currentItem)
{
    if (!currentItem->childCount()) currentItem = currentItem->parent();
    CVCurveData *CVData = hashWithTreeItemsAndGraphData.value(currentItem)->CVData;

    convolutionStatusBarDialog->setFileName(currentItem->text(0));
    convolutionStatusBarDialog->show();

    unsigned long int start_time = clock();

    //qDebug() << "Main " << QThread::currentThread();

    if (CVData->convolute())
    {
        unsigned long int end_time = clock();
        unsigned long int exec_time = end_time - start_time;

        convolutionStatusBarDialog->close();

        ui->plainTextEdit->appendPlainText("Convolution of \"" + currentItem->text(0) + "\" has been completed in " + QString::number((double)exec_time/1000.0) + " seconds.");
        ui->plainTextEdit->appendPlainText("Convolution parameters:");
        ui->plainTextEdit->appendPlainText("\tRu = " + QString::number(generalSettingsDialog->RuDefaultValue));
        ui->plainTextEdit->appendPlainText("\tCd = " + QString::number(generalSettingsDialog->CdDefaultValue) + "\n");

        QTreeWidgetItem *newChild_si = new QTreeWidgetItem(currentItem);
        newChild_si->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint));
        currentItem->addChild(newChild_si);
        hashWithTreeItemsAndGraphData.insert(newChild_si, new GraphData(CVData,
                                                                        CVData->E(), CVData->Isi(),
                                                                        &EAxisCaption, &IsiAxisCaption,
                                                                        QCPRange(CVData->ERangeMin(), CVData->ERangeMax()), QCPRange(CVData->IsiRangeMin(), CVData->IsiRangeMax()),
                                                                        CVCurveData::I_vs_E_semiint));

        QTreeWidgetItem *newChild_sd = new QTreeWidgetItem(currentItem);
        newChild_sd->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff));
        currentItem->addChild(newChild_sd);
        hashWithTreeItemsAndGraphData.insert(newChild_sd, new GraphData(CVData,
                                                                        CVData->E(), CVData->Isd(),
                                                                        &EAxisCaption, &IsdAxisCaption,
                                                                        QCPRange(CVData->ERangeMin(), CVData->ERangeMax()), QCPRange(CVData->IsdRangeMin(), CVData->IsdRangeMax()),
                                                                        CVCurveData::I_vs_E_semidiff));

        ui->treeWidget->clearSelection();
        currentItem->setExpanded(true);
        newChild_sd->setSelected(true);
        ui->treeWidget->setCurrentItem(newChild_sd);
        ui->treeWidget->setFocus();
        drawPlot(newChild_sd);
    }
    else
    {
        convolutionStatusBarDialog->close();
        QMessageBox::warning(this, "Error", "Something went wrong during convolution operation!");
    }

//    if (!currentItem->childCount())
//        currentItem = currentItem->parent();

//    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//    {
//        if (currentItem->text(0) == (*it)->fileName())
//        {
//            if ((*it)->startConvolution())
//            {

//            }

//            break;
//        }
//    }
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
    if (ui->DragAndDropButton->isChecked())
    {
        deactivateAllControlButtons();
        ui->DragAndDropButton->setChecked(true);

        customPlot->setInteraction(QCP::iRangeDrag);
        customPlot->setInteraction(QCP::iRangeZoom);
    }
    else
        deactivateAllControlButtons();
}

void MainWindow::on_DefaultViewButton_clicked()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    customPlot->xAxis->setRange(hashWithTreeItemsAndGraphData.value(currentItem)->xRange);
    customPlot->yAxis->setRange(hashWithTreeItemsAndGraphData.value(currentItem)->yRange);
    customPlot->replot();

//    if (currentItem->childCount())
//    {
//        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//        {
//            if (currentItem->text(0) == (*it)->fileName())
//            {
//                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->yAxis->setRange((*it)->IRangeMin(), (*it)->IRangeMax());
//                customPlot->replot();
//                break;
//            }
//        }
//        return;
//    }

//    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//        if ((*it)->fileName() == currentItem->parent()->text(0))
//        {
//            if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E))
//            {
//                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->yAxis->setRange((*it)->IRangeMin(), (*it)->IRangeMax());
//                customPlot->replot();
//                break;
//            }
//            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T))
//            {
//                customPlot->xAxis->setRange((*it)->TRangeMin(), (*it)->TRangeMax());
//                customPlot->yAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->replot();
//                break;
//            }
//            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint))
//            {
//                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->yAxis->setRange((*it)->IsiRangeMin(), (*it)->IsiRangeMax());
//                customPlot->replot();
//                break;
//            }
//            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff))
//            {
//                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->yAxis->setRange((*it)->IsdRangeMin(), (*it)->IsdRangeMax());
//                customPlot->replot();
//                break;
//            }
//            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_smoothed))
//            {
//                customPlot->xAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->yAxis->setRange((*it)->IRangeMin(), (*it)->IRangeMax());
//                customPlot->replot();
//                break;
//            }
//            else if (currentItem->text(0) == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T_smoothed))
//            {
//                customPlot->xAxis->setRange((*it)->TRangeMin(), (*it)->TRangeMax());
//                customPlot->yAxis->setRange((*it)->ERangeMin(), (*it)->ERangeMax());
//                customPlot->replot();
//                break;
//            }
//        }
}

void MainWindow::treeWidgetItemPressed(QTreeWidgetItem *item, int column)
{
    CVCurveData::PlotType plotType = hashWithTreeItemsAndGraphData.value(item)->plotType;
    ui->ConvolutionButton->setEnabled(plotType == CVCurveData::I_vs_E || plotType == CVCurveData::I_vs_E_smoothed);

    drawPlot(item);

    // fileNameBuffer = item->text(0);  // DELETE

    // makeAllGraphsInvisible();

//    if (mapWithStacks.contains(item))
//    {
//        foreach (QCPCurve *plot, *(mapWithStacks.value(item))) {
//            plot->setVisible(true);
//        }
//        customPlot->replot();
//        return;
//    }

//    curvePlot->setVisible(true);
//    smoothedCurvePlot->setVisible(true);



//    if (item->childCount())
//    {
//        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//        {
//            if (item->text(0) == (*it)->fileName())
//            {
//                drawPlot(*it, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
//                ui->ConvolutionButton->setEnabled(true);
//                break;
//            }
//        }
//        return;
//    }

//    QString parentName = item->parent()->text(0);
//    QString itemName = item->text(0);

//    if (itemName.contains(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E))) ui->ConvolutionButton->setEnabled(true);

//    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//        if ((*it)->fileName() == parentName) drawPlot(*it, itemName);
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
        showData->setDisabled(true);
        convolute->setDisabled(true);
    }
    else
    {
        renameItem->setDisabled(false);
        showInExplorer->setDisabled(false);
        exportTXT->setDisabled(false);
        deleteItem->setDisabled(false);
        showData->setDisabled(false);
        convolute->setDisabled(true);

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

void MainWindow::showCVDataInDialog()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (!currentItem->childCount())
        currentItem = currentItem->parent();

    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
    {
        if (currentItem->text(0) == (*it)->fileName())
        {
            showDataDialog->setCVData(*it);
            showDataDialog->fillTable();
            showDataDialog->show();
            break;
        }
    }
}

//void MainWindow::progressBarUpdate(int progress)
//{
//    ui->progressBar->setValue(progress);
//}

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
    {
        ui->DeleteDataButton->setEnabled(true);
        ui->AnalyzePeakButton->setEnabled(true);
    }
    else
    {
        ui->DeleteDataButton->setEnabled(false);
        ui->AnalyzePeakButton->setEnabled(false);
    }
}

void MainWindow::plotContextMenuRequested(QPoint pos)
{
    plotContextMenu->popup(customPlot->mapToGlobal(pos));
}

void MainWindow::initializeTreeContextMenu()
{
    treeContextMenu->addAction(renameItem);
    treeContextMenu->addAction(showData);
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

void MainWindow::makeAllGraphsInvisible()
{
    for(int i = 0; i < customPlot->plottableCount(); i++)
    {
        customPlot->plottable(i)->setVisible(false);
    }
    customPlot->replot();
}

void MainWindow::smoothSettingsDialogWasClosed(bool buttonType)
{
    if (buttonType) // if OK clicked
    {
        QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
        CVCurveData::PlotType plotType = hashWithTreeItemsAndGraphData.value(currentItem)->plotType;
        CVCurveData *CVData = hashWithTreeItemsAndGraphData.value(currentItem)->CVData;

        smoothSettingsDialog->hide();
        smoothedCurvePlot->setData(QVector<double>(), QVector<double>());
        customPlot->replot();

        QTreeWidgetItem *item = new QTreeWidgetItem();

        if (plotType == CVCurveData::I_vs_E)
        {
            item->setText(0, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_smoothed));
            hashWithTreeItemsAndGraphData.insert(item, new GraphData(CVData,
                                                                     CVData->E(), CVData->Ismoothed(),
                                                                     &EAxisCaption, &IAxisCaption,
                                                                     hashWithTreeItemsAndGraphData.value(currentItem)->xRange,
                                                                     hashWithTreeItemsAndGraphData.value(currentItem)->yRange,
                                                                     CVCurveData::I_vs_E_smoothed));
        }
        else if (plotType == CVCurveData::E_vs_T)
        {
            item->setText(0, CVCurveData::PlotTypes.at(CVCurveData::E_vs_T_smoothed));
            hashWithTreeItemsAndGraphData.insert(item, new GraphData(CVData,
                                                                     CVData->T(), CVData->Esmoothed(),
                                                                     &TAxisCaption, &EAxisCaption,
                                                                     hashWithTreeItemsAndGraphData.value(currentItem)->xRange,
                                                                     hashWithTreeItemsAndGraphData.value(currentItem)->yRange,
                                                                     CVCurveData::E_vs_T_smoothed));
        }

        if (item->text(0) == QString()) return;

        if (currentItem->childCount())
            currentItem->addChild(item);
        else
            currentItem->parent()->addChild(item);
    }
    else  // if Cancel clicked
    {
        smoothSettingsDialog->hide();
        smoothedCurvePlot->setData(QVector<double>(), QVector<double>());
        customPlot->replot();
    }
}

void MainWindow::createPlotStack()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    int sizeOfStack = selectedItems.size();
    CVCurveData::PlotType plotType = hashWithTreeItemsAndGraphData.value(selectedItems.first())->plotType;
    QString plotTypeString = CVCurveData::PlotTypes.at(plotType);

    QString *xAxisStackLabel = hashWithTreeItemsAndGraphData.value(selectedItems.first())->xAxisCaption;
    QString *yAxisStackLabel = hashWithTreeItemsAndGraphData.value(selectedItems.first())->yAxisCaption;

    QList<QCPRange> xAxisRanges;
    QList<QCPRange> yAxisRanges;
    QCPRange xAxisStackRange;
    QCPRange yAxisStackRange;

    foreach (QTreeWidgetItem *item, selectedItems) {
        xAxisRanges.append(hashWithTreeItemsAndGraphData.value(item)->xRange);
        yAxisRanges.append(hashWithTreeItemsAndGraphData.value(item)->yRange);
    }

    xAxisStackRange = findMaximalQCPRange(xAxisRanges);
    yAxisStackRange = findMaximalQCPRange(yAxisRanges);

    QTreeWidgetItem *stackItem = new QTreeWidgetItem(ui->treeWidget);
    stackItem->setText(0, "Stack [" + plotTypeString + "]");
    stackItem->setTextColor(0, QColor(0, 0, 255));
    ui->treeWidget->addTopLevelItem(stackItem);

    hashWithTreeItemsAndGraphData.insert(stackItem, new GraphData(xAxisStackLabel, yAxisStackLabel,
                                                                  xAxisStackRange, yAxisStackRange,
                                                                  GraphData::Stack));

    makeAllGraphsInvisible();
    QList<QCPCurve*> *plotList = new QList<QCPCurve*>();
    GradientColor gradient(sizeOfStack);
    QList<QColor> gradientColors = gradient.getGradientColorsList();

    for (int i = 0; i < sizeOfStack; i++)
    {
        QCPCurve *plot = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
        plot->addData(*(hashWithTreeItemsAndGraphData.value(selectedItems.at(i))->xValues), *(hashWithTreeItemsAndGraphData.value(selectedItems.at(i))->yValues));

        QPen pen = plot->pen();
        pen.setColor(gradientColors.at(i));
        pen.setWidth(generalSettingsDialog->plotLineWidth);
        plot->setPen(pen);
        plotList->append(plot);
    }

    mapWithStacks.insert(stackItem, plotList);
    drawPlot(stackItem);
    ui->treeWidget->clearSelection();
    ui->treeWidget->setCurrentItem(stackItem);

//    QTreeWidgetItem *stackItem = new QTreeWidgetItem(ui->treeWidget);
//    stackItem->setText(0, "Stack [" + plotType + "]");
//    stackItem->setTextColor(0, QColor(0, 0, 255));
//    ui->treeWidget->addTopLevelItem(stackItem);
//    //ui->treeWidget->setCurrentItem(stackItem);

//    makeAllGraphsInvisible();

//    QList<QCPCurve*> *plotList = new QList<QCPCurve*>();

//    //rgb(168,0,119)
//    //rgb(102,255,0)
//    int r1 = 168, g1 = 0, b1 = 119;
//    int r2 = 102, g2 = 255, b2 = 0;

//    for (int i = 0; i < fileNames.size(); i++)
//    {
//        double pointPos = (double)i/(fileNames.size() - 1);

//        QCPCurve *plot = new QCPCurve(customPlot->xAxis, customPlot->yAxis);

//        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//            if ((*it)->fileName() == fileNames.at(i))
//            {
//                // (!) getGraphDataFromCVDataAndPlotType(*it, plotType);
//                break;
//            }

//        // (!) plot->addData(graphData->xValues, graphData->yValues);

//        QPen pen = plot->pen();
//        pen.setColor(QColor(r1 + (pointPos*(r2-r1)), g1 + (pointPos*(g2-g1)), b1 + (pointPos*(b2-b1))));
//        pen.setWidth(generalSettingsDialog->plotLineWidth);
//        plot->setPen(pen);
//        plotList->append(plot);
//    }

//    mapWithStacks.insert(stackItem, plotList);
//    customPlot->replot();
}

QCPRange MainWindow::findMaximalQCPRange(QList<QCPRange> ranges)
{
    QCPRange maximalRange = ranges.first();

    foreach(QCPRange range, ranges)
    {
        if (range.upper > maximalRange.upper)
            maximalRange.upper = range.upper;
        if (range.lower < maximalRange.lower)
            maximalRange.lower = range.lower;
    }

    return maximalRange;
}

void MainWindow::generalSettingsDialogClosed()
{
    foreach(CVCurveData *CVData, *CVCurves)
        CVData->correctPotentialValues(generalSettingsDialog);

    EAxisCaption = generalSettingsDialog->potentialAxisCaption;
}

void MainWindow::convoluteMultipleFiles(QList<QTreeWidgetItem*> itemsForConvolutionList)
{
    foreach(QTreeWidgetItem *item, itemsForConvolutionList)
    {
        convolutionApply(item);
    }
}

//void MainWindow::getGraphDataFromCVDataAndPlotType(CVCurveData *CVData, QString plotType)
//{
//    if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E))
//    {
//        graphData->xValues = CVData->E();
//        graphData->yValues = CVData->I();
//        graphData->xAxisCaption = "E, V";
//        graphData->yAxisCaption = "I, mA";
//    }
//    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T))
//    {
//        graphData->xValues = CVData->T();
//        graphData->yValues = CVData->E();
//        graphData->xAxisCaption = "t, s";
//        graphData->yAxisCaption = "E, V";
//    }
//    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semiint))
//    {
//        graphData->xValues = CVData->Esmoothed();
//        graphData->yValues = CVData->Isi();
//        graphData->xAxisCaption = "E, V";
//        graphData->yAxisCaption = "I (convoluted)";
//    }
//    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_semidiff))
//    {
//        graphData->xValues = CVData->Esmoothed();
//        graphData->yValues = CVData->Isd();
//        graphData->xAxisCaption = "E, V";
//        graphData->yAxisCaption = "I (semidifferential)";
//    }
//    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::I_vs_E_smoothed))
//    {
//        graphData->xValues = CVData->E();
//        graphData->yValues = CVData->Ismoothed();
//        graphData->xAxisCaption = "E, V";
//        graphData->yAxisCaption = "I, mA";
//    }
//    else if (plotType == CVCurveData::PlotTypes.at(CVCurveData::E_vs_T_smoothed))
//    {
//        graphData->xValues = CVData->T();
//        graphData->yValues = CVData->Esmoothed();
//        graphData->xAxisCaption = "t, s";
//        graphData->yAxisCaption = "E, V";
//    }
//}

void MainWindow::on_ZoomInButton_clicked()
{
    if (ui->ZoomInButton->isEnabled())
    {
        deactivateAllControlButtons();
        ui->ZoomInButton->setChecked(true);

        customPlot->setSelectionRectMode(QCP::srmZoom);
    }
    else
        deactivateAllControlButtons();
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
    ui->CrossButton->setDown(false);
    ui->CrossButton->setChecked(false);

    cross_1_hor->setVisible(false);
    cross_1_vert->setVisible(false);
    cross_2_hor->setVisible(false);
    cross_2_vert->setVisible(false);
    customPlot->replot();
}

void MainWindow::on_ConvolutionButton_clicked()
{
    if (ui->treeWidget->selectedItems().isEmpty()) return;

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    //convolutionSettingsDialog->show();
    convolutionApply(currentItem);
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
            deltaXlabel->setText("ΔX = " + QString::number(mouse_X - cross_1_coordinates.x(), 'g', 4));
            deltaYlabel->setText("ΔY = " + QString::number(mouse_Y - cross_1_coordinates.y(), 'g', 4));
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
        deltaXlabel->setVisible(true);
        deltaYlabel->setVisible(true);
        cross_2_hor->setVisible(true);
        cross_2_vert->setVisible(true);
        cross_1_coordinates.setX(customPlot->xAxis->pixelToCoord(event->pos().x()));
        cross_1_coordinates.setY(customPlot->yAxis->pixelToCoord(event->pos().y()));
    }
}

void MainWindow::mouseReleasedOnGraph(QMouseEvent *event)
{
    mouseIsPressedOnGraph = false;
    if (ui->CrossButton->isChecked())
    {
        deltaXlabel->setVisible(false);
        deltaYlabel->setVisible(false);
        cross_2_hor->setVisible(false);
        cross_2_vert->setVisible(false);

        double mouse_X = customPlot->xAxis->pixelToCoord(event->pos().x());
        double mouse_Y = customPlot->yAxis->pixelToCoord(event->pos().y());

        cross_1_hor->point1->setCoords(mouse_X - 1, mouse_Y);
        cross_1_hor->point2->setCoords(mouse_X, mouse_Y);
        cross_1_vert->point1->setCoords(mouse_X, mouse_Y - 1);
        cross_1_vert->point2->setCoords(mouse_X, mouse_Y);
        customPlot->replot();
    }
}

void MainWindow::on_SelectionButton_clicked()
{
    if (ui->SelectionButton->isChecked())
    {
        deactivateAllControlButtons();
        ui->SelectionButton->setChecked(true);

        customPlot->setSelectionRectMode(QCP::srmSelect);
    }
    else
        deactivateAllControlButtons();
}

void MainWindow::on_actionConvolute_all_triggered()
{
    // updateFileNamesListFromTreeWidget();
//    QList<QTreeWidgetItem*> parentItems;
//    QList<QTreeWidgetItem*> allTreeItems = hashWithTreeItemsAndGraphData.keys();
//    foreach(QTreeWidgetItem *item, allTreeItems)
//    {
//        if (item->childCount())
//        {
//            parentItems.append(item);
//        }
//    }

    QList<QTreeWidgetItem*> parentItems;
    QList<QTreeWidgetItem *> allTreeItems = ui->treeWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive);
    foreach(QTreeWidgetItem *item, allTreeItems)
    {
        if (item->childCount())
        {
            parentItems.append(item);
        }
    }

    convoluteMultipleFilesDialog = new ConvoluteMultipleFilesDialog(parentItems, this);
    connect(convoluteMultipleFilesDialog, SIGNAL(convoluteButtonClicked(QList<QTreeWidgetItem*>)),
            this, SLOT(convoluteMultipleFiles(QList<QTreeWidgetItem*>)));
    convoluteMultipleFilesDialog->setWindowModality(Qt::WindowModal);
    convoluteMultipleFilesDialog->show();
}

void MainWindow::on_actionSmooth_all_I_vs_E_triggered()
{
    updateFileNamesListFromTreeWidget();
    // smoothMultipleFilesDialog = new SmoothMultipleFilesDialog(CVCurves, &fileNamesListFromTreeWidget, this);
    smoothMultipleFilesDialog = new SmoothMultipleFilesDialog(&hashWithTreeItemsAndGraphData, this);
    smoothMultipleFilesDialog->setWindowModality(Qt::WindowModal);
    smoothMultipleFilesDialog->show();
}

void MainWindow::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;
    if (item->text(0) == QString() || item->text(0).isEmpty() || item->text(0) == NULL) return;
    if (!hashWithTreeItemsAndGraphData.contains(item)) return;

    hashWithTreeItemsAndGraphData.value(item)->CVData->changeFileName(item->text(0));

//    for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//    {
//        if (fileNameBuffer == (*it)->fileName())
//        {
//            (*it)->changeFileName(item->text(0));
//            break;
//        }
//    }
}

void MainWindow::renameTableWidgetItem()
{
    ui->treeWidget->editItem(ui->treeWidget->currentItem(), 0);
}

void MainWindow::deleteSingleTreeWidgetItem()
{
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();

    if (hashWithTreeItemsAndGraphData.value(currentItem)->treeItemType == GraphData::SinglePlot)
    {
        if (hashWithTreeItemsAndGraphData.value(currentItem)->isParent)
        {
            CVCurveData *_CVData = hashWithTreeItemsAndGraphData.value(currentItem)->CVData;
            QList<GraphData*> values = hashWithTreeItemsAndGraphData.values();

            foreach (GraphData *data, values) {
                if (data->CVData == _CVData)
                {
                    QTreeWidgetItem *item = hashWithTreeItemsAndGraphData.key(data);
                    hashWithTreeItemsAndGraphData.remove(item);
                    values.removeOne(data);
                    delete data;
                }
            }
            CVCurves->removeOne(_CVData);
            delete _CVData;
            delete currentItem;
            return;
        }

        delete hashWithTreeItemsAndGraphData.value(currentItem);
        hashWithTreeItemsAndGraphData.remove(currentItem);
        delete currentItem;
    }
    else if (hashWithTreeItemsAndGraphData.value(currentItem)->treeItemType == GraphData::Stack)
    {

    }
    else if (hashWithTreeItemsAndGraphData.value(currentItem)->treeItemType == GraphData::Folder)
    {

    }

//    if (CVCurves->isEmpty())
//    {
//        QList<GraphData*> values = hashWithTreeItemsAndGraphData.values();
//        while (!values.isEmpty())
//            delete values.takeFirst();

//        curvePlot->setData(QVector<double>(), QVector<double>());
//        customPlot->replot();
//    }


//    if (ui->treeWidget->currentItem()->childCount())
//    {
//        for (int i = 0; i < CVCurves->size(); i++)
//            if (ui->treeWidget->currentItem()->text(0) == CVCurves->at(i)->fileName())
//            {
//                delete CVCurves->takeAt(i);
//                break;
//            }

//        if (CVCurves->isEmpty())
//        {
//            currentItemChangedSlot_deactivator = true;
//            ui->treeWidget->takeTopLevelItem(0);
//            curvePlot->setData(QVector<double>(), QVector<double>());
//            customPlot->replot();
//            currentItemChangedSlot_deactivator = false;
//            return;
//        }

//        int itemIndex = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());

//        if (itemIndex == ui->treeWidget->topLevelItemCount() - 1)
//            ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(itemIndex - 1));
//        else
//            ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(itemIndex + 1));

//        ui->treeWidget->takeTopLevelItem(itemIndex);
//    }
//    else
//    {
//        QTreeWidgetItem *item = ui->treeWidget->currentItem();
//        ui->treeWidget->setCurrentItem(item->parent());
//        delete item;
//    }
}

void MainWindow::clearTreeWidget()
{
    QList<GraphData*> values = hashWithTreeItemsAndGraphData.values();

    while (!CVCurves->isEmpty())
        delete CVCurves->takeFirst();

    while (!values.isEmpty())
        delete values.takeFirst();

    while (ui->treeWidget->topLevelItemCount())
        ui->treeWidget->takeTopLevelItem(0);

    hashWithTreeItemsAndGraphData.clear();
    mapWithStacks.clear();

    curvePlot->setData(QVector<double>(), QVector<double>());
    customPlot->replot();

//    currentItemChangedSlot_deactivator = true;

//    while (!CVCurves->isEmpty())
//        delete CVCurves->takeFirst();

//    while (ui->treeWidget->topLevelItemCount())
//        ui->treeWidget->takeTopLevelItem(0);

//    curvePlot->setData(QVector<double>(), QVector<double>());
//    customPlot->replot();

//    currentItemChangedSlot_deactivator = false;
}

void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (previous == NULL) return;
    if (current == NULL) return;
    if (!hashWithTreeItemsAndGraphData.contains(previous)) return;
    if (!hashWithTreeItemsAndGraphData.contains(current)) return;
    if (currentItemChangedSlot_deactivator) return;

//    if (mapWithStacks.contains(previous))
//    {
//        treeWidgetItemPressed(current, 0);
//        return;
//    }

    treeWidgetItemPressed(current, 0);

//    qDebug() << "/ current - " << current->text(0) << " // previous - " << previous->text(0) << " /";

//    if(previous->childCount())
//    {
//        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//        {
//            if (previous->text(0) == (*it)->fileName())
//            {
//                (*it)->axisRanges.remove(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
//                (*it)->axisRanges.insert(CVCurveData::PlotTypes.at(CVCurveData::I_vs_E), Range(customPlot->xAxis->range().lower,
//                                                                                               customPlot->xAxis->range().upper,
//                                                                                               customPlot->yAxis->range().lower,
//                                                                                               customPlot->yAxis->range().upper));
//                break;
//            }

//        }
//    }
//    else
//    {
//        QString parentName = previous->parent()->text(0);
//        QString plotType = previous->text(0);
//        for (QVector<CVCurveData*>::iterator it = CVCurves->begin(); it != CVCurves->end(); it++)
//        {
//            if (parentName == (*it)->fileName())
//            {
//                (*it)->axisRanges.remove(plotType);
//                (*it)->axisRanges.insert(plotType, Range(customPlot->xAxis->range().lower,
//                                                         customPlot->xAxis->range().upper,
//                                                         customPlot->yAxis->range().lower,
//                                                         customPlot->yAxis->range().upper));
//                break;
//            }
//        }
//    }

}

void MainWindow::on_CrossButton_clicked()
{
    if (ui->CrossButton->isChecked())
    {
        deactivateAllControlButtons();
        ui->CrossButton->setChecked(true);

        cross_1_hor->setVisible(true);
        cross_1_vert->setVisible(true);
        customPlot->replot();
    }
    else
        deactivateAllControlButtons();
}

void MainWindow::on_actionCreate_stack_triggered()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();

    if (selectedItems.size() < 2)
    {
        QMessageBox::warning(this, "Create stack [error]", "At least two plots should be selected.\nUse CTRL button to select more than one plot.");
        return;
    }

    CVCurveData::PlotType plotType = hashWithTreeItemsAndGraphData.value(selectedItems.first())->plotType;
    foreach (QTreeWidgetItem *item, selectedItems) {
        if (hashWithTreeItemsAndGraphData.value(item)->plotType != plotType)
        {
            QMessageBox::warning(this, "Create stack [error]", "Cannot create stack of plots having different types!");
            return;
        }
    }

    if (hashWithTreeItemsAndGraphData.value(selectedItems.first())->treeItemType == GraphData::SinglePlot)
    {
        createPlotStack();
        return;
    }
    else if (hashWithTreeItemsAndGraphData.value(selectedItems.first())->treeItemType == GraphData::Stack)
    {
        QMessageBox::question(this, "WTF?", "Stack of stacks? Are you serious? Never do that again!");
        return;
    }
    else if (hashWithTreeItemsAndGraphData.value(selectedItems.first())->treeItemType == GraphData::Folder)
    {
        QMessageBox::warning(this, "Create stack [error]", "Cannot create stack of folders");
        return;
    }

//    QStringList selectedFileNames;
//    QStringList selectedFileParentsNames;
//    foreach (QTreeWidgetItem *item, ui->treeWidget->selectedItems()) {
//        selectedFileNames.append(item->text(0));
//        if (!item->childCount()) selectedFileParentsNames.append(item->parent()->text(0));
//    }

//    ui->treeWidget->clearSelection();

//    if (!selectedFileParentsNames.isEmpty()) //
//    {
//        if (selectedFileNames.size() != selectedFileParentsNames.size())
//        {
//            QMessageBox::warning(this, "Create stack [error]", "Cannot create stack of plots having different types");
//            return;
//        }

//        for (int i = 0; i < selectedFileNames.size(); i++)
//        {
//            if (selectedFileNames.at(i) != selectedFileNames.at(0))
//            {
//                QMessageBox::warning(this, "Create stack [error]", "Cannot create stack of plots having different types");
//                return;
//            }
//        }

//        createPlotStack(selectedFileParentsNames, selectedFileNames.at(0));
//    }
//    else
//    {
//        createPlotStack(selectedFileNames, CVCurveData::PlotTypes.at(CVCurveData::I_vs_E));
//    }
}
void MainWindow::on_SaveViewButton_clicked()
{
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    hashWithTreeItemsAndGraphData.value(currentItem)->setXRange(customPlot->xAxis->range());
    hashWithTreeItemsAndGraphData.value(currentItem)->setYRange(customPlot->yAxis->range());
}

