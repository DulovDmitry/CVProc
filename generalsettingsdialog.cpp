#include "generalsettingsdialog.h"
#include "ui_generalsettingsdialog.h"

GeneralSettingsDialog::GeneralSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralSettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");

    settings = new QSettings("ORG335a", "CVProc", this);
    loadSettings();

    curveColor = new QColor(curveColorName);
    selectionColor = new QColor(selectionColorName);
    smoothColor = new QColor(smoothColorName);


    // *** Initialization of elements ***
    // General tab
    ui->DecimalSeparator_comboBox->setCurrentText(decimalSeparator ? "." : ",");
    ui->CurrentUnits_comboBox->setCurrentText(currentUnits ? "A" : "mA");
    ui->PotentialUnits_comboBox->setCurrentText(potentialUnits ? "V" : "mV");
    ui->correlationCoef_spinBox->setValue(potentialCorrelationCoeffitient);
    ui->potentialAxisCaption_lineEdit->setText(potentialAxisCaption);
    ui->Ru_spinBox->setValue(RuDefaultValue);
    ui->Cd_spinBox->setValue(CdDefaultValue);
    ui->I_vs_E_checkBox->setChecked(true);  // has to be configured
    ui->I_vs_E_checkBox->setEnabled(false); //
    ui->E_vs_T_checkBox->setChecked(true);  //

    // Graphics tab
    ui->CurveColor_Button->setStyleSheet("QPushButton{background-color: " + curveColor->toRgb().name() + "; border: 1px solid lightgray; border-radius: 5px}");
    ui->SelectionColor_Button->setStyleSheet("QPushButton{background-color: " + selectionColor->toRgb().name() + "; border: 1px solid lightgray; border-radius: 5px}");
    ui->SmoothColor_Button->setStyleSheet("QPushButton{background-color: " + smoothColor->toRgb().name() + "; border: 1px solid lightgray; border-radius: 5px}");
    ui->TicksIn_radioButton->setChecked(ticksIn);
    ui->TicksOut_radioButton->setChecked(!ticksIn);
    ui->Grid_checkBox->setChecked(gridIsVisible);
    ui->LineWidth_spinBox->setValue(plotLineWidth);
    ui->FontLabel->setFont(QFont(axisFontFamily, axisFontSize));
    ui->FontLabel->setText(axisFontFamily + ", " + QString::number(axisFontSize));
    ui->AxisWidth_SpinBox->setValue(axisWidth);
    ui->curve_margin_SpinBox->setValue(curveMargin);
    ui->hint_label->setPixmap(QIcon(":/new/prefix1/hintIcon.ico").pixmap(QSize(16, 16)));


    // Export tab

}

GeneralSettingsDialog::~GeneralSettingsDialog()
{
    delete ui;
}

void GeneralSettingsDialog::loadSettings()
{
    // General settings
    decimalSeparator = settings->value("DECIMAL_SEPARATOR", false).toBool(); 
    currentUnits = settings->value("CURRENT_UNITS", false).toBool();
    potentialUnits = settings->value("POTENTIAL_UNITS", false).toBool(); 
    RuDefaultValue = settings->value("Ru_VALUE", 0).toInt();
    CdDefaultValue = settings->value("Cd_VALUE", 0).toInt();
    potentialCorrelationCoeffitient = settings->value("POTENTIAL_CORRELATION_COEF", 0).toInt();
    potentialAxisCaption = settings->value("POTENTIAL_AXIS_CAPTION", "E").toString();

    // Graphics settings
    curveColorName = settings->value("CURVE_COLOR", "#000000").toString();
    selectionColorName = settings->value("SELECTION_COLOR", "#000000").toString();
    smoothColorName = settings->value("SMOOTH_COLOR", "#000000").toString();
    ticksIn = settings->value("TICKS_IN", false).toBool();
    gridIsVisible = settings->value("GRID_IS_VISIBLE", true).toBool();
    plotLineWidth = settings->value("PLOT_LINE_WIDTH", 2).toInt();
    axisWidth = settings->value("AXIS_WIDTH", 1.5).toDouble();
    curveMargin = settings->value("CURVE_MARGIN", 0.07).toDouble();
    axisFontSize = settings->value("AXIS_FONT_SIZE", 12).toInt();
    axisFontFamily = settings->value("AXIS_FONT_FAMILY", "Arial").toString();

    // Export settings

}

