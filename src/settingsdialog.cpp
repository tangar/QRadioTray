//
// Settings dialog.
//
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "stationdialog.h"
#include "player.h"
#include "logger.h"
#include <QTimer>
#include <QUrl>

// maximal period of station test imn msec
#define MAX_TEST_TIMEOUT    5000

SettingsDialog::SettingsDialog( QWidget * parent )
    :QDialog( parent ),
     ui( new Ui::SettingsDialog ),
     selectedStation( -1 ),
     isSelection( false )
{
    ui->setupUi( this );
    ui->stationTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setStationList( const QList< Station > & list )
{
    stationList = list;
    updateStationsTable();
}

QList< Station > SettingsDialog::getStationList() const
{
    return stationList;
}

void SettingsDialog::removeStation()
{
    if ( getSelection() )
    {
        stationList.removeAt( selectedStation );
        updateStationsTable();
    }
}

void SettingsDialog::moveUpStation()
{
    if ( getSelection() )
    {
        if ( selectedStation > 0 )
        {
            stationList.swap( selectedStation, selectedStation - 1 );
            selectedStation -= 1;
            updateStationsTable();
        }
    }
}

void SettingsDialog::moveDownStation()
{
    if ( getSelection() )
    {
        if ( selectedStation < stationList.count() - 1 )
        {
            stationList.swap( selectedStation, selectedStation + 1 );
            selectedStation += 1;
            updateStationsTable();
        }
    }
}

void SettingsDialog::appendStation()
{
    StationDialog dialog;
    Station station;
    dialog.setStation( station );

    if ( dialog.exec() == QDialog::Accepted )
    {
        Station returnedStation = dialog.getStation();
        if ( checkStation( returnedStation ) )
        {
            stationList.append( dialog.getStation() );
            updateStationsTable();
        }

    }
}

void SettingsDialog::editStation()
{
    if ( getSelection() )
    {
        StationDialog dialog;
        dialog.setStation( stationList[ selectedStation ] );
        if ( dialog.exec() == QDialog::Accepted )
        {
            Station returnedStation = dialog.getStation();
            if ( checkStation( returnedStation ) )
            {
                stationList[ selectedStation ] = returnedStation;
                updateStationsTable();
            }
        }
    }
}

void SettingsDialog::updateStationsTable()
{
    ui->stationTable->clearContents();
    ui->stationTable->setRowCount( 0 );

    foreach ( const Station & station, stationList )
    {
        const int currRow = ui->stationTable->rowCount();
        ui->stationTable->insertRow( currRow );
        ui->stationTable->setItem( currRow, 0, new QTableWidgetItem( station.name ) );
        ui->stationTable->setItem( currRow, 1, new QTableWidgetItem( station.description ) );
        ui->stationTable->setItem( currRow, 2, new QTableWidgetItem( station.url ) );
        ui->stationTable->setItem( currRow, 3, new QTableWidgetItem( station.encoding ) );
    }

    restoreSelection();
}

bool SettingsDialog::getSelection()
{
    if ( ui->stationTable->selectedItems().count() > 0 )
    {
        selectedStation = ui->stationTable->selectedItems().first()->row();
        isSelection = true;
    }
    else
    {
        selectedStation = -1;
        isSelection = false;
    }

    return isSelection;
}

void SettingsDialog::restoreSelection()
{
    if ( isSelection && ( ui->stationTable->rowCount() > 0 ) )
    {
        if ( selectedStation < 0 )
            selectedStation = 0;
        else if ( selectedStation >= ui->stationTable->rowCount() )
            selectedStation = ui->stationTable->rowCount() - 1;
        ui->stationTable->selectRow( selectedStation );
    }
}

bool SettingsDialog::checkStation(Station station)
{
    Player testPlayer;
    QTimer timer;
    bool ret = false;

    LOG_DEBUG( "settings", tr( "Testing url \"%1\" ...." ).arg(station.url) );
    testPlayer.setUrl( QUrl( station.url ) );
    testPlayer.startPlay();
    timer.start(MAX_TEST_TIMEOUT);
    while( timer.isActive() )
    {
        qApp->processEvents();

        if ( testPlayer.isError()) {
            ret = false;
            break;
        }
        else if ( testPlayer.isPlaying() ) {
            ret = true;
            break;
        }
    }
    LOG_DEBUG( "settings", tr( "Url check result: %1" ).arg(ret) );

    return ret;
}
