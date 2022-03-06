#include "showdatadialog.h"
#include "ui_showdatadialog.h"

ShowDataDialog::ShowDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowDataDialog)
{
    ui->setupUi(this);
    settings = new QSettings("ORG335a", "CVProc", this);
    setWindowTitle("Data");

    // Table widget configuration
    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight);
    ui->tableWidget->setStyleSheet("QHeaderView::section"
                                   "{ padding: 4px;"
                                   "border-style: none;"
                                   "border-bottom: 1px solid #8c8c8c;"
                                   "border-right: 1px solid #8c8c8c; }"
                                   "QTableWidget QTableCornerButton::section"
                                   "{ background-color: #fce2cf;"
                                   "border-style: none;"
                                   "border-bottom: 1px solid #8c8c8c;"
                                   "border-right: 1px solid #8c8c8c; }");
    ui->tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section { background-color: #fce2cf }");
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #fce2cf }");
    ui->tableWidget->horizontalHeader()->setFixedHeight(25);
    ui->tableWidget->horizontalHeader()->setMinimumSectionSize(60);
//    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
//    ui->tableWidget->verticalHeader()->setHighlightSections(false);
}

ShowDataDialog::~ShowDataDialog()
{
    delete ui;
}

void ShowDataDialog::setCVData(CVCurveData *incomingCVData)
{
    CVData = incomingCVData;
}

void ShowDataDialog::fillTable()
{
    ui->tableWidget->clear();
    settings->sync();

    QList<QPair<QString, QVector<double>>> dataForTable = CVData->getAvaliableDataForTable();
    int columnCount = dataForTable.size();
    int rowCount = CVData->I().size();

    QStringList headerLabels;
    for (QList<QPair<QString, QVector<double>>>::iterator it = dataForTable.begin(); it != dataForTable.end(); it++)
        headerLabels.append((*it).first);

    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    QList<QPair<QString, QVector<double>>>::iterator it = dataForTable.begin();
    for (int column = 0; column < columnCount; column++)
    {
        for (int row = 0; row < rowCount; row++)
        {
            ui->tableWidget->setItem(row, column, new QTableWidgetItem((row < (*it).second.size()) ? QString::number((*it).second.at(row)) : QString()));
            ui->tableWidget->setRowHeight(row, 15);
        }
        it++;
    }

    ui->tableWidget->resizeColumnsToContents();
}

void ShowDataDialog::on_CopyButton_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    int columnCount = ui->tableWidget->columnCount();
    int rowCount = ui->tableWidget->rowCount();
    QString text;

    for (int column = 0; column < columnCount; column++)
        QTextStream(&text) << ui->tableWidget->horizontalHeaderItem(column)->text() << "\t";

    QTextStream(&text) << "\n";

    for (int row = 0; row < rowCount; row++)
    {
        for (int column = 0; column < columnCount; column++)
            QTextStream(&text) << ui->tableWidget->item(row, column)->text() << "\t";

        QTextStream(&text) << "\n";
    }

    clipboard->setText(text);
}

