#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QCoreApplication>
#include <QDir>

#include <QDebug>

#include "db.h"
#include "dbinit.h"

DBInit::DBInit()
    : m_success(false)
{
    QString connectionName = "init";
    static DB& db = DB::instance(connectionName);

    if(!db.conn.isOpen()) {
        return;
    }

    QString sqlCreateMusicTable = "create table if not exists music ("
            "id integer PRIMARY KEY autoincrement,"
            "playlist_id int default 0,"
            "name varchar(200) default '',"
            "path varchar(255) default '',"
            "md5 char(32) default '',"
            "sort mediumint default 0,"
            "volume mediumint  default 0,"
            "created_at text,"
            "updated_at text"
            ")";

    if(!db.exec(sqlCreateMusicTable)){
        return;
    }

    QString sqlCreateSettingTable = "create table if not exists settings("
            "uid integer PRIMARY KEY AUTOINCREMENT, "
            //"user_id integer default 0,"

            "mode tinyint default 3,"

            "volume smallint default 70,"
            "smart_volume tinyint default 1,"
            "last_music_id int default 0,"

            "created_at text,"
            "updated_at text"
            ")";

    if(!db.exec(sqlCreateSettingTable)){
        return;
    }

    m_success = true;
    qDebug() << "init success: " << m_success;
}

DBInit::~DBInit()
{
}


//bool succeed()
bool DBInit::succeed()
{
    return m_success;
}

