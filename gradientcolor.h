#ifndef GRADIENTCOLOR_H
#define GRADIENTCOLOR_H

#include <QColor>
#include <QDebug>
#include <QMessageBox>
#include <cmath>
#include <ctime>

class GradientColor
{
public:    
    enum GradientName
    {
        RedToGreen,
        RedToBlue,
        OrangeToViolet,
        CyanToMagenta,
        MagentaToLime,
        CyanToOrangeToMagenta,
        OrangeToGreenToDarkblue,
        NUMBER_OF_GRADIENT_NAMES
    };

    GradientColor();
    GradientColor(int numberOfGradientColorsNeeded);
    GradientColor(GradientName gradientName, int numberOfGradientColorsNeeded);
    ~GradientColor() { qDebug() << "GradientColor destructor"; }

public slots:
    const QList<QColor> getGradientColorsList();

private slots:
    void fillInputColorsList();
    void fillGradientColorsList();
    QColor sRGBCompanding(QColor color);
    QColor inverseSRGBCompanding(QColor color);

private:
    GradientName gradientName;
    int numberOfGradientColors;
    const double gamma = 2.4;

    QList<QColor> inputColorsList;
    QList<QColor> gradientColorsList;
};

#endif // GRADIENTCOLOR_H
