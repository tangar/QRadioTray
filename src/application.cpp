#include "application.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QtUiTools/QUiLoader>
#include <QPushButton>
#include <QLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include <QUrl>
#include <QMultiMap>
#include <QTextCodec>
#include <QDateTime>
#include <QShortcutEvent>
#include <QApplication>
#include <QDesktopWidget>


#include "aboutdialog.h"
#include "settingsdialog.h"

#define CONFIG_FILE  "config.ini"
#define UI_FILE_ADDDIALOG           ":/ui/dialog.ui"
#define UI_FILE_SETTINGSDIALOG      ":/ui/settings.ui"

#define STOP_HOTKEY "Alt+X"
#define PAUSE_HOTKEY "Alt+Z"
#define VOLUME_UP_HOTKEY "Alt+W"
#define VOLUME_DOWN_HOTKEY "Alt+Q"

// volume changing step in %
#define VOLUME_STEP 5
#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define DEF_VOLUME 50


Application::Application(QObject *parent) :
        QObject(parent)
{
    if ( QFile::exists( CONFIG_FILE ) )
    {
        settings = new QSettings( CONFIG_FILE, QSettings::IniFormat );
    }

    // playerStop shortcut
    globalShortcut = new QxtGlobalShortcut(this);
    connect(globalShortcut, SIGNAL(activated()), this, SLOT(stopPlayer()));
    globalShortcut->setShortcut(QKeySequence(STOP_HOTKEY));

    // playerPause shortcut
    globalShortcut = new QxtGlobalShortcut(this);
    connect(globalShortcut, SIGNAL(activated()), this, SLOT(playOrPausePlayer()));
    globalShortcut->setShortcut(QKeySequence(PAUSE_HOTKEY));

    // volumeUp shortcut
    globalShortcut = new QxtGlobalShortcut(this);
    connect(globalShortcut, SIGNAL(activated()), this, SLOT(increaseVolume()));
    globalShortcut->setShortcut(QKeySequence(VOLUME_UP_HOTKEY));

    // volumeDown shortcut
    globalShortcut = new QxtGlobalShortcut(this);
    connect(globalShortcut, SIGNAL(activated()), this, SLOT(decreaseVolume()));
    globalShortcut->setShortcut(QKeySequence(VOLUME_DOWN_HOTKEY));
}

Application::~Application()
{   
}

void Application::configure()
{
    currTrayIcon = 0;
    trayIconList.append(":/images/radio_32_active_2.png");
    trayIconList.append(":/images/radio_32_active_1.png");
    trayIconList.append(":/images/radio_32_active.png");
    trayIconCount = trayIconList.count();

    //! set volume level 50%
    volumeLevel = DEF_VOLUME;
    player.setVolume((qreal)volumeLevel/100);

    bool sysTrayAvailable = QSystemTrayIcon::isSystemTrayAvailable();
    qDebug() << QDateTime::currentDateTime().time() << "System tary available: " << sysTrayAvailable;
    bool supportMessages = QSystemTrayIcon::supportsMessages();
    qDebug() << QDateTime::currentDateTime().time() << "System tary messages support: " << supportMessages;

    if (!( sysTrayAvailable && supportMessages ))
        QMessageBox::critical(  &trayMenu,
                                tr("Critical error"),
                                tr("System tray and System tray notifications are not enabled in your system. Will now quit"));

    createBaseMenu();

    trayItem.setContextMenu(&trayMenu);
    trayItem.setIcon(QIcon(":/images/radio_32_passive.png"));
    trayItem.show();

    trayItem.showMessage(tr("QRadioTray"), tr("Started successfully"), QSystemTrayIcon::Information, 1);

    connect(trayItem.contextMenu(), SIGNAL(triggered(QAction*)), this, SLOT(processMenu(QAction*)));

    ((QApplication *)qApp)->desktop()->installEventFilter(this);
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    QString oname = object->objectName();
    int type = (int) event->type();

    switch(type)
    {
    case QEvent::MouseMove:
        qDebug() << "QEvent::MouseMove"; break;
    case QEvent::StatusTip:
        qDebug() << "QEvent::StatusTip"; break;
    case QEvent::UpdateRequest:
        qDebug() << "QEvent::UpdateRequest"; break;
    case QEvent::Paint:
        qDebug() << "QEvent::Paint"; break;
    case QEvent::Shortcut:
        qDebug() << "QEvent::Shortcut"; break;
    case QEvent::ShortcutOverride:
        qDebug() << "QEvent::ShortcutOverride"; break;
    case QEvent::KeyPress:
        qDebug() << "QEvent::KeyPress"; break;
    default:
        qDebug() << "Unknown event: " << type; break;
    }

    if (( type == QEvent::Shortcut ) || (type == QEvent::ShortcutOverride))
    {

    }

    if (oname != "")
    {
        if(event->type() == QEvent::Shortcut)
        {
            QShortcutEvent *shortcutEvent = static_cast<QShortcutEvent*>(event);
            qDebug() << "Ate key press" << shortcutEvent->key();
        }
    }

    return true;
}

