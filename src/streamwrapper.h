#ifndef STREAMWRAPPER_H
#define STREAMWRAPPER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class StreamWrapper : public QObject
{
    Q_OBJECT
public:
    explicit StreamWrapper(QObject *parent = 0);
    ~StreamWrapper();

    QFile *streamFile;
    QTextStream *stream;
signals:

public slots:

};

#endif // STREAMWRAPPER_H
