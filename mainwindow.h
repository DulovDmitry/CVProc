#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>

#include "qcustomplot.h"
#include "cvcurvedata.h"
#include "smoothsettingsdialog.h"
#include "convolutionsettingsdialog.h"
#include "generalsettingsdialog.h"
#include "convolutemultiplefilesdialog.h"
#include "smoothmultiplefilesdialog.h"
#include "showdatadialog.h"
#include "convolutionstatusbardialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GraphData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // settings control:
    void saveSettings();
    void loadSettings();

    // widgets control:
    void treeWidgetItemPressed(QTreeWidgetItem *item, int column);
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void renameTableWidgetItem();
    void deleteSingleTreeWidgetItem();
    void clearTreeWidget();
    void updateFileNamesListFromTreeWidget();
    void progressBarUpdate(int progress);

    // Plot context menu control
    void initializePlotContextMenu();
    void plotContextMenuRequested(QPoint pos);

    // Tree widget context menu control
    void initializeTreeContextMenu();
    void treeContextMenuRequested(QPoint pos);
    void showCVDataInDialog();

    // buttons control:
    void on_SelectionButton_clicked();
    void on_DragAndDropButton_clicked();
    void on_DefaultViewButton_clicked();
    void on_ZoomInButton_clicked();
    void deactivateAllControlButtons();
    void on_ConvolutionButton_clicked();
    void on_SmoothButton_clicked();
    void on_CrossButton_clicked();

    // graph control:
    void drawPlot(CVCurveData *CVData, QString plotType);
    void applyGraphicalSettings();
    void plotDoubleClicked(QMouseEvent* event);
    void axisLabelDoubleClicked(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
    void graphSelectionChanged(QCPDataSelection selection);
    void mousePositionChanged(QMouseEvent *event);
    void mousePressedOnGraph(QMouseEvent* event);
    void mouseReleasedOnGraph(QMouseEvent* event);
    void savePlotAsPDF();
    void savePlotAsJPEG();
    void savePlotAsPNG();
    void exportPlotAsTXT();
    void makeAllGraphsInvisible();

    // Menubar slots
    void on_actionSettings_triggered();
    void on_actionConvolute_all_triggered();
    void on_actionSmooth_all_I_vs_E_triggered();
    void on_actionImport_ASCII_triggered();
    void on_actionCreate_stack_triggered();

    // Other methods and slots
    void SavGolFilterApply(int m, int pol_order);
    void convolutionApply(int Ru, int Cd);
    void smoothSettingsDialogWasClosed(bool buttonType);
    void createPlotStack(QStringList fileNames, QString plotType);
    void getGraphDataFromCVDataAndPlotType(CVCurveData *CVData, QString plotType);

private:
    Ui::MainWindow *ui;
    QVector<CVCurveData*> *CVCurves;
    QSettings *settings;
    QThread *CVCurveDataThread;
    GraphData *graphData;

    QString fileNameBuffer;
    QStringList fileNamesListFromTreeWidget;

    // Flags
    bool currentItemChangedSlot_deactivator = false;
    bool mouseIsPressedOnGraph = false;

    // QCustomPlot
    QCustomPlot *customPlot;
    QCPCurve *curvePlot;
    QCPCurve *smoothedCurvePlot;
    QCPSelectionRect *selectionRect;
    QMap<QTreeWidgetItem*, QList<QCPCurve*>*> mapWithStacks;

    // Items for graph
    QCPItemStraightLine *cross_1_vert;
    QCPItemStraightLine *cross_1_hor;
    QCPItemStraightLine *cross_2_vert;
    QCPItemStraightLine *cross_2_hor;
    QCPItemText *deltaXlabel;
    QCPItemText *deltaYlabel;
    QPointF cross_1_coordinates;

    // additional dialogs
    GeneralSettingsDialog *generalSettingsDialog;
    SmoothSettingsDialog *smoothSettingsDialog;
    ConvolutionSettingsDialog *convolutionSettingsDialog;
    ConvoluteMultipleFilesDialog *convoluteMultipleFilesDialog;
    SmoothMultipleFilesDialog *smoothMultipleFilesDialog;
    ShowDataDialog *showDataDialog;
    ConvolutionStatusBarDialog *convolutionStatusBarDialog;

    // Filters for QFileDialogs
    const QString pdfFilter = "PDF files (*.pdf);; All files (*.*)";
    const QString jpegFilter = "JPEG images (*.jpg);; All files (*.*)";
    const QString pngFilter = "PNG images (*.png);; All files (*.*)";
    const QString txtFilter = "TXT files (*.txt);; All files (*.*)";

    // Registry values
    QString lastSaveDir;
    QString lastOpenDir;

    // Context menu for plot
    QMenu *plotContextMenu;
    QAction *openGraphProperties;
    QAction *savePDF;
    QAction *saveJPEG;
    QAction *savePNG;
    QAction *exportTXT;

    // Context menu for tree widget
    QMenu *treeContextMenu;
    QAction *renameItem;
    QAction *showData;
    QAction *showInExplorer;
    QAction *convolute;
    QAction *deleteItem;
    QAction *deleteAllItems;

};

class GraphData
{
public:
    GraphData() { qDebug() << "GraphData constructor"; }
    ~GraphData() { qDebug() << "GraphData destructor"; }

    QString xAxisCaption;
    QString yAxisCaption;
    QVector<double> xValues;
    QVector<double> yValues;

};
#endif // MAINWINDOW_H
