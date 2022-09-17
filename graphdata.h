#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <qcustomplot.h>
#include <cvcurvedata.h>

class GraphData
{
public:

    enum TreeItemType
    {
        SinglePlot,
        Stack,
        Folder,
    };

    GraphData();

    GraphData(CVCurveData *_CVData,
              const QVector<double> *_xValues,
              const QVector<double> *_yValues,
              QString *_xAxisCaption,
              QString *_yAxisCaption,
              QCPRange _defaultXRange,
              QCPRange _defaultYRange,
              CVCurveData::PlotType _plotType,
              bool _isParent = false,
              GraphData::TreeItemType _treeItemType = GraphData::SinglePlot) :

        CVData(_CVData),
        xValues(_xValues),
        yValues(_yValues),
        xAxisCaption(_xAxisCaption),
        yAxisCaption(_yAxisCaption),
        xRange(_defaultXRange),
        yRange(_defaultYRange),
        plotType(_plotType),
        isParent(_isParent),
        treeItemType(_treeItemType)

    { qDebug() << "GraphData constructor"; }

    GraphData(QString *_xAxisCaption,
              QString *_yAxisCaption,
              QCPRange _defaultXRange,
              QCPRange _defaultYRange,
              GraphData::TreeItemType _treeItemType,
              bool _isParent = false) :

        xAxisCaption(_xAxisCaption),
        yAxisCaption(_yAxisCaption),
        xRange(_defaultXRange),
        yRange(_defaultYRange),
        treeItemType(_treeItemType),
        isParent(_isParent)

    { qDebug() << "GraphData constructor"; }

    ~GraphData() { qDebug() << "GraphData destructor"; }

    CVCurveData *CVData;
    CVCurveData::PlotType plotType;
    GraphData::TreeItemType treeItemType;

    QString *xAxisCaption;
    QString *yAxisCaption;

    QCPRange xRange;
    QCPRange yRange;

    bool isParent;

    const QVector<double> *xValues;
    const QVector<double> *yValues;

    // setters
    void setXRange(const QCPRange _xRange) { this->xRange = _xRange; }
    void setYRange(const QCPRange _yRange) { this->yRange = _yRange; }
};

#endif // GRAPHDATA_H
