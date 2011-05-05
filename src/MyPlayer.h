/*
 * MyPlayer.h
 *
 */

#ifndef MYPLAYER_H_
#define MYPLAYER_H_

#include <QWidget>
#include <phonon/audiooutput.h>
//#include <phonon/VideoWidget.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <Phonon/ObjectDescription>
#include <AudioOutputDevice>
#include <QList>
#include <QLabel>
#include <QPushButton>

class QAction;
class QTableWidget;
class QLCDNumber;

class MyPlayerView : public QWidget
{
    Q_OBJECT

public:
    MyPlayerView();

    QSize sizeHint() const {
        return QSize(500, 300);
    }

public slots:
    void playNow();
    void StopPlay();
    void SetFile(const QString& file);
    void SetUrl(const QUrl &url);
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void sourceChanged(const Phonon::MediaSource &source);
    void aboutToFinish();
    void setBufferingValue(int value);
    void tick(qint64 time);
    void processMetaData(void);    
    QString getSource(void);
    void setVolume(qreal volumeLevel);
    void playerPlayOrPause();

signals:
    void backButtonPressed();
    void playerTick(quint64 time);
    void playerPlay();
    void playerStop();    
    void metaDataChanged(QMultiMap<QString, QString> data);
private:
    void setupUi();
private:
    Phonon::MediaObject*   mediaObject;
    Phonon::AudioOutput*   audioOutput;
    Phonon::MediaSource    source;
    //Phonon::VideoWidget*   videoWidget;
    QLabel*                mStatusLabel;
    QLabel*                mStatusTitle;
    QAction*               playAction;
    QAction*               pauseAction;
    QAction*               stopAction;
    QLCDNumber*            timeLcd;
    Phonon::VolumeSlider*  volumeSlider;
    QWidget*               dummy;
    QMultiMap<QString, QString> metaMap;
};


#endif /* MYPLAYER_H_ */
