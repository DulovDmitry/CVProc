#include "cvcurvedata.h"

using namespace Eigen;
using namespace std;

QStringList const CVCurveData::PlotTypes = QStringList() << "I vs E" << "E vs t" << "I vs t" << "I (semiintegral) vs E" << "I (semidifferential) vs E" << "I vs E (smoothed)" << "E vs t (smoothed)" << "I vs t (smoothed)";

CVCurveData::CVCurveData()
{
    initializeParameters();

    qDebug() << "CVCurveData constructor";
}

CVCurveData::CVCurveData(QString filePath)
{
    this->mFilePath = filePath;
    settings = new QSettings("ORG335a", "CVProc", this);
    initializeParameters();

//    qDebug() << "CVCurveData 1" << QThread::currentThread();

//    QThread *_thread = new QThread(this);
//    _thread->setObjectName("CVCurveData");
//    this->moveToThread(_thread);
//    _thread->start();

//    qDebug() << "CVCurveData 2" << QThread::currentThread();

    qDebug() << "CVCurveData constructor";
}

CVCurveData::~CVCurveData()
{
    qDebug() << "CVCurveData destructor";
}

void CVCurveData::setFilePath(QString filePath)
{
    this->mFilePath = filePath;
}

void CVCurveData::addLineInFileBody(QString line)
{
    mFileBody.append(line);
}

void CVCurveData::processFileBody()
{
    QRegularExpression separator("[( |\t)]");
    QLocale locale(settings->value("DECIMAL_SEPARATOR", false).toBool() ? QLocale::English : QLocale::Catalan);
    QStringList currentLine;
    //int headerSize = mFileBody.at(1).split(separator, Qt::SkipEmptyParts).last().toInt();
    int headerSize = getDataBeginnigStringNumber();

    for (int i = 0; i < headerSize; i++)
    {
        currentLine = mFileBody.at(i).split(separator, Qt::SkipEmptyParts);
        if (currentLine.isEmpty()) continue;
        if (currentLine.at(0) == QString("dE/dt"))
        {
            mScanRate = locale.toDouble(currentLine.at(1));
            if (mFileBody.at(i + 1).split(separator, Qt::SkipEmptyParts).at(2) == QString("mV/s")) mScanRate = mScanRate/1000;
            break;
        }
    }

    short columnWithE = -1;
    short columnWithI = -1;
    short columnWithT = -1;

    currentLine = mFileBody.at(headerSize - 1).split(separator, Qt::SkipEmptyParts);
    for (int i = 0; i < currentLine.size(); i++)
    {
        if (currentLine.at(i) == "Ewe/V" || currentLine.at(i) == "Ewe/mV")
        {
            columnWithE = i;
            mEIsAvaliable = true;
        }
        else if (currentLine.at(i) == "I/A" || currentLine.at(i) == "I/mA")
        {
            columnWithI = i;
            mIIsAvaliable = true;
        }
        else if (currentLine.at(i) == "time/s")
        {
            columnWithT = i;
            mTIsAvaliable = true;
        }
    }

    if (!mEIsAvaliable || !mIIsAvaliable) return;
    currentLine.clear();

    for (int i = headerSize; i < mFileBody.size(); i++)
    {
        currentLine = mFileBody.at(i).split(separator, Qt::SkipEmptyParts);
        if (mEIsAvaliable) this->mE.append(locale.toDouble(currentLine.at(columnWithE)));
        if (mIIsAvaliable) this->mI.append(locale.toDouble(currentLine.at(columnWithI)));
        if (mTIsAvaliable) this->mT.append(locale.toDouble(currentLine.at(columnWithT)));
    }

    if (mEIsAvaliable)
    {
        correctPotentialValues();

        mEmin = *std::min_element(mE.begin(), mE.end());
        mEmax = *std::max_element(mE.begin(), mE.end());
        mErange = mEmax - mEmin;
    }
    if (mIIsAvaliable)
    {
        mImin = *std::min_element(mI.begin(), mI.end());
        mImax = *std::max_element(mI.begin(), mI.end());
        mIrange = mImax - mImin;

        //axisRanges.insert(PlotTypes.at(I_vs_E), Range(ERangeMin(), ERangeMax(), IRangeMin(), IRangeMax()));
    }
    if(mTIsAvaliable)
    {
        double t0 = mT[0];
        for (int i = 0; i < mT.size(); i++)
            mT[i] -= t0;


        mTmin = *std::min_element(mT.begin(), mT.end());
        mTmax = *std::max_element(mT.begin(), mT.end());
        mTrange = mTmax - mTmin;

        //axisRanges.insert(PlotTypes.at(E_vs_T), Range(TRangeMin(), TRangeMax(), ERangeMin(), ERangeMax()));
    }

    QFileInfo fileInfo(mFilePath);
    mFileName = fileInfo.completeBaseName();
}

