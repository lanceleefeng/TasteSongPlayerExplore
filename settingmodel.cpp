//
// Created by Administrator on 2017-3-15.
//

#include "settingmodel.h"
#include "basemodel.h"

SettingModel::SettingModel()
{

}

QVariantMap SettingModel::getSetting()
{
    QString sql = "SELECT uid, volume, mode FROM " + table + " WHERE uid=:uid LIMIT 1";
    QStringList keys;
    keys << "uid" << "volume" << "mode";

    QVariantMap bindings;
    bindings["uid"] = uid;

    QList<QVariantMap> datas = DB::instance().query(sql, keys, bindings);

    QVariantMap data;
    if(!datas.isEmpty()){
        data = datas.at(0);
    }
    return data;
}


bool SettingModel::saveMode(int mode)
{
    bool isValid;
    switch(mode){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            isValid = true;
            break;
        default:
            isValid = false;
    }

    if(!isValid){
        return false;
    }

    QVariantMap data;
    data["mode"] = mode;

    return saveSetting(data);
}

bool SettingModel::saveVolume(int volume)
{
    volume = volume > 100 ? 100 : volume;
    volume = volume < 0 ? 0 : volume;

    QVariantMap data;
    data["volume"] = volume;

    return saveSetting(data);
}


bool SettingModel::settingExists(int uid)
{
    //QString sql = "SELECT volume FROM " + table + " WHERE uid=? LIMIT 1";
    QString sql = "SELECT uid FROM " + table + " WHERE uid=:uid LIMIT 1";
    QStringList keys;
    keys << "uid";

    //QVariantList bindings;
    //bindings << uid;

    QVariantMap bindings;
    bindings["uid"] = uid;

    //static DB& db = DB::instance("setting");
    QList<QVariantMap> datas = DB::instance().query(sql, keys, bindings);

    bool exists = !datas.isEmpty();
    return exists;
}


bool SettingModel::saveSetting(QVariantMap data)
{
    static DB& db = DB::instance();
    QString sql;

    if(!settingExists(uid)){
        data["uid"] = uid;

        QStringList keyList;
        QStringList valueList;

        QMapIterator<QString, QVariant> i(data);
        while(i.hasNext()){
            i.next();
            keyList << i.key();
            valueList << db.standardBinding(i.key());
        }

        QString insertKeys = keyList.join(',');
        QString insertValues = valueList.join(',');
        sql = "INSERT INTO " + table + " (%1) VALUES (%2)";
        sql = QString(sql).arg(insertKeys).arg(insertValues);
    }else{
        //sql = "UPDATE " + table + " SET volume=:volume WHERE uid=:uid";
        sql = "UPDATE " + table + " SET %1 WHERE uid=:uid";

        QStringList setList;

        QMap<QString, QVariant>::const_iterator i = data.constBegin();
        while(i != data.constEnd()){
            setList << i.key() + "=" + db.standardBinding(i.key());
            ++i;
        }

        QString sets = setList.join(',');
        sql = QString(sql).arg(sets);
        data["uid"] = uid;
    }

    qDebug() << __FUNCTION__;
    qDebug() << "save sql: " << sql;
    return DB::instance().exec(sql, data);

}
