//
// Settings dialog.
//
#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>

#include "station.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog( QWidget * parent = 0 );
        ~SettingsDialog();

        void setStationList( const QList< Station > & list );
        QList< Station > getStationList() const;

    public slots:
        void updateStationsTable();
        void removeStation();
        void moveUpStation();
        void moveDownStation();
        void appendStation();
        void editStation();
        bool getSelection();
        void restoreSelection();
        bool checkStation( Station station );

    private:
        Ui::SettingsDialog * ui;
        int selectedStation;
        bool isSelection;
        QList< Station > stationList;
};

#endif
