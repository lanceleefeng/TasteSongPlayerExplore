
#include "musicmodel.h"

#include "db.h"
#include "basemodel.h"


MusicModel::MusicModel()
//MusicModel::MusicModel(BaseModel *parent)
{

}


QList<QVariantMap> MusicModel::getSongs()
{

    static DB& db = DB::instance();

    QString sql = QString("SELECT id,name,path,sort,volume, skip, updated_at FROM %1 ORDER BY sort ASC").arg(m_table);

    QStringList keys;
    keys << "id" << "name" << "path" << "sort" << "volume" << "skip";

    //QList<QMap<QString, QVariant>> datas = query(sql);
    QList<QMap<QString, QVariant>> datas = db.query(sql, keys);
    return datas;
}

QList<QUrl> MusicModel::getSongs(qint32 playlistId)
{
    static DB& db = DB::instance();

    QString sql = QString("SELECT id,name,path,sort,volume, updated_at FROM %1 WHERE playlist_id=%2 ORDER BY sort ASC").arg(m_table).arg(playlistId);

    QStringList keys;
    keys << "id" << "name" << "path" << "sort" << "volume";

    // QStringList paths =
    // QList<QVariant> datas = query(sql);
    QList<QMap<QString, QVariant>> datas = db.query(sql, keys);
    // QList<QHash<QString, QVariant>> datas = query(sql);

    QList<QUrl> results;
    QMap<QString, QVariant> data;
    // foreach(QMap<QString, QVariant> data, datas)
            foreach(data, datas)
        {
            // results.append(QUrl::fromLocalFile(data["path"]));
            results.append(QUrl::fromLocalFile(data["path"].toString()));

            // qDebug() << data["path"].toString();

        }
    return results;

}


bool MusicModel::addSongs(QList<QMap<QString, QVariant>> songs)
{
    if(songs.isEmpty()){
        return true;
    }

    static DB& db = DB::instance("music");

    // "a" + "b" 居然不能用，但中间加个变量就可以
    QString sql = "INSERT INTO " + m_table + " (playlist_id, name, path, sort) VALUES (:pid, :name, :path, :sort)";

    return db.exec(sql, songs);
}






