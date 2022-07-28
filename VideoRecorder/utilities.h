#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>

class Utilities
{
public:
    Utilities();
    static QString getDataPath();
    static QString newSavedVideoName();
    static QString getSavedVideoPath(QString name, QString postfix);
};

#endif // UTILITIES_H
