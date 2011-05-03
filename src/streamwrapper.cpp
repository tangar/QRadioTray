#include "streamwrapper.h"

StreamWrapper::StreamWrapper(QObject *parent) :
    QObject(parent)
{
    streamFile = new QFile("debug.log");
    if (streamFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        stream = new QTextStream(streamFile);
    }
}

StreamWrapper::~StreamWrapper()
{
    if (streamFile->isOpen())
        streamFile->close();
}
