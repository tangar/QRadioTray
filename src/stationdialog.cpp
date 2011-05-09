//
// Station dialog.
//
#include "stationdialog.h"
#include "ui_stationdialog.h"

StationDialog::StationDialog( QWidget * parent )
    :QDialog( parent ),
     ui( new Ui::StationDialog )
{
    ui->setupUi( this );
}

StationDialog::~StationDialog()
{
    delete ui;
}

Station StationDialog::getStation() const
{
    Station station;
    station.name = ui->name->text();
    station.url = ui->url->text();
    station.description = ui->description->text();

    switch ( ui->encoding->currentIndex() )
    {
        case 0 : station.encoding = "UTF-8"       ; break;
        case 1 : station.encoding = "Windows-1251"; break;
        default: station.encoding = "UTF-8"       ; break;
    }

    return station;
}

void StationDialog::setStation( const Station & station )
{
    ui->name->setText( station.name );
    ui->description->setText( station.description );
    ui->url->setText( station.url );

    if ( station.encoding == "UTF-8" )
        ui->encoding->setCurrentIndex( 0 );
    else if ( station.encoding == "Windows-1251" )
        ui->encoding->setCurrentIndex( 1 );
    else
        ui->encoding->setCurrentIndex( 0 );
}
