//
// Logger.
//
#include "logger.h"

#include <QDebug>
#include <QDateTime>
#include <QTextStream>

Logger * Logger::logger;

Logger::Logger()
    :QObject( 0 )
{
    if ( logger )
    {
        delete logger;
        logger = 0;
    }
    logger = this;
}

Logger::~Logger()
{
    if ( file.isOpen() )
    {
        file.flush();
        file.close();
    }
}

void Logger::add( Type type, const QString & source, const QString & message )
{
    QString logMessage;
    switch ( type )
    {
        case Debug      : logMessage = "[ DEBUG ] "; break;
        case Information: logMessage = "[ INFO. ] "; break;
        case Warning    : logMessage = "[ WARN. ] "; break;
        case Error      : logMessage = "[ ERROR ] "; break;
    }

    logMessage += QString( "%1: %2 - %3" )
                  .arg( QDateTime::currentDateTime().toString( "hh:mm:ss" ) )
                  .arg( source )
                  .arg( message );
    qDebug() << logMessage;
    if ( file.isOpen() )
    {
        QTextStream out( &file );
        out << logMessage << "\r\n";
    }
}

void Logger::setLogFile( const QString & fileName )
{
    logFile = fileName;
    if ( !fileName.isEmpty() )
    {
        if ( file.isOpen() )
        {
            file.flush();
            file.close();
        }
        file.setFileName( fileName );
        file.open( QFile::WriteOnly );
    }
}

Logger * Logger::instance()
{
    return logger;
}
