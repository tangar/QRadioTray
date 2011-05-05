/*
 * MyPlayer.cpp
 *
 */

#include <QtGui>
#include <QDebug>
#include <QDateTime>
#include <QStringList>

#include "MyPlayer.h"

/*
 * --------------------------------------------------------------
 * --------------------------------------------------------------
 */
MyPlayerView::MyPlayerView()
{
    int i = 0;

    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);

    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QStringList mimeTypes  = Phonon::BackendCapabilities::availableMimeTypes();
    qDebug() << QDateTime::currentDateTime().time() << "Total MIME types:" << mimeTypes.count();
    i=0;
    foreach (QString mimeType, mimeTypes)
    {
        qDebug() << "Type #"<< i << ": " << mimeType;
        i++;
    }

    QList<Phonon::AudioOutputDevice> audioOuts = Phonon::BackendCapabilities::availableAudioOutputDevices ();
    qDebug() << QDateTime::currentDateTime().time() << "Total audio output devices:" << audioOuts.count();

    foreach (Phonon::AudioOutputDevice device, audioOuts)
    {
        qDebug() << "Device #"<< i << ": " << device.name();
        i++;
    }

    QList<Phonon::EffectDescription> effectDescriptions = Phonon::BackendCapabilities::availableAudioEffects();
    qDebug() << QDateTime::currentDateTime().time() << "Total audio effects:" << effectDescriptions.count();
    i=0;
    foreach (Phonon::EffectDescription effect, effectDescriptions)
    {
        qDebug() << "Effect #"<< i << ": " << effect.name();
        i++;
    }

    mediaObject->setTickInterval(1000);
    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),this, SLOT(sourceChanged(Phonon::MediaSource)));
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));
    connect(mediaObject, SIGNAL(bufferStatus(int)), this, SLOT(setBufferingValue(int)));
    connect(mediaObject, SIGNAL(metaDataChanged()), this, SLOT(processMetaData()));

    Phonon::createPath(mediaObject, audioOutput);

    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    playAction->setDisabled(true);

    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    pauseAction->setDisabled(true);

    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    stopAction->setDisabled(true);

    connect(playAction, SIGNAL(triggered()), mediaObject, SLOT(play()));
    connect(pauseAction, SIGNAL(triggered()), mediaObject, SLOT(pause()) );
    connect(stopAction, SIGNAL(triggered()), mediaObject, SLOT(stop()));


    setupUi();
}
/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::SetFile(const QString& file)
{
    source = Phonon::MediaSource(file);
}
/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::StopPlay()
{
    mediaObject->stop();
    mediaObject->clearQueue();
}
/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::SetUrl(const QUrl &url)
{
    source = Phonon::MediaSource(url);
}

/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::playNow()
{
    mediaObject->stop();
    mediaObject->clearQueue();
    mediaObject->setCurrentSource(source);    
    mediaObject->play();
    mediaObject->pause();
}

/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    qDebug() << QDateTime::currentDateTime().time() << QDateTime::currentDateTime().time() << ": Phonon state changed to: " << newState;
    switch (newState) {
    case Phonon::ErrorState:
        qDebug() << "Error state";
        emit playerStop();
        if(mediaObject){
            if (mediaObject->errorType() == Phonon::FatalError) {
                qDebug() << "Fatal error:" << mediaObject->errorString();
            } else {
                qDebug() << "Error:" << mediaObject->errorString();
            }
        }else{
            qDebug() << "Warning: Media is NULL";
        }
        break;
    case Phonon::PlayingState:
        emit playerPlay();
        qDebug() << "Playing state";
        mStatusLabel->setText("PlayingState");
        playAction->setEnabled(false);
        pauseAction->setEnabled(true);
        stopAction->setEnabled(true);
        break;
    case Phonon::StoppedState:
        emit playerStop();
        qDebug() << "Stopped state";
        mStatusLabel->setText("StoppedState");
        stopAction->setEnabled(false);
        playAction->setEnabled(true);
        pauseAction->setEnabled(false);
        timeLcd->display("00:00");
        break;
    case Phonon::PausedState:
        emit playerStop();
        qDebug() << "Paused state";
        mStatusLabel->setText("PausedState");
        pauseAction->setEnabled(false);
        stopAction->setEnabled(true);
        playAction->setEnabled(true);
        break;
    case Phonon::BufferingState:
        qDebug() << "Buffering state";
        mStatusLabel->setText("BufferingState");
        break;
    default:
        break;
    }
}

/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::setBufferingValue(int value)
{
    qDebug() << QDateTime::currentDateTime().time() << "Buffering: " << QString::number(value);
}

/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::sourceChanged(const Phonon::MediaSource& /*source*/)
{
    mStatusLabel->setText("sourceChanged");
    timeLcd->display("00:00");    
}
/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::tick(qint64 time)
{
    processMetaData();
    emit playerTick(time);
}

/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::aboutToFinish()
{
    emit backButtonPressed();
}

/*
  * --------------------------------------------------------------
  * --------------------------------------------------------------
  */
void MyPlayerView::setupUi()
{
    mStatusTitle = new QLabel(tr("Status:"));
    mStatusLabel = new QLabel();

    dummy = new QWidget();
    dummy->setDisabled(true);

    //videoWidget = new Phonon::VideoWidget(dummy);
    //Phonon::createPath(mediaObject, videoWidget);

    QHBoxLayout *topLayout1 = new QHBoxLayout;
    topLayout1->addWidget(mStatusTitle);
    topLayout1->addWidget(mStatusLabel);

    QToolBar *bar = new QToolBar;
    bar->addAction(playAction);
    bar->addAction(pauseAction);
    bar->addAction(stopAction);

    QPalette palette;
    palette.setBrush(QPalette::Light, Qt::darkGray);
    timeLcd = new QLCDNumber;
    timeLcd->setPalette(palette);



    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout1);
    mainLayout->addWidget(bar);
    mainLayout->addWidget(volumeSlider);
    mainLayout->addWidget(timeLcd);
    setLayout(mainLayout);
}

void MyPlayerView::processMetaData(void)
{
    QMultiMap<QString, QString> newMetaMap = mediaObject->metaData();

    if (newMetaMap != metaMap)
    {
        metaMap.clear();
        metaMap = newMetaMap;
        qDebug() << QDateTime::currentDateTime().time() << "New meta data:" << metaMap;

        emit metaDataChanged(metaMap);
    }
}

QString MyPlayerView::getSource(void)
{
    return source.url().toString();
}

void MyPlayerView::setVolume(qreal volumeLevel)
{
    qreal tmp = volumeLevel;

    tmp = (tmp < 0) ? 0 : tmp;
    tmp = (tmp > 1) ? 1 : tmp;
    audioOutput->setVolume(tmp);
    qDebug() << QDateTime::currentDateTime().time() << "Volume changed to:" << tmp;
}

void MyPlayerView::playerPlayOrPause()
{
    if( mediaObject->state() == Phonon::PlayingState )
        mediaObject->pause();
    else
    if( mediaObject->state() == Phonon::PausedState )
        mediaObject->play();
}
