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

    ui->tableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings(QSettings *settings)
{
    QStringList gList,kList;

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
    int selectedRow;
    if (ui->tableWidget->selectedItems().length())
    {
        selectedRow = ui->tableWidget->selectedItems().first()->row();
        stationList.removeAt(selectedRow);
        this->popullateTable();
    }
}

void SettingsDialog::moveUp(void)
{
    int selectionRow;

    if (ui->tableWidget->selectedItems().length() == 0)
        return;

    selectionRow = ui->tableWidget->selectedItems().first()->row();
    if ( selectionRow )
    {        
        stationList.swap(selectionRow, selectionRow-1);
        this->popullateTable();
    }

}

void SettingsDialog::moveDown(void)
{
    int selectionRow;

    if (ui->tableWidget->selectedItems().length() == 0)
        return;

    selectionRow = ui->tableWidget->selectedItems().first()->row();
    if ( selectionRow < stationList.length()-1 )
    {
        // отображение смещенния выбора ячеек
        stationList.swap(selectionRow, selectionRow+1);
        this->popullateTable();
    }
}

void SettingsDialog::appendRecord(void)
{
    AppendDialog dialog;
    Station *station = new Station();

    if (dialog.exec())
    {

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
    int selectedRow, selectedCol;

//    // save table selection
//    QList<QTableWidgetItem *> selectedItems;
//    selectedItems = ui->tableWidget->selectedItems();
//    selectedRow = selectedItems.first()->row();
//    selectedCol = selectedItems.first()->column();

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);

    foreach (Station * station, stationList)
    {
        int currRow = ui->tableWidget->rowCount();

        ui->tableWidget->insertRow(currRow);

        QTableWidgetItem *item1 = new QTableWidgetItem(station->name);
        QTableWidgetItem *item2 = new QTableWidgetItem(station->description);
        QTableWidgetItem *item3 = new QTableWidgetItem(station->url);
        QTableWidgetItem *item4 = new QTableWidgetItem(station->encoding);



        ui->tableWidget->setItem(currRow,0, item1);
        ui->tableWidget->setItem(currRow,1, item2);
        ui->tableWidget->setItem(currRow,2, item3);
        ui->tableWidget->setItem(currRow,3, item4);
    }


//    // restopre selection
//    ui->tableWidget->item(selectedRow, selectedCol)->setSelected(true);


}




