#include "utilities.h"

Utilities::Utilities()
{

}

QString Utilities::getDataPath()
{
    QString user_movie_path = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).constFirst();
    QDir movie_dir(user_movie_path);
    movie_dir.mkpath("Video_Recordings");
    return movie_dir.absoluteFilePath("Video_Recordings");
}

QString Utilities::newSavedVideoName()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd+HH:mm:ss");
}

QString Utilities::getSavedVideoPath(QString name, QString postfix)
{
    return QString("%1/%2.%3").arg(Utilities::getDataPath(), name, postfix);
}
