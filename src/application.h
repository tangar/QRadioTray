//
// Application.
//
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QSettings>
#include <QMultiMap>
#include <QFile>
#include <QTextStream>

#include "settingsdialog.h"
#include "station.h"
#include "player.h"

class Application : public QApplication
{
    Q_OBJECT

    public:
        explicit Application( int & argc, char ** argv );
        ~Application();

        bool loadSettings();
        void storeSettings();
        bool configure();

    private slots:
        void onPlayerPlay();
        void onPlayerPause();
        void onPlayerStop();
        void onPlayerError();
        void onPlayerVolumeChanged( int volume );
        void onMetaDataChange( const QMultiMap< QString, QString > & data );
        void processStationAction( QAction * action );
        void animateIcon( quint64 tick );
        void about();
        void manageSettings();
        void updateStationsMenu();

    private:
        QSystemTrayIcon trayItem;
        QMenu trayMenu;
        QStringList trayIconList;
        int currTrayIcon;
        QMenu stationsMenu;
        Player player;
        QList< Station > stationList;
        Station lastStation;
        QActionGroup * stationsGroup;

        QString stopHotkey;
        QString pauseHotkey;
        QString volumeUpHotkey;
        QString volumeDownHotkey;
        QString quitHotkey;
};

#endif
