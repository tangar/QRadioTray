//
// Application.
//
#include "application.h"
#include "aboutdialog.h"
#include "settingsdialog.h"
#include "logger.h"

#include <QUrl>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QxtGlobalShortcut>

// Config file.
#define CONFIG_FILE        "config.ini"

// Hotkeys.
#define STOP_HOTKEY        "Alt+X"
#define PAUSE_HOTKEY       "Alt+Z"
#define VOLUME_UP_HOTKEY   "Alt+W"
#define VOLUME_DOWN_HOTKEY "Alt+Q"

Application::Application( int & argc, char ** argv )
    :QApplication( argc, argv ),
     currTrayIcon( 0 )
{
}

Application::~Application()
{
}

bool Application::loadSettings()
{
    if ( !QFile::exists( CONFIG_FILE ) )
    {
        LOG_ERROR( "Application", tr( "No config file!" ) );
        QMessageBox::critical( 0, tr( "Error" ), tr( "No config file!" ) );
        return false;
    }

    QSettings settings( CONFIG_FILE, QSettings::IniFormat );
    settings.beginGroup( "STATIONS" );
    const int count = settings.beginReadArray( "station" );
    stationList.clear();
    for ( int  i = 0; i < count; ++i )
    {
        settings.setArrayIndex( i );
        Station station;
        station.name = settings.value( "name" ).toString();
        station.description = settings.value( "description" ).toString();
        station.url = settings.value( "url" ).toString();
        station.encoding = settings.value( "encoding" ).toString();
        stationList.append( station );
    }
    settings.endArray();
    settings.endGroup();

    return true;
}

void Application::storeSettings()
{
    QSettings settings( CONFIG_FILE, QSettings::IniFormat );
    settings.beginGroup( "STATIONS" );
    settings.beginWriteArray( "station" );
    for ( int i = 0; i < stationList.count(); ++i )
    {
        const Station & station = stationList[ i ];
        settings.setArrayIndex( i );
        settings.setValue( "name", station.name );
        settings.setValue( "description", station.description );
        settings.setValue( "url", station.url );
        settings.setValue( "encoding", station.encoding );
    }
    settings.endArray();
    settings.endGroup();
}

