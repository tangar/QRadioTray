#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "appenddialog.h"

#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->pushButton  ->setIcon(this->style()->standardIcon(QStyle::SP_ArrowUp));
    ui->pushButton_2->setIcon(this->style()->standardIcon(QStyle::SP_ArrowDown));
    ui->pushButton_3->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogInfoView));
    ui->pushButton_4->setIcon(this->style()->standardIcon(QStyle::SP_TrashIcon));
    ui->pushButton_5->setIcon(this->style()->standardIcon(QStyle::SP_DriveNetIcon));

    ui->tableWidget->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings(QSettings *settings)
{
    QStringList gList,kList;

    selectedRow = 0;
    selectedColumn = 0;
    isSelection = false;

    stationList.clear();
    settings->beginGroup( "STATIONS" );
    gList.clear();
    gList = settings->childGroups();
    foreach ( QString gStr, gList )
    {
        ui->tableWidget->insertRow(0);

        settings->beginGroup(gStr);
        Station *station = new Station;
        station->name = settings->value("name").toString();
        station->description = settings->value("description").toString();
        station->url = settings->value("url").toString();
        station->encoding = settings->value("encoding").toString();
        stationList.append(station);
        settings->endGroup();
    }
    settings->endGroup();

    if (stationList.length())
        this->popullateTable();

}

void SettingsDialog::removeRecord(void)
{
    // get current selection
    if (getSelection())
    {
        stationList.removeAt(selectedRow);
        this->popullateTable();
    }
}

void SettingsDialog::moveUp(void)
{
    if (ui->tableWidget->selectedItems().length() == 0)
        return;

    // get current selection
    getSelection();
    if ( selectedRow > 0)
    {                
        stationList.swap(selectedRow, selectedRow-1);
        selectedRow -= 1;
        this->popullateTable();
    }

}

void SettingsDialog::moveDown(void)
{
    if (ui->tableWidget->selectedItems().length() == 0)
        return;

    // get current selection
    getSelection();
    if ( selectedRow < stationList.length() -1 )
    {
        // отображение смещенния выбора ячеек        
        stationList.swap(selectedRow, selectedRow+1);
        selectedRow += 1;
        this->popullateTable();
    }
}

void SettingsDialog::appendRecord(void)
{
    AppendDialog dialog;
    Station *station = new Station();

    if (dialog.exec())
    {

        // get current selection
        getSelection();

        station->name = dialog.station.name;
        station->description = dialog.station.description;
        station->url = dialog.station.url;
        station->encoding = dialog.station.encoding;
        stationList.append(station);
        this->popullateTable();
    }
}

void SettingsDialog::editRecord(void)
{
    AppendDialog dialog;
    int selectionRow;

    // get current selection
    getSelection();

    if (ui->tableWidget->selectedItems().length() == 0)
        return;
    selectionRow = ui->tableWidget->selectedItems().first()->row();

    dialog.init(stationList.at(selectionRow));
    if (dialog.exec())
    {
        stationList.at(selectionRow)->name = dialog.station.name;
        stationList.at(selectionRow)->description = dialog.station.description;
        stationList.at(selectionRow)->url = dialog.station.url;
        stationList.at(selectionRow)->encoding = dialog.station.encoding;

        this->popullateTable();
    }
}

void SettingsDialog::popullateTable(void)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);

    foreach (Station * station, stationList)
    {
        int currRow = ui->tableWidget->rowCount();

        ui->tableWidget->insertRow(currRow);

        QTableWidgetItem *item1 = new QTableWidgetItem(station->name);
        item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QTableWidgetItem *item2 = new QTableWidgetItem(station->description);
        item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QTableWidgetItem *item3 = new QTableWidgetItem(station->url);
        item3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QTableWidgetItem *item4 = new QTableWidgetItem(station->encoding);
        item4->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        ui->tableWidget->setItem(currRow,0, item1);
        ui->tableWidget->setItem(currRow,1, item2);
        ui->tableWidget->setItem(currRow,2, item3);
        ui->tableWidget->setItem(currRow,3, item4);
    }

    // restopre selection
    restoreSelection();
}

bool SettingsDialog::getSelection(void)
{
    if (ui->tableWidget->selectedItems().length())
    {
        selectedRow = ui->tableWidget->selectedItems().first()->row();
        selectedColumn = ui->tableWidget->selectedItems().first()->column();
        isSelection = true;
    }
    else
    {
        selectedRow = 0;
        selectedColumn = 0;
        isSelection = false;
    }

    return isSelection;
}

void SettingsDialog::restoreSelection(void)
{
    if ( ( isSelection ) &&
         ( ui->tableWidget->rowCount() ) &&
         ( ui->tableWidget->columnCount() ) )
    {
        if ( ui->tableWidget->rowCount() <= selectedRow )
            selectedRow = ui->tableWidget->rowCount() - 1;
        if ( ui->tableWidget->columnCount() <= selectedColumn )
            selectedColumn = ui->tableWidget->columnCount() - 1;

        ui->tableWidget->item(selectedRow, selectedColumn)->setSelected(true);
    }
}
