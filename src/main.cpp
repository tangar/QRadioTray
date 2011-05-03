#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <stdlib.h>

#include "application.h"
#include "streamwrapper.h"
#include <stdio.h>

StreamWrapper *streamWrapper;

// оббертка
void myMessageOutput(QtMsgType type, const char *msg)
{
    QString src;
    switch (type) {
    case QtDebugMsg:
        src = QObject::tr("Debug:");
        break;
    case QtWarningMsg:
        src = QObject::tr("Warning:");
        break;
    case QtCriticalMsg:
        src = QObject::tr("Critical:");
        break;
    case QtFatalMsg:
        src = QObject::tr("Fatal:");
        abort();
    }
    *(streamWrapper->stream) << src + QObject::tr(msg) << endl;
    printf("%s\n", msg);
}

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    QCoreApplication::setApplicationName(QObject::tr("QRadioTray"));
    QCoreApplication::setApplicationVersion(QObject::tr("0.1"));

//    streamWrapper = new StreamWrapper();
//    if(streamWrapper->streamFile->isOpen())
//    {
//        *(streamWrapper->stream) << "test tsream\n";
//        qInstallMsgHandler(myMessageOutput);
//    }

    QString locale = QLocale::system().name();
    qDebug() << QDateTime::currentDateTime().time() << "Locale variable: " << locale;
    locale.truncate(2);
    QString codecName = QObject::tr(":/translations/qradiotray_") + locale;
    qDebug() << QDateTime::currentDateTime().time() << "Using codec: "<< codecName;
    QTranslator translator;
    translator.load(codecName);
    app.installTranslator(&translator);

    //app.setStyle( "cleanlooks" );
    app.setQuitOnLastWindowClosed(false);
    app.setWindowIcon(QIcon(":/images/radio_32_active.png"));

    Application interface;
    interface.parent_ = &app;
    interface.configure();

    // досрочный выход в случае ошибок на этапе настройки
    if ( interface.loadSettings() == false)
        return -1;

    return app.exec();
}
