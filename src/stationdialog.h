//
// Station dialog.
//
#ifndef STATION_DIALOG_H
#define STATION_DIALOG_H

#include <QDialog>

#include "station.h"

namespace Ui {
    class StationDialog;
}

class StationDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit StationDialog( QWidget * parent = 0 );
        ~StationDialog();

        void setStation( const Station & station );
        Station getStation() const;

    private:
        Ui::StationDialog * ui;
};

#endif
