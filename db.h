#ifndef DB_H
#define DB_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QList>
#include <QUrl>
#include <QMap>

// #include <QDebug>

// 单例模式
// http://blog.yangyubo.com/2009/06/04/best-cpp-singleton-pattern/#id7

/*
class Singleton {
public:
    static Singleton& Instance() {
        static Singleton theSingleton;
        return theSingleton;
    }

    // more (non-static) functions here
    
private:
    Singleton();                            // ctor hidden
    Singleton(Singleton const&);            // copy ctor hidden
    Singleton& operator=(Singleton const&); // assign op. hidden
    ~Singleton();                           // dtor hidden
};
*/

class DB
{
public:
    // static DB& instance();

    // 算了，先这样写吧，反正不懂c++
    //static DB& instance()
    static DB& instance(QString connectionName = "")
    {
        static DB db;


        // connectionName 没有意义？
        // 只有第一个设置有效，
        // 如果已连接，conn.isOpen()启动是有效的！
        // 如果允许多个连接，只有保存成数组了，
        // 或者其他的，新增一个连接增加一个元素

        // 如果客户端启动多个，或者同时有在不同地方的多个客户端启动
        // 这些情况暂时忽略

        qDebug() << "db.conn.isOpen(): " << db.conn.isOpen();


        //if(db.conn.isValid()) { // isValid()表示驱动是否有效
        if(!db.conn.isOpen()) { // isOpen()连接是否打开
            //db.conn = db.setConn(connectionName);
            db.setConn(connectionName);
            //return db;
        }

        qDebug() << "db.conn.isOpen(): " << db.conn.isOpen();

        db.q = new QSqlQuery(db.conn);
        
        return db;
    }

    QString standardBinding(QString binding);
    
    void test();
    QString error();

    bool exec(QString sql);
    bool exec(QString sql, QVariantMap data);
    bool exec(QString sql, QList<QVariantMap> datas);

    bool exec(QString sql, QVariantList data);
    bool exec(QString sql, QList<QVariantList> datas);


    QList<QVariantList> fetch();
    QList<QVariantMap> fetch(QStringList keys);


    // QList<QHash<QString, QVariant>> getSongs();

    // 可以定义一个用while循环得到的索引数组数据！
    // QMap<QString, QVariant> query(QString sql);
    QList<QVariantList> query(QString sql);

    QList<QVariantList> query(QString sql, bool placeholder, QVariantMap bindings);

    // 这样写用起来还是容易出错，不如增加到3个参数，
    // 如果要绑定参数，必须用3个参数的，且绑定参数是第个3
    // 这样string类型的list，都可以用QStringList了

    //QStringList 是 QListIterator<QString> 的简写，而不是 QList<QString>
    //QList<QVariantList> query(QString sql, QList<QString> bindings);
    QList<QVariantList> query(QString sql, bool placeholder, QList<QString> bindings);


    //http://doc.qt.io/qt-5/qstringlist.html
    //QList<QMap<QString, QVariant>> query(QString sql, QStringList keys);
    QList<QVariantMap> query(QString sql, QStringList keys);
    QList<QVariantMap> query(QString sql, QStringList keys, QVariantMap bindings);
    //QList<QVariantMap> query(QString sql, QStringList keys, QStringList bindings);
    QList<QVariantMap> query(QString sql, QStringList keys, QVariantList bindings);


    //static会报错？操，用的实例方式访问的
    //也不对，改成静态方式仍提示DB::conn undefined
    //static QSqlDatabase conn;
    QSqlDatabase conn;

    //static qint32 errorCode;
    //static QString errorMsg;

    qint32 errorCode = 0;
    QString errorMsg = "";


private:
    DB(){}
    DB(DB const&){}
    DB& operator=(DB const&);
    ~DB(){}

    void setConn(QString connectionName = "");

    //QSqlQuery query;
    QSqlQuery* q;


};

#endif // DB_H