QString CVCurveData::avaliableInputParameters()
{
    QStringList parameters;

    parameters.append(mEIsAvaliable ? "E" : NULL);
    parameters.append(mIIsAvaliable ? "I" : NULL);
    parameters.append(mTIsAvaliable ? "t" : NULL);

    return parameters.join(", ");
}

QList<QPair<QString, QVector<double>>> CVCurveData::getAvaliableDataForTable()
{
    //QStringList parameters;
    //QHash<QString, QVector<double>> data;

    QList<QPair<QString, QVector<double>>> data;

    data.append(QPair<QString, QVector<double>>(mEIsAvaliable ? (settings->value("POTENTIAL_UNITS", false).toBool() ? "E, V" : "E, mV") : QString(), mEIsAvaliable ? mE : QVector<double>()));
    data.append(QPair<QString, QVector<double>>(mIIsAvaliable ? (settings->value("CURRENT_UNITS", false).toBool() ? "i, A" : "i, mA") : QString(), mIIsAvaliable ? mI : QVector<double>()));
    data.append(QPair<QString, QVector<double>>(mTIsAvaliable ? "t, s" : QString(), mTIsAvaliable ? mT : QVector<double>()));
    data.append(QPair<QString, QVector<double>>(mEsmoothedIsAvaliable ? (settings->value("POTENTIAL_UNITS", false).toBool() ? "E (smoothed), V" : "E (smoothed), mV") : QString(), mEsmoothedIsAvaliable ? mEsmoothed : QVector<double>()));
    data.append(QPair<QString, QVector<double>>(mIsmoothedIsAvaliable ? (settings->value("CURRENT_UNITS", false).toBool() ? "i (smoothed), A" : "i (smoothed), mA") : QString(), mIsmoothedIsAvaliable ? mIsmoothed : QVector<double>()));
    data.append(QPair<QString, QVector<double>>(mIsiIsAvaliable ? "i (semi-integral)" : QString(), mIsiIsAvaliable ? mIsi : QVector<double>()));
    data.append(QPair<QString, QVector<double>>(mIsdIsAvaliable ? "i (semi-differential)" : QString(), mIsdIsAvaliable ? mIsd : QVector<double>()));

//    parameters.append(mEIsAvaliable ? (settings->value("POTENTIAL_UNITS", false).toBool() ? "E, V" : "E, mV") : NULL);
//    parameters.append(mIIsAvaliable ? (settings->value("CURRENT_UNITS", false).toBool() ? "i, A" : "i, mA") : NULL);
//    parameters.append(mTIsAvaliable ? "t, s" : NULL);
//    parameters.append(mEsmoothedIsAvaliable ? (settings->value("POTENTIAL_UNITS", false).toBool() ? "E (smoothed), V" : "E (smoothed), mV") : NULL);
//    parameters.append(mIsmoothedIsAvaliable ? (settings->value("CURRENT_UNITS", false).toBool() ? "i (smoothed), A" : "i (smoothed), mA") : NULL);
//    parameters.append(mIsiIsAvaliable ? "i (semi-integral)" : NULL);
//    parameters.append(mIsdIsAvaliable ? "i (semi-differential)" : NULL);

    data.removeAll(QPair<QString, QVector<double>>(QString(), QVector<double>()));
    return data;
}

void CVCurveData::initializeParameters()
{
    mEmax = 0;
    mEmin = 0;
    mErange = 0;
    mImax = 0;
    mImin = 0;
    mIrange = 0;
    mTmax = 0;
    mTmin = 0;
    mTrange = 0;
    mScanRate = 0;
    actualPotentialShift = 0;
    mEIsAvaliable = false;
    mIIsAvaliable = false;
    mTIsAvaliable = false;
    mIsiIsAvaliable = false;
    mIsdIsAvaliable = false;
    mIsmoothedIsAvaliable = false;
    mEsmoothedIsAvaliable = false;
    mTcorrIsAvaliable = false;

    curveMargin = settings->value("CURVE_MARGIN", 0.07).toDouble();
}

//void CVCurveData::correctTime() // УДАЛИТЬ
//{

//    QVector<double> rowNumbers;
//    long double a = 0, b = 0;
//    long double sumXY = 0, sumY = 0, sumX = 0;
//    long double sumX2 = 0;
//    int n = mT.size();

//    for (int i = 0; i < n; i++)
//    {
//        rowNumbers.append(i/100);
//    }

//    for (int i = 0; i < n; i++)
//    {
//        sumX += i;
//        sumX2 += i*i;
//        sumY += mT[i];
//        sumXY += i*mT[i];
//    }