bool Application::configure()
{
    if ( !QSystemTrayIcon::isSystemTrayAvailable() )
    {
        LOG_ERROR( "Application", tr( "No system tray available!" ) );
        QMessageBox::critical( 0, tr( "Error" ), tr( "No system tray available!" ) );
        return false;
    }

    if ( !QSystemTrayIcon::supportsMessages() )
    {
        LOG_ERROR( "Application", tr( "System tray not support messages!" ) );
        QMessageBox::critical( 0, tr( "Error" ), tr( "System tray not support messages!" ) );
        return false;
    }

    // Tray icons set.
    trayIconList.append( ":/images/radio-active-2.png" );
    trayIconList.append( ":/images/radio-active-1.png" );
    trayIconList.append( ":/images/radio-active.png"   );
    currTrayIcon = 0;

    // Setup player.
    connect( &player, SIGNAL( playerTick( quint64 ) ), SLOT( animateIcon( quint64 ) ) );
    connect( &player, SIGNAL( playing() ), SLOT( onPlayerPlay() ) );
    connect( &player, SIGNAL( paused() ), SLOT( onPlayerPause() ) );
    connect( &player, SIGNAL( stopped() ), SLOT( onPlayerStop() ) );
    connect( &player, SIGNAL( errorOccured() ), SLOT( onPlayerError() ) );
    connect( &player, SIGNAL( volumeChanged( int ) ), SLOT( onPlayerVolumeChanged( int ) ) );
    connect( &player, SIGNAL( metaDataChanged( const QMultiMap< QString, QString > ) ),
                      SLOT ( onMetaDataChange( const QMultiMap< QString, QString > ) ) );

    // Setup global shortcuts.
    QxtGlobalShortcut * globalShortcut;
    globalShortcut = new QxtGlobalShortcut( &trayItem );
    if ( globalShortcut )
    {
        globalShortcut->setShortcut( QKeySequence( PAUSE_HOTKEY ) );
        connect( globalShortcut, SIGNAL( activated() ), &player, SLOT( playOrPause() ) );
    }
    globalShortcut = new QxtGlobalShortcut( &trayItem );
    if ( globalShortcut )
    {
        globalShortcut->setShortcut( QKeySequence( STOP_HOTKEY ) );
        connect( globalShortcut, SIGNAL( activated() ), &player, SLOT( stopPlay() ) );
    }
    globalShortcut = new QxtGlobalShortcut( &trayItem );
    if ( globalShortcut )
    {
        globalShortcut->setShortcut( QKeySequence( VOLUME_UP_HOTKEY ) );
        connect( globalShortcut, SIGNAL( activated() ), &player, SLOT( volumeUp() ) );
    }
    globalShortcut = new QxtGlobalShortcut( &trayItem );
    if ( globalShortcut )
    {
        globalShortcut->setShortcut( QKeySequence( VOLUME_DOWN_HOTKEY ) );
        connect( globalShortcut, SIGNAL( activated() ), &player, SLOT( volumeDown() ) );
    }

    // Create stations menu.
    stationsMenu.setTitle( tr("Stations" ) );
    stationsMenu.setIcon( QIcon( ":/images/radio-passive.png" ) );
    stationsGroup = new QActionGroup( &stationsMenu );
    if ( stationsGroup )
    {
        stationsGroup->setExclusive( true );
        updateStationsMenu();
        connect( stationsGroup, SIGNAL( triggered( QAction * ) ),
                                SLOT( processStationAction( QAction * ) ) );
    }

    // Create base menu.
    trayMenu.addMenu( &stationsMenu );
    trayMenu.addSeparator();
    QAction * action;
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/audio-volume-up.png" ) );
        action->setText( tr( "Volume up" ) );
        action->setShortcut( QKeySequence( VOLUME_UP_HOTKEY ) );
        connect( action, SIGNAL( triggered() ), &player, SLOT( volumeUp() ) );
        trayMenu.addAction( action );
    }
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/audio-volume-down.png" ) );
        action->setText( tr( "Volume down" ) );
        action->setShortcut( QKeySequence( VOLUME_DOWN_HOTKEY ) );
        connect( action, SIGNAL( triggered() ), &player, SLOT( volumeDown() ) );
        trayMenu.addAction( action );
    }
    trayMenu.addSeparator();
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/media-playback-start.png" ) );
        action->setText( tr( "Play|Pause" ) );
        action->setShortcut( QKeySequence( PAUSE_HOTKEY ) );
        connect( action, SIGNAL( triggered() ), &player, SLOT( playOrPause() ) );
        trayMenu.addAction( action );
    }
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/media-playback-stop.png" ) );
        action->setText( tr( "Stop" ) );
        action->setShortcut( QKeySequence( STOP_HOTKEY ) );
        connect( action, SIGNAL( triggered() ), &player, SLOT( stopPlay() ) );
        trayMenu.addAction( action );
    }
    trayMenu.addSeparator();
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/application-info.png" ) );
        action->setText( tr( "Info" ) );
        action->setMenuRole( QAction::PreferencesRole );
        connect( action, SIGNAL( triggered() ), this, SLOT( about() ) );
        trayMenu.addAction( action );
    }
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/application-settings.png" ) );
        action->setText( tr( "Settings" ) );
        action->setMenuRole( QAction::PreferencesRole );
        connect( action, SIGNAL( triggered() ), this, SLOT( manageSettings() ) );
        trayMenu.addAction( action );
    }
    action = new QAction( &trayMenu );
    if ( action )
    {
        action->setIcon( QIcon( ":/images/application-exit.png" ) );
        action->setText( tr( "Exit" ) );
        action->setMenuRole( QAction::QuitRole );
        connect( action, SIGNAL( triggered() ), this, SLOT( quit() ) );
        trayMenu.addAction( action );
    }

    // Setup tray item.
    trayItem.setContextMenu( &trayMenu );
    trayItem.setIcon( QIcon( ":/images/radio-passive.png" ) );
    trayItem.show();
    trayItem.showMessage( tr( "QRadioTray" ), tr( "Program started!" ), QSystemTrayIcon::Information );

    return true;
}

