#ifndef SMOOTHSETTINGSDIALOG_H
#define SMOOTHSETTINGSDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class SmoothSettingsDialog;
}

class SmoothSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SmoothSettingsDialog(QWidget *parent = nullptr);
    ~SmoothSettingsDialog();

signals:
    void sendFilterParameters(int m, int polynomial_order);

    void ButtonClickedSignal(bool buttonType);

private slots:
    void on_m_spinBox_valueChanged(int arg1);

    void on_OK_Button_clicked();

    void on_Cancel_Button_clicked();

    void setInitialParameters();

    void closeEvent(QCloseEvent *event);

public slots:
    void on_pol_order_spinBox_valueChanged(int arg1);


private:
    Ui::SmoothSettingsDialog *ui;
};

#endif // SMOOTHSETTINGSDIALOG_H