void Application::createBaseMenu()
{
    QAction * act;
    act = new QAction(this);

//    act->installEventFilter(act);
    trayMenu.clear();

    if (!QSystemTrayIcon::isSystemTrayAvailable()) return;

    stationsMenu = new QMenu(tr("Stations"));
    stationsMenu->setIcon(QIcon(":/images/radio_32_passive.png"));
    trayMenu.addMenu(stationsMenu);

    trayMenu.addSeparator();

    act->setIcon(QIcon(":/images/volume_up_32.png"));

    act->setShortcut(QKeySequence(VOLUME_UP_HOTKEY));
    act->setText(tr("Volume +10%"));
    act->setProperty("type", QVariant("volume_up"));
    trayMenu.addAction(act);

    act = new QAction(this);
    act->setIcon(QIcon(":/images/volume_down_32.png"));
    act->setShortcut(QKeySequence(VOLUME_DOWN_HOTKEY));
    act->setText(tr("Volume -10%"));
    act->setProperty("type", QVariant("volume_down"));
    trayMenu.addAction(act);

    trayMenu.addSeparator();

    act = new QAction(this);
    act->setShortcut(QKeySequence(PAUSE_HOTKEY));
    act->setIcon(QIcon(":/images/pause_32.png"));
    act->setText(tr("Pause"));
    act->setProperty("type", QVariant("pause"));
    trayMenu.addAction(act);

    act = new QAction(this);
    act->setShortcut(QKeySequence(STOP_HOTKEY));
    act->setIcon(QIcon(":/images/stop_32.png"));
    act->setText(tr("Stop"));
    act->setProperty("type", QVariant("stop"));
    trayMenu.addAction(act);

    trayMenu.addSeparator();

    act = new QAction(this);
    act->setIcon(QIcon(":/images/info_32.png"));
    act->setText(tr("Info"));
    act->setProperty("type", QVariant("about"));
    act->setMenuRole(QAction::PreferencesRole);
    trayMenu.addAction(act);

    trayMenu.addSeparator();

    act = new QAction(this);
    act->setIcon(QIcon(":/images/settings_32.png"));
    act->setText(tr("Settings"));
    act->setProperty("type", QVariant("settings"));
    act->setMenuRole(QAction::PreferencesRole);
    trayMenu.addAction(act);

    act = new QAction(this);
    act->setIcon(QIcon(":/images/close_32.png"));
    act->setText(tr("Exit"));
    act->setProperty("type", QVariant("exit"));
    act->setMenuRole(QAction::QuitRole);
    trayMenu.addAction(act);

}

void Application::processMenu(QAction *action)
{
    qDebug()<< QDateTime::currentDateTime().time()
            << "Selection:" << endl
            << "\tname:" << action->text() << endl
            << "\tdescription:" << action->property("description").toString() << endl
            << "\turl:" << action->property("url").toString() << endl
            << "\tencoding:" << action->property("encoding").toString() << endl
            << "\ttype:" << action->property("type").toString() << endl
            << "\tstatus:" << action->property("status").toBool() << endl;

    if (action->property("type").toString() == "exit")
    {
        player.StopPlay();
        qApp->exit();
    }
    else
        if (action->property("type").toString() == "settings")
        {
        this->manageSettings();
    }
    else
        if (action->property("type").toString() == "about")
        {
        AboutDialog dialog;

        dialog.exec();
    }
    else
        if (action->property("type").toString() == "pause")
        {
        playOrPausePlayer();
    }
    else
        if (action->property("type").toString() == "stop")
        {
        stopPlayer();
        foreach(QAction *action,stationsMenu->actions())
            if ( action->isCheckable() ) action->setChecked(false);
    }
    else
        if (action->property("type").toString() == "volume_up")
        {
        increaseVolume();
    }
    else
        if (action->property("type").toString() == "volume_down")
        {
        decreaseVolume();
    }
    else
        if (action->property("type").toString() == "media")
        {

        QTextCodec::setCodecForCStrings(QTextCodec::codecForName(action->property("encoding").toString().toAscii()));
        QTextCodec::setCodecForTr(QTextCodec::codecForName(action->property("encoding").toString().toAscii()));

        qDebug() << "codec for cstrings:" << QTextCodec::codecForCStrings()->name();
        qDebug() << "codec for tr:" << QTextCodec::codecForTr()->name();

        QUrl url(action->property("url").toString());        

        foreach(QAction *action,stationsMenu->actions())
            if ( (action->isCheckable()) && (action != trayItem.contextMenu()->activeAction()))
                action->setChecked(false);
        if (player.getSource() == url.toString())
        {
            action->setChecked(false);
            player.StopPlay();
        }
        else
        {
            action->setChecked(true);

            player.SetUrl(url);
            player.disconnect();
            player.playNow();
        }

        qDebug() << "connection player tick to app:"            << connect(&player, SIGNAL(playerTick(quint64)), this, SLOT(animateIcon(quint64)));
        qDebug() << "connection player play to app:"            << connect(&player, SIGNAL(playerPlay()), this, SLOT(onPlayerPlay()));
        qDebug() << "connection player stop to app:"            << connect(&player, SIGNAL(playerStop()), this, SLOT(onPlayerStop()));        
        qDebug() << "connection player metadatachanged to app:" << connect(&player, SIGNAL(metaDataChanged(QMultiMap<QString, QString>)), this, SLOT(onMetaDataChange(QMultiMap<QString, QString>)));

    }
}