void Application::processStationAction( QAction * action )
{
    if ( !action )
        return;

    const int num = action->data().toInt();
    if ( ( num < 0 ) || ( num >= stationList.count() ) )
        return;

    lastStation = stationList[ num ];
    if ( lastStation.url != player.getSource() )
    {
        if ( player.isPlaying() || player.isPaused() )
        {
            player.stopPlay();
            player.setUrl( QUrl( lastStation.url ) );
            player.startPlay();
        }
        else
            player.setUrl( QUrl( lastStation.url ) );
        LOG_INFO( "Application", tr( "Station #%1 selected." ).arg( num ) );
    }
}

void Application::about()
{
    AboutDialog dialog;
    dialog.exec();
}

void Application::manageSettings()
{
    SettingsDialog dialog;
    dialog.setStationList( stationList );
    if ( dialog.exec() == QDialog::Accepted )
    {
        stationList = dialog.getStationList();
        storeSettings();
        updateStationsMenu();
    }
}

void Application::updateStationsMenu()
{
    if ( !stationsGroup )
        return;

    stationsGroup->actions().clear();
    stationsMenu.clear();
    for ( int i = 0; i < stationList.count(); ++i )
    {
        QAction * action = new QAction( stationsGroup );
        if ( action )
        {
            const Station & station = stationList[ i ];
            action->setText( station.name );
            action->setData( i );
            action->setToolTip( station.description );
            action->setCheckable( true );
            stationsMenu.addAction( action );
        }
    }
}

void Application::animateIcon( quint64 tick )
{
    Q_UNUSED( tick );

    if ( !player.isPlaying() )
        return;

    if ( currTrayIcon >= trayIconList.count() )
        currTrayIcon = 0;
    trayItem.setIcon( QIcon( trayIconList[ currTrayIcon ] ) );
    currTrayIcon++;
}

void Application::onPlayerPlay()
{
    trayItem.setIcon( QIcon( ":/images/radio-active.png" ) );
    trayItem.showMessage( tr( "QRadioTray" ), tr( "Radio is playing." ),
                          QSystemTrayIcon::Information );
    trayItem.setToolTip( tr( "Radio is playing." ) );
}

void Application::onPlayerPause()
{
    trayItem.setIcon( QIcon( ":/images/radio-passive.png" ) );
    trayItem.showMessage( tr( "QRadioTray" ), tr( "Radio is paused." ),
                          QSystemTrayIcon::Information );
    trayItem.setToolTip( tr( "Radio is paused." ) );
}

void Application::onPlayerStop()
{
    trayItem.setIcon( QIcon( ":/images/radio-passive.png" ) );
    trayItem.showMessage( tr( "QRadioTray" ), tr( "Radio stopped." ),
                          QSystemTrayIcon::Information );
    trayItem.setToolTip( tr( "Radio stopped." ) );
}

void Application::onPlayerError()
{
    trayItem.setIcon( QIcon( ":/images/radio-passive.png" ) );
    trayItem.showMessage( tr( "QRadioTray" ), tr( "Error occured!" ),
                          QSystemTrayIcon::Critical );
    trayItem.setToolTip( tr( "Error occured!" ) );
}

void Application::onPlayerVolumeChanged( int volume )
{
    trayItem.showMessage( tr( "QRadioTray" ), tr( "Volume %1\%." ).arg( volume ),
                          QSystemTrayIcon::Information );
}

void Application::onMetaDataChange( const QMultiMap< QString, QString > & data )
{
    QString metaInfo;
    foreach ( const QString & key, data.keys() )
    {
        if ( ( ( key == "ARTIST" ) || ( key == "ALBUM" ) || ( key == "TITLE" ) ) &&
                ( data.value( key ) != "" ) )
        {
            QTextCodec * codec = QTextCodec::codecForName( lastStation.encoding.toAscii() );
            if ( codec )
                metaInfo += QString( "%1:\r\n%2\r\n\r\n" )
                            .arg( key )
                            .arg( codec->toUnicode( data.value( key ).toAscii() ) );
        }
    }
    metaInfo = metaInfo.trimmed();
    trayItem.showMessage( tr( "QRadioTray" ), metaInfo, QSystemTrayIcon::Information );
    trayItem.setToolTip( metaInfo );
}
