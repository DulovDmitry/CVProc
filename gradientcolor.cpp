#include "gradientcolor.h"

GradientColor::GradientColor()
{
    qDebug() << "GradientColor constructor blank";
}

GradientColor::GradientColor(int numberOfGradientColorsNeeded) : numberOfGradientColors(numberOfGradientColorsNeeded)
{
    std::srand(std::time(nullptr));
    gradientName = GradientName(std::rand() % GradientColor::NUMBER_OF_GRADIENT_NAMES);
    fillInputColorsList();
    fillGradientColorsList();
    qDebug() << "GradientColor constructor";
}

GradientColor::GradientColor(GradientName _gradientName, int numberOfGradientColorsNeeded) : gradientName(_gradientName), numberOfGradientColors(numberOfGradientColorsNeeded)
{
    qDebug() << "GradientColor constructor";
    fillInputColorsList();
    fillGradientColorsList();
}

const QList<QColor> GradientColor::getGradientColorsList()
{
    return gradientColorsList;
}

void GradientColor::fillInputColorsList()
{
    switch (gradientName)
    {
    case RedToGreen:
        inputColorsList.append(QColor(255, 0, 0));
        inputColorsList.append(QColor(0, 255, 0));
        break;

    case RedToBlue:
        inputColorsList.append(QColor(255, 0, 0));
        inputColorsList.append(QColor(0, 0, 255));
        break;

    case OrangeToViolet:
        inputColorsList.append(QColor(255, 128, 0));
        inputColorsList.append(QColor(131, 0, 255));
        break;

    case CyanToMagenta:
        inputColorsList.append(QColor(0, 255, 255));
        inputColorsList.append(QColor(255, 0, 255));
        break;

    case MagentaToLime:
        inputColorsList.append(QColor(255, 0, 255));
        inputColorsList.append(QColor(128, 255, 0));
        break;

    case CyanToOrangeToMagenta:
        inputColorsList.append(QColor(64, 224, 208));
        inputColorsList.append(QColor(255, 128, 0));
        inputColorsList.append(QColor(255, 0, 255));
        break;

    case OrangeToGreenToDarkblue:
        inputColorsList.append(QColor(255, 128, 0));
        inputColorsList.append(QColor(128, 255, 0));
        inputColorsList.append(QColor(0, 0, 255));
        break;

    default:
        inputColorsList.append(QColor(0, 0, 0));
        break;
    }
}

void GradientColor::fillGradientColorsList()
{
    QColor currentGradColor;
    int numberOfSections = inputColorsList.size() - 1;
    double normalizedStepLenght = (double)numberOfSections/(numberOfGradientColors - 1);
    double currentDistance = 0;

    for (int i = 0; i < numberOfGradientColors; i++)
    {
        if (numberOfSections == (int)currentDistance)
        {
            gradientColorsList.append(inputColorsList.at(numberOfSections));
            break;
        }

        int section = floor(currentDistance);
        double currentSectionPoint = currentDistance - floor(currentDistance);
        QColor color1 = inverseSRGBCompanding(inputColorsList.at(section));
        QColor color2 = inverseSRGBCompanding(inputColorsList.at(section + 1));

        currentGradColor.setRed(color1.red() * (1 - currentSectionPoint) + color2.red() * currentSectionPoint);
        currentGradColor.setGreen(color1.green() * (1 - currentSectionPoint) + color2.green() * currentSectionPoint);
        currentGradColor.setBlue(color1.blue() * (1 - currentSectionPoint) + color2.blue() * currentSectionPoint);
        currentGradColor = sRGBCompanding(currentGradColor);
        qDebug() << currentDistance << currentSectionPoint << currentGradColor;
        gradientColorsList.append(currentGradColor);

        currentDistance += normalizedStepLenght;
    }


//    QColor color1 = inverseSRGBCompanding(inputColorsList.at(0));
//    QColor color2 = inverseSRGBCompanding(inputColorsList.at(1));
//    QColor currentGradColor;

//    for (int i = 0; i < numberOfGradientColors; i++)
//    {
//        double point = (double)i/(numberOfGradientColors - 1);

//        currentGradColor.setRed(color1.red() * (1 - point) + color2.red() * point);
//        currentGradColor.setGreen(color1.green() * (1 - point) + color2.green() * point);
//        currentGradColor.setBlue(color1.blue() * (1 - point) + color2.blue() * point);

//        currentGradColor = sRGBCompanding(currentGradColor);
//        qDebug() << point << currentGradColor;
//        gradientColorsList.append(currentGradColor);
//    }
}

QColor GradientColor::sRGBCompanding(QColor color)
{
    //Convert color from 0..255 to 0..1
    double r = color.red() / 255.0;
    double g = color.green() / 255.0;
    double b = color.blue() / 255.0;

    //Apply companding to Red, Green, and Blue
    if (r > 0.0031308) r = 1.055*pow(r, 1.0/gamma)-0.055; else r = r * 12.92;
    if (g > 0.0031308) g = 1.055*pow(g, 1.0/gamma)-0.055; else g = g * 12.92;
    if (b > 0.0031308) b = 1.055*pow(b, 1.0/gamma)-0.055; else b = b * 12.92;

    //return new color. Convert 0..1 back into 0..255
    QColor result(r*255, g*255, b*255);
    return result;
}

QColor GradientColor::inverseSRGBCompanding(QColor color)
{
    //Convert color from 0..255 to 0..1
    double r = color.red() / 255.0;
    double g = color.green() / 255.0;
    double b = color.blue() / 255.0;

    //Inverse Red, Green, and Blue
    if (r > 0.04045) r = pow((r+0.055)/1.055, gamma); else r = r / 12.92;
    if (g > 0.04045) g = pow((g+0.055)/1.055, gamma); else g = g / 12.92;
    if (b > 0.04045) b = pow((b+0.055)/1.055, gamma); else b = b / 12.92;

    //return new color. Convert 0..1 back into 0..255
    QColor result(r*255, g*255, b*255);
    return result;
}
