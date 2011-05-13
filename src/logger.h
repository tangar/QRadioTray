//
// Logger.
//
#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>

class Logger : public QObject
{
    Q_OBJECT

    public:
        enum Type { Debug, Information, Warning, Error };

        explicit Logger();
        ~Logger();

        static Logger * instance();

        void add( Type type, const QString & source, const QString & message );
        void setLogFile( const QString & fileName );

    private:
        static Logger * logger;
        QString logFile;
        QFile file;
};

#define LOG_DEBUG( s, m ) { if ( Logger::instance() ) Logger::instance()->add( Logger::Debug      , ( s ), ( m ) ); }
#define LOG_INFO( s, m )  { if ( Logger::instance() ) Logger::instance()->add( Logger::Information, ( s ), ( m ) ); }
#define LOG_WARN( s, m )  { if ( Logger::instance() ) Logger::instance()->add( Logger::Warning    , ( s ), ( m ) ); }
#define LOG_ERROR( s, m ) { if ( Logger::instance() ) Logger::instance()->add( Logger::Error      , ( s ), ( m ) ); }


#endif // LOGGER_H
