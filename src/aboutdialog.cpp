//
// About dialog.
//
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog( QWidget * parent )
    :QDialog( parent ),
     ui( new Ui::AboutDialog )
{
    ui->setupUi( this );
    //setWindowTitle( tr( "About QRadioTray " ) + tr( PROGRAM_VERSION ) );
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