//    a = n*sumXY;
//    a -= sumX*sumY;
//    a /= (n*sumX2 - sumX*sumX);

//    b = sumY - a*sumX;
//    b /= n;

//    for (int i = 0; i < n; i++)
//        mTcorr.append(a*i + b);

//    mTcorrIsAvaliable = true;
//}

QVector<double> CVCurveData::vectorWithCoefsForSavGolFilter(int m, int pol_order)
{
    int frame_size = 2 * m + 1;

    MatrixXd vandermort_matrix(frame_size, pol_order + 1);
    MatrixXd vandermort_matrix_T(pol_order + 1, frame_size);
    int current_element = (1 - frame_size) / 2;

    for (int i = 0; i < pol_order + 1; i++)
    {
        for (int k = 0; k < frame_size; k++)
        {
            vandermort_matrix(k, i) = pow(current_element, i);
            vandermort_matrix_T(i, k) = pow(current_element, i);
            current_element++;
        }
        current_element = (1 - frame_size) / 2;
    }

    MatrixXd resulted_matrix = (vandermort_matrix_T*vandermort_matrix).inverse()*vandermort_matrix_T;

    QVector<double> coef(frame_size);
    for (int i = 0; i < frame_size; i++)
        coef[i] = (resulted_matrix(0, i));

    return coef;
}

int CVCurveData::getDataBeginnigStringNumber()
{
    QStringList currentLine;
    int i = 0;

    for (; i < mFileBody.size(); i++)
        if (mFileBody.at(i).contains("Ewe/V") && mFileBody.at(i).contains("I/mA")) break;

    return i+1;
}

bool CVCurveData::convolute()
{
    qDebug() << "convolute() " << QThread::currentThread();
    int Ru = 0, Cd = 0; // get from registry

    this->mEcorr.clear();
    this->mIcorr.clear();
    this->mIsd.clear();
    this->mIsi.clear();

    QVector<double> I = mIsmoothedIsAvaliable ? this->mIsmoothed : this->mI; // если доступны сглаженные данные, то программа использует их
    QVector<double> E;

    if (mEsmoothedIsAvaliable)
        E = mEsmoothed;
    else
    {
        SavGolFilter(10, 1, CVCurveData::E_vs_T);
        E = mEsmoothed;
    }

    double currentSum = 0;
    unsigned long int numberOfSteps = (I.size() - 1) * (I.size() - 2) / 2;
    unsigned long int currentStep = 0;
    double deltaT = mT.last()/mT.size();

    double sqtr_deltaT = sqrt(deltaT);
    double inversed_sqrt_pi = 1/sqrt(M_PI);
    const int I_size = I.size();

    double term_A[I_size] = {0}; // term_A = (I[k-1] + I[k])/2 * sqtr_deltaT
    double term_B[I_size] = {0}; // term_B = 1 / sqrt(i - k + 0.5)

    for (int i = 1; i < I_size; i++)
    {
        term_A[i] = (I[i-1] + I[i])/2 * sqtr_deltaT * inversed_sqrt_pi;
        term_B[i-1] = 1 / sqrt((i - 1) + 0.5);
    }

    for (int i = 1; i < I_size; i++)
    {
        // Algorithm H
        currentSum = 0;
        for (int k = 1; k <= i; k++)
        {
            currentSum += term_A[k] * term_B[i-k];
            currentStep++;
        }
        mIsi.append(currentSum);
        emit progressWasChanged((int)(100*(double)currentStep/numberOfSteps));
    }
    mIsiIsAvaliable = true;

    mIsiMin = *std::min_element(mIsi.begin(), mIsi.end());
    mIsiMax = *std::max_element(mIsi.begin(), mIsi.end());
    mIsiRange = mIsiMax - mIsiMin;
    //axisRanges.insert(PlotTypes.at(I_vs_E_semiint), Range(ERangeMin(), ERangeMax(), IsiRangeMin(), IsiRangeMax()));

    for (int i = 0; i < mIsi.size() - 1; i++)
    {
        mIsd.append((mIsi[i+1] - mIsi[i]) / (E[i+1] - E[i]) * copysign(1.0, E[i+1] - E[i]));
    }
    mIsdIsAvaliable = true;

    mIsdMin = *std::min_element(mIsd.begin(), mIsd.end());
    mIsdMax = *std::max_element(mIsd.begin(), mIsd.end());
    mIsdRange = mIsdMax - mIsdMin;
    //axisRanges.insert(PlotTypes.at(I_vs_E_semidiff), Range(ERangeMin(), ERangeMax(), IsdRangeMin(), IsdRangeMax()));

    return true;
}

