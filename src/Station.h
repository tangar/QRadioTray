#ifndef STATION_H
#define STATION_H

#include <QString>

class Station
{
public:
    QString name;
    QString description;
    QString url;
    QString encoding;
    bool activity;

    Station()
    {
        name = "";
    }
};

#endif // STATION_H
