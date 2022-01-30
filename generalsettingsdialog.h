#ifndef GENERALSETTINGSDIALOG_H
#define GENERALSETTINGSDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSettings>
#include <QColor>
#include <QColorDialog>
#include <QFontDialog>

//#include "mainwindow.h"

namespace Ui {
class GeneralSettingsDialog;
}

class GeneralSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeneralSettingsDialog(QWidget *parent = nullptr);
    ~GeneralSettingsDialog();

    friend class MainWindow;
    friend class CVCurveData;

private slots:
    void loadSettings();
    void saveSettings();

    void on_CurveColor_Button_clicked();

    void on_SelectionColor_Button_clicked();

    void on_SmoothColor_Button_clicked();

    void on_OK_Button_clicked();

    void on_LineWidth_spinBox_valueChanged(int arg1);

    void on_AxisWidth_SpinBox_valueChanged(double arg1);

    void on_Grid_checkBox_stateChanged(int arg1);

    void on_TicksIn_radioButton_clicked();

    void on_TicksOut_radioButton_clicked();

    void on_curve_margin_SpinBox_valueChanged(double arg1);

    void on_FontButton_clicked();

    void on_ApplyButton_clicked();

signals:
    void applyButtonClicked();

private:
    Ui::GeneralSettingsDialog *ui;
    QSettings *settings;

    // Colors
    QColor *curveColor;
    QColor *selectionColor;
    QColor *smoothColor;

    // *** Registry values ***
    
    // General
    bool decimalSeparator; // 0 - comma, 1 - dot
    bool currentUnits; // 0 - mA, 1 - A
    bool potentialUnits; // 0 - mV, 1 - V
    int RuDefaultValue;
    int CdDefaultValue;
    int potentialCorrelationCoeffitient;
    QString potentialAxisCaption;
    
    // Graphics
    QString curveColorName;
    QString selectionColorName;
    QString smoothColorName;
    bool ticksIn;
    bool gridIsVisible;
    int plotLineWidth;
    double axisWidth;
    double curveMargin;
    int axisFontSize;
    QString axisFontFamily;
    
    // Export

};

#endif // GENERALSETTINGSDIALOG_H