bool Application::loadSettings()
{
    QString gStr;
    QStringList gList;

    if (settings)
    {
        // Загрузка настроек.
        settings->beginGroup( "STATIONS" );
        gList.clear();
        gList = settings->childGroups();

        stationsMenu->clear();
        foreach ( gStr, gList )
        {
            settings->beginGroup(gStr);
            stationsMenu->addAction(appendStation(settings->value("name").toString(),
                                                  settings->value("description").toString(),
                                                  settings->value("url").toString(),
                                                  settings->value("encoding").toString()));
            settings->endGroup();
        }
        settings->endGroup();

    }
    else
        QMessageBox::warning(&trayMenu,
                              tr("QRadiTray"),
                              tr("Unable open config file. No settings recivied."));
    return true;
}

QAction * Application::appendStation(QString name, QString description, QString url, QString enc)
{
    QAction * act = new QAction(this);
    act->setText(name);
    act->setToolTip(description);    
    act->setProperty("url", QVariant(url));
    act->setProperty("encoding", QVariant(enc));
    act->setProperty("type", QVariant("media"));
    act->setProperty("active", QVariant(false));
    act->setToolTip(tr("%1").arg(description));
    act->setCheckable(true);
    return act;       
}

void Application::manageSettings()
{    
    int counter = 0;

    if (settingsDialog.isVisible())
        return;

    settingsDialog.loadSettings(settings);
    if ( settingsDialog.exec() )
        if (settings)
        {
        settings->beginGroup( "STATIONS" );
            settings->clear();
            foreach (Station *station, settingsDialog.stationList)
            {
                settings->beginGroup(QString::number(counter));
                settings->setValue("name",station->name);
                settings->setValue("description", station->description);
                settings->setValue("url", station->url);
                settings->setValue("encoding", station->encoding);
                settings->endGroup();

                counter++;
            }
        settings->endGroup();
        this->loadSettings();
    }
}

void Application::animateIcon(quint64 tick)
{
    trayItem.setIcon(QIcon(trayIconList.at(currTrayIcon)));
    currTrayIcon++;
    if (currTrayIcon >= trayIconCount) currTrayIcon = 0;
}

void Application::onPlayerStop()
{
    qDebug() << QDateTime::currentDateTime().time() << "application stop";
    trayItem.setIcon(QIcon(":/images/radio_32_passive.png"));

    foreach(QAction *action,stationsMenu->actions())
        if (action->isCheckable()) action->setChecked(false);
}

void Application::onPlayerPlay()
{
    qDebug() << QDateTime::currentDateTime().time() << "application play";
    trayItem.setIcon(QIcon(":/images/radio_32_active.png"));
}

void Application::onMetaDataChange(QMultiMap<QString, QString> data)
{
    QString metaInfo;
    foreach (QString key, data.keys())
        if (data.value(key) != "")
            metaInfo += tr("\n%1 \t: %2").arg(key).arg(data.value(key));

    qDebug() << QDateTime::currentDateTime().time() << "new meta data:" << metaInfo;
    trayItem.showMessage(tr("QRadioTray"), metaInfo, QSystemTrayIcon::NoIcon, 5000);
    trayItem.setToolTip(metaInfo);
}

void Application::playOrPausePlayer()
{
    qDebug() << "Application player play/pause";
    player.playerPlayOrPause();
}

void Application::increaseVolume()
{
    volumeLevel = ( volumeLevel + VOLUME_STEP > MAX_VOLUME ) ? MAX_VOLUME : volumeLevel + VOLUME_STEP;
    player.setVolume(((qreal)volumeLevel)/100);
}

void Application::decreaseVolume()
{
    volumeLevel = ( volumeLevel - VOLUME_STEP >= MIN_VOLUME ) ? volumeLevel - VOLUME_STEP : MIN_VOLUME;
    player.setVolume(((qreal)volumeLevel)/100);
}

void Application::stopPlayer()
{
    player.StopPlay();
}
