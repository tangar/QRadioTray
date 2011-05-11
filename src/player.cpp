//
// Player.
//
#include "player.h"
#include "logger.h"

#include <QUrl>

Player::Player( QObject * parent )
    :QObject( parent ),
     volumeStep( 0.1 )
{
    audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
    mediaObject = new Phonon::MediaObject( this );

    if ( !audioOutput || !mediaObject )
        return;

    const QStringList & mimeTypes = Phonon::BackendCapabilities::availableMimeTypes();
    LOG_INFO( "player", tr( "Total MIME types = %1." ).arg( mimeTypes.count() ) );
    int i = 0;
    foreach ( const QString & mimeType, mimeTypes )
    {
        LOG_INFO( "player", tr( "Type #%1: %2." ).arg( i ).arg( mimeType ) );
        i++;
    }

    const QList< Phonon::AudioOutputDevice > & audioOutputs = Phonon::BackendCapabilities::availableAudioOutputDevices();
    LOG_INFO( "player", tr( "Total audio output devices = %1." ).arg( audioOutputs.count() ) );
    i = 0;
    foreach ( const Phonon::AudioOutputDevice & device, audioOutputs )
    {
        LOG_INFO( "player", tr( "Device #%1: %2." ).arg( i ).arg( device.name() ) );
        i++;
    }

    const QList< Phonon::EffectDescription > & effectDescriptions = Phonon::BackendCapabilities::availableAudioEffects();
    LOG_INFO( "player", tr( "Total audio effects = %1." ).arg( effectDescriptions.count() ) );
    i = 0;
    foreach ( const Phonon::EffectDescription & effect, effectDescriptions )
    {
        LOG_INFO( "player", tr( "Effect #%1: %2." ).arg( i ).arg( effect.name() ) );
        i++;
    }

    mediaObject->setTickInterval( 1000 );

    connect( mediaObject, SIGNAL( tick( qint64 ) ), SLOT( tick( qint64 ) ) );
    connect( mediaObject, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ),
                          SLOT( stateChanged( Phonon::State, Phonon::State ) ) );
    connect( mediaObject, SIGNAL( currentSourceChanged( Phonon::MediaSource ) ),
                          SLOT( sourceChanged( Phonon::MediaSource ) ) );
    connect( mediaObject, SIGNAL( aboutToFinish() ), SLOT( aboutToFinish() ) );
    connect( mediaObject, SIGNAL( bufferStatus( int ) ),
                          SLOT( setBufferingValue( int ) ) );
    connect( mediaObject, SIGNAL( metaDataChanged() ),
                          SLOT( processMetaData() ) );

    audioOutput->setVolume( 0.5 );

    Phonon::createPath( mediaObject, audioOutput );
}

void Player::setFile( const QString & file )
{
    source = Phonon::MediaSource( file );
}

void Player::setUrl( const QUrl & url )
{
    source = Phonon::MediaSource( url );
}

void Player::startPlay()
{
    if ( !mediaObject )
        return;

    mediaObject->setCurrentSource( source );
    mediaObject->play();
    emit playing();
    LOG_INFO( "player", tr( "Start play." ) );
}

void Player::pausePlay()
{
    if ( !mediaObject )
        return;

    mediaObject->pause();
    emit paused();
    LOG_INFO( "player", tr( "Pause play." ) );
}

void Player::playOrPause()
{
    if ( !mediaObject )
        return;

    if ( mediaObject->state() == Phonon::PlayingState )
        pausePlay();
    else
        startPlay();
}

void Player::stopPlay()
{
    if ( !mediaObject )
        return;

    mediaObject->stop();
    mediaObject->clearQueue();
    emit stopped();
    LOG_INFO( "player", tr( "Stop play." ) );
}

void Player::volumeUp()
{
    if ( !audioOutput )
        return;

    setVolume( audioOutput->volume() + volumeStep );
}

void Player::volumeDown()
{
    if ( !audioOutput )
        return;

    setVolume( audioOutput->volume() - volumeStep );
}

void Player::setVolume( qreal level )
{
    if ( !audioOutput )
        return;

    if ( level < 0.0 )
        level = 0.0;
    else if ( level > 1.0 )
        level = 1.0;
    audioOutput->setVolume( level );
    level = qRound( 100.0 * level );
    emit volumeChanged( level );

    LOG_INFO( "player", tr( "Volume changed to %1." ).arg( level ) );
}

void Player::setVolumeStep( qreal step )
{
    if ( step <= 0 )
        volumeStep = 0.01;
    else if ( step > 1 )
        volumeStep = 1;
    else
        volumeStep = step;
}

void Player::stateChanged( Phonon::State newState, Phonon::State oldState )
{
    Q_UNUSED( oldState );

    LOG_INFO( "player", tr( "Phonon state changed to %1." ).arg( newState ) );
    if ( newState == Phonon::ErrorState )
    {
        LOG_DEBUG( "player", tr( "Error state." ) );
        if ( mediaObject )
        {
            if ( mediaObject->errorType() == Phonon::FatalError )
                LOG_ERROR( "player", tr( "Fatal error \"%1\"!" ).arg( mediaObject->errorString() ) )
            else
                LOG_ERROR( "player", tr( "Error \"%1\"!" ).arg( mediaObject->errorString() ) );
        }
        else
            LOG_WARN( "player", tr( "No media object!" ) );

        emit errorOccured();
    }
    else if ( newState == Phonon::PlayingState )
    {
        LOG_DEBUG( "player", tr( "Playing state." ) );
    }
    else if ( newState == Phonon::StoppedState )
    {
        LOG_DEBUG( "player", tr( "Stopped state." ) );
    }
    else if ( newState == Phonon::PausedState )
    {
        LOG_DEBUG( "player", tr( "Paused state." ) );
    }
    else if ( newState == Phonon::BufferingState )
    {
        LOG_DEBUG( "player", tr( "Buffering state." ) );
    }
}

void Player::setBufferingValue( int value )
{
    LOG_INFO( "player", tr( "Buffering %1." ).arg( value ) );
}

void Player::sourceChanged( const Phonon::MediaSource & source )
{  
    LOG_INFO( "player", tr( "Source changed %1." ).arg( source.fileName() ) );
}

void Player::tick( qint64 time )
{
    emit playerTick( time );
}

void Player::aboutToFinish()
{
}

void Player::processMetaData()
{
    if ( !mediaObject )
        return;

    LOG_INFO( "player", tr( "New meta data." ) );
    emit metaDataChanged( mediaObject->metaData() );
}

QString Player::getSource() const
{
    return source.url().toString();
}

bool Player::isPlaying()
{
    if ( !mediaObject )
        return false;

    return ( mediaObject->state() == Phonon::PlayingState );
}

bool Player::isPaused()
{
    if ( !mediaObject )
        return false;

    return ( mediaObject->state() == Phonon::PausedState );
}

bool Player::isStopped()
{
    if ( !mediaObject )
        return false;

    return ( mediaObject->state() == Phonon::StoppedState );
}

bool Player::isError()
{
    if ( !mediaObject )
        return false;

    return ( mediaObject->state() == Phonon::ErrorState );
}