void GeneralSettingsDialog::saveSettings()
{
    // General settings
    settings->setValue("DECIMAL_SEPARATOR", decimalSeparator);
    settings->setValue("CURRENT_UNITS", currentUnits);
    settings->setValue("POTENTIAL_UNITS", potentialUnits);
    settings->setValue("Ru_VALUE", RuDefaultValue);
    settings->setValue("Cd_VALUE", CdDefaultValue);
    settings->setValue("POTENTIAL_CORRELATION_COEF", potentialCorrelationCoeffitient);
    settings->setValue("POTENTIAL_AXIS_CAPTION", potentialAxisCaption);

    // Graphics settings
    settings->setValue("CURVE_COLOR", curveColorName);
    settings->setValue("SELECTION_COLOR", selectionColorName);
    settings->setValue("SMOOTH_COLOR", smoothColorName);
    settings->setValue("TICKS_IN", ticksIn);
    settings->setValue("GRID_IS_VISIBLE", gridIsVisible);
    settings->setValue("PLOT_LINE_WIDTH", plotLineWidth);
    settings->setValue("AXIS_WIDTH", axisWidth);
    settings->setValue("CURVE_MARGIN", curveMargin);
    settings->setValue("AXIS_FONT_SIZE", axisFontSize);
    settings->setValue("AXIS_FONT_FAMILY", axisFontFamily);

    // Export settings


    settings->sync();

}

void GeneralSettingsDialog::on_CurveColor_Button_clicked()
{
    *curveColor = QColorDialog::getColor(QColor(curveColorName), this, "Choose color");
    if (!curveColor->isValid())
    {
        *curveColor = QColor(curveColorName);
        return;
    }
    curveColorName = curveColor->toRgb().name();
    ui->CurveColor_Button->setStyleSheet("QPushButton{background-color: " + curveColorName + "; border: 1px solid lightgray; border-radius: 5px}");
}

void GeneralSettingsDialog::on_SelectionColor_Button_clicked()
{
    *selectionColor = QColorDialog::getColor(QColor(selectionColorName), this, "Choose color");
    if (!selectionColor->isValid())
    {
        *selectionColor = QColor(selectionColorName);
        return;
    }
    selectionColorName = selectionColor->toRgb().name();
    ui->SelectionColor_Button->setStyleSheet("QPushButton{background-color: " + selectionColorName + "; border: 1px solid lightgray; border-radius: 5px}");
}

void GeneralSettingsDialog::on_SmoothColor_Button_clicked()
{
    *smoothColor = QColorDialog::getColor(QColor(smoothColorName), this, "Choose color");
    if (!smoothColor->isValid())
    {
        *smoothColor = QColor(smoothColorName);
        return;
    }
    smoothColorName = smoothColor->toRgb().name();
    ui->SmoothColor_Button->setStyleSheet("QPushButton{background-color: " + smoothColorName + "; border: 1px solid lightgray; border-radius: 5px}");
}

void GeneralSettingsDialog::on_OK_Button_clicked()
{
    // general settings fixing
    decimalSeparator = (ui->DecimalSeparator_comboBox->currentText() == ",") ? false : true;
    currentUnits = (ui->CurrentUnits_comboBox->currentText() == "mA") ? false : true;
    potentialUnits = (ui->PotentialUnits_comboBox->currentText() == "mV") ? false : true;
    potentialCorrelationCoeffitient = ui->correlationCoef_spinBox->value();
    potentialAxisCaption = ui->potentialAxisCaption_lineEdit->text();
    RuDefaultValue = ui->Ru_spinBox->value();
    CdDefaultValue = ui->Cd_spinBox->value();

    emit applyButtonClicked();
    saveSettings();
    this->hide();
}

void GeneralSettingsDialog::on_LineWidth_spinBox_valueChanged(int arg1)
{
    this->plotLineWidth = arg1;
}

void GeneralSettingsDialog::on_AxisWidth_SpinBox_valueChanged(double arg1)
{
    this->axisWidth = arg1;
}

void GeneralSettingsDialog::on_Grid_checkBox_stateChanged(int arg1)
{
    this->gridIsVisible = arg1;
}

void GeneralSettingsDialog::on_TicksIn_radioButton_clicked()
{
    this->ticksIn = ui->TicksIn_radioButton->isChecked();
}

void GeneralSettingsDialog::on_TicksOut_radioButton_clicked()
{
    this->ticksIn = ui->TicksIn_radioButton->isChecked();
}

void GeneralSettingsDialog::on_curve_margin_SpinBox_valueChanged(double arg1)
{
    this->curveMargin = arg1;
}

void GeneralSettingsDialog::on_FontButton_clicked()
{
    bool ok = true;
    QFont font = QFontDialog::getFont(&ok, QFont(axisFontFamily, axisFontSize));
    axisFontFamily = font.family();
    axisFontSize = font.pointSize();

    ui->FontLabel->setFont(QFont(axisFontFamily, axisFontSize));
    ui->FontLabel->setText(axisFontFamily + ", " + QString::number(axisFontSize));
}

void GeneralSettingsDialog::on_ApplyButton_clicked()
{
    saveSettings();
    emit applyButtonClicked();
}
