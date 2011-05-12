//
// Player.
//
#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/objectdescription.h>

class Player : public QObject
{
    Q_OBJECT

    public:
        explicit Player( QObject * parent = 0 );
        void setFile( const QString & file );
        void setUrl( const QUrl & url );
        QString getSource() const;
        void setVolume( qreal level );
        void setVolumeStep( qreal step );
        bool isPlaying();
        bool isPaused();
        bool isStopped();
        bool isError();
        bool isBuffering();
        static bool checkSource( QString source);

    public slots:
        void startPlay();
        void pausePlay();
        void stopPlay();
        void playOrPause();
        void volumeUp();
        void volumeDown();
        void stateChanged( Phonon::State newState, Phonon::State oldState );
        void sourceChanged( const Phonon::MediaSource & source );
        void aboutToFinish();
        void setBufferingValue( int value );
        void tick( qint64 time );
        void processMetaData();

    signals:
        void playerTick( quint64 time );
        void playing();
        void paused();
        void stopped();
        void errorOccured();
        void buffering( int state );
        void volumeChanged( int volume );
        void metaDataChanged( const QMultiMap< QString, QString > & data );

    private:
        Phonon::MediaObject * mediaObject;
        Phonon::AudioOutput * audioOutput;
        Phonon::MediaSource   source;
        qreal volumeStep;
};

#endif
