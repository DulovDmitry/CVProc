#ifndef CVCURVEDATA_H
#define CVCURVEDATA_H

#include <QVector>
#include <QStringList>
#include <QLocale>
#include <QFileInfo>
#include <QtMath>
#include <QProgressDialog>
#include <QObject>
#include <QDebug>
#include <QSettings>
#include <QRegularExpression>
#include <QThread>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "generalsettingsdialog.h"

#include <iostream>
#include <math.h>
#include <Eigen/Dense>

class Range;

class CVCurveData : public QObject
{
    Q_OBJECT
public:
    CVCurveData();
    CVCurveData(QString filePath);
    ~CVCurveData();

    enum PlotType
    {
        I_vs_E,
        E_vs_T,
        I_vs_E_semiint,
        I_vs_E_semidiff,
        I_vs_E_smoothed,
        E_vs_T_smoothed
    };

    static const QStringList PlotTypes;

    // setters:
    void setFilePath(QString filePath);
    void addLineInFileBody(QString line);

    // getters:
    QString fileName() const {return mFileName;}
    QString filePath() const {return mFilePath;} // delete later
    QVector<double> E() const {return mE;}
    QVector<double> Esmoothed() const {return mEsmoothed;}
    QVector<double> I() const {return mI;}
    QVector<double> Isi() const {return mIsi;}
    QVector<double> Isd() const {return mIsd;}
    QVector<double> Ismoothed() const {return mIsmoothed;}
    QVector<double> T() const {return mT;}
    QVector<double> Tcorr() const {return mTcorr;}
    double ERangeMin() const {return mEmin - mErange * curveMargin;}
    double ERangeMax() const {return mEmax + mErange * curveMargin;}
    double IRangeMin() const {return mImin - mIrange * curveMargin;}
    double IRangeMax() const {return mImax + mIrange * curveMargin;}
    double IsiRangeMin() const {return mIsiMin - mIsiRange * curveMargin;}
    double IsiRangeMax() const {return mIsiMax + mIsiRange * curveMargin;}
    double IsdRangeMin() const {return mIsdMin - mIsdRange * curveMargin;}
    double IsdRangeMax() const {return mIsdMax + mIsdRange * curveMargin;}
    double TRangeMin() const {return mTmin - mTrange * curveMargin;}
    double TRangeMax() const {return mTmax + mTrange * curveMargin;}
    double scanRate() const {return mScanRate;}
    int numberOfLinesInFileBody() const {return mFileBody.size();}
    int sizeOfE() const {return mE.size();}
    bool EIsAvaliable() const {return mEIsAvaliable;}
    bool EsmoothedIsAvaliable() const {return mEsmoothedIsAvaliable;}
    bool IIsAvaliable() const {return mIIsAvaliable;}
    bool IsmoothedIsAvaliable() const {return mIsmoothedIsAvaliable;}
    bool TIsAvaliable() const {return mTIsAvaliable;}
    bool IsiIsAvaliable() const {return  mIsiIsAvaliable;}
    bool IsdIsAvaliable() const {return  mIsdIsAvaliable;}
    QString avaliableInputParameters();
    QList<QPair<QString, QVector<double>>> getAvaliableDataForTable();

    // complex methods (public):
    void processFileBody();
    bool convolute();
    bool startConvolution();
    QString exportParametersAsText();
    void SavGolFilter(int m, int pol_order, PlotType);
    void changeFileName(QString newFileName);

    // public members:
    QMap<QString, Range> axisRanges;

signals:
     void progressWasChanged(int progress);

private:
     QSettings *settings;

    // complex methods (private):
    void initializeParameters();
    void correctTime(); // deprecated?
    QVector<double> vectorWithCoefsForSavGolFilter(int m, int pol_order);
    int getDataBeginnigStringNumber();

    // imported file parameters:
    QString mFilePath;
    QString mFileName;
    QStringList mFileBody;

    // numerical data:
    QVector<double> mE;
    QVector<double> mI;
    QVector<double> mT;
    QVector<double> mEsmoothed;
    QVector<double> mIsmoothed;
    QVector<double> mTsmoothed; // нужно ли?
    QVector<double> mIsd;
    QVector<double> mIsi;
    QVector<double> mEcorr;
    QVector<double> mIcorr;
    QVector<double> mTcorr; // delete later
    double mEmax, mEmin, mErange;
    double mImax, mImin, mIrange;
    double mIsiMax, mIsiMin, mIsiRange;
    double mIsdMax, mIsdMin, mIsdRange;
    double mTmax, mTmin, mTrange;
    double curveMargin;
    double Ru;
    double mScanRate; // V/s

    // flags:
    bool mEIsAvaliable;
    bool mIIsAvaliable;
    bool mTIsAvaliable;
    bool mIsiIsAvaliable;
    bool mIsdIsAvaliable;
    bool mIsmoothedIsAvaliable;
    bool mEsmoothedIsAvaliable;
    bool mTcorrIsAvaliable; // delete later
};

class Range
{
public:
    friend class CVCurveData;

    Range() {}
    Range(double x_min, double x_max, double y_min, double y_max)
    {
        _xmin = x_min;
        _xmax = x_max;
        _ymin = y_min;
        _ymax = y_max;
    }
    ~Range() {}

    double x_min() const {return _xmin;}
    double x_max() const {return _xmax;}
    double y_min() const {return _ymin;}
    double y_max() const {return _ymax;}

private:
    double _xmin;
    double _xmax;
    double _ymin;
    double _ymax;

};

#endif // CVCURVEDATA_H
