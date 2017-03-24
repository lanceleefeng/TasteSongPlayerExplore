#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include "basemodel.h"


class MusicModel : public BaseModel
{
    //Q_OBJECT
public:
    MusicModel();
    //MusicModel(BaseModel *parent = 0);

    // QList<QHash<QString, QVariant>> getSongs();
    //QList<QMap<QString, QVariant>> getSongs();
    QList<QVariantMap> getSongs();
    QList<QUrl> getSongs(qint32 playlistId);

    bool addSongs(QList<QMap<QString, QVariant>> songs);

private:
    QString m_table = "music";

};


#endif // MUDICMODEL_H