bool CVCurveData::startConvolution()
{
    qDebug() << "startConvolution() " << QThread::currentThread();
    QFuture<bool> f = QtConcurrent::run(this, convolute);
    f.waitForFinished();
    return f.result();
}

QString CVCurveData::exportParametersAsText()
{
    QString text;

    text += mEIsAvaliable ? "E, V" : NULL;
    text += mIIsAvaliable ? "\tI, mA" : NULL;
    text += mTIsAvaliable ? "\tt, s" : NULL;
    text += mTcorrIsAvaliable ? "\tt (corrected), s" : NULL;
    text += mEsmoothedIsAvaliable ? "\tE (smoothed), V" : NULL;
    text += mIsmoothedIsAvaliable ? "\tI (smoothed), mA" : NULL;
    text += mIsiIsAvaliable ? "\tI (semiint)" : NULL;
    text += mIsdIsAvaliable ? "\tI (semidiff)" : NULL;

    for (int i = 0; i < mE.size(); i++)
    {
        text += "\n";

        if (mEIsAvaliable)
            text += QString::number(mE[i]);

        if (mIIsAvaliable)
            text += QString("\t%1").arg(mI[i]);

        if (mTIsAvaliable)
            text += QString("\t%1").arg(mT[i]);

        if (mTcorrIsAvaliable)
            text += QString("\t%1").arg(mTcorr[i]);

        if (mEsmoothedIsAvaliable)
            text += QString("\t%1").arg(mEsmoothed[i]);

        if (mIsmoothedIsAvaliable)
            text += QString("\t%1").arg(mIsmoothed[i]);

        if (mIsiIsAvaliable)
            if (i < mIsi.size())
                text += QString("\t%1").arg(mIsi[i]);

        if (mIsdIsAvaliable)
            if (i < mIsd.size())
                text += QString("\t%1").arg(mIsd[i]);
    }

    return text;
}

void CVCurveData::SavGolFilter(int m, int pol_order, CVCurveData::PlotType plotType)
{
    // avaliable plotTypes:
    // * I_vs_E
    // * E_vs_T

    QVector<double> coefs = vectorWithCoefsForSavGolFilter(m, pol_order);
    QVector<double> *activeVector;
    QVector<double> *resultVector;
    int activeVectorSize;
    double current_value = 0;

    if (plotType == CVCurveData::I_vs_E)
    {
        activeVector = &mI;
        resultVector = &mIsmoothed;
        mIsmoothedIsAvaliable = true;
    }
    else if (plotType == CVCurveData::E_vs_T)
    {
        activeVector = &mE;
        resultVector = &mEsmoothed;
        mEsmoothedIsAvaliable = true;
    }

    if (activeVector->isEmpty()) return;

    activeVectorSize = activeVector->size();
    resultVector->clear();
    for (int i = 0; i < m; i++)
        resultVector->append(activeVector->at(i));

    for (int i = m; i < activeVectorSize - m; i++)
    {
        for (int k = -m; k <= m; k++)
            current_value += activeVector->at(i+k)*coefs[k+m];

        resultVector->append(current_value);
        current_value = 0;
    }

    for (int i = activeVectorSize - m; i < activeVectorSize; i++)
        resultVector->append(activeVector->at(i));

}

void CVCurveData::changeFileName(QString newFileName)
{
    mFileName = newFileName;
}

void CVCurveData::correctPotentialValues(GeneralSettingsDialog *generalSettings)
{
    const int settingsShiftValue = generalSettings->potentialCorrelationCoeffitient;
    const int shift = settingsShiftValue - actualPotentialShift;
    bool potentialUnits = generalSettings->potentialUnits;

    for (int i = 0; i < mE.size(); i++)
    {
        mE[i] = mE[i] + (potentialUnits ? (double)shift/1000 : shift);
    }

    mEmin = *std::min_element(mE.begin(), mE.end());
    mEmax = *std::max_element(mE.begin(), mE.end());
    mErange = mEmax - mEmin;

    if (!mEsmoothed.isEmpty())
    {
        for (int i = 0; i < mEsmoothed.size(); i++)
        {
            mEsmoothed[i] = mEsmoothed[i] + (potentialUnits ? (double)shift : shift);
        }
    }

    actualPotentialShift += shift;
}

void CVCurveData::correctPotentialValues()
{
    settings->sync();
    const int shift = settings->value("POTENTIAL_CORRELATION_COEF").toInt();
    bool potentialUnits = settings->value("POTENTIAL_UNITS").toBool();

    for (int i = 0; i < mE.size(); i++)
    {
        mE[i] = mE[i] + (potentialUnits ? (double)shift/1000 : shift);
    }

    actualPotentialShift += shift;
}
