#include <QApplication>

#include <QDir>
#include <QFile>
#include <QLockFile>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QMessageBox>


#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


#include <iostream>
#include "db.h"
#include "dbinit.h"
#include "player.h"
#include "windows.h"
#include "tools.h"

//#include <string>
//#include "string.h"
#include "Psapi.h"
#include "config.h"

#pragma comment (lib,"Psapi.lib")


// 看说明，QLockFile的意思，本身会创建一个文件
// 用局部变量时，关掉窗口出现过获取锁定失败
// 改成用指针，只要退出程序，锁定占用就直接被撤销了


//public static QLockFile lock;
//static QLockFile lock; // 是一种实例化方式
static QLockFile *lock; // 只是定义一个指针
static QString dataPath;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // QCoreApplication::setApplicationName("TasteSong Explore 02");
    // QCoreApplication::setApplicationName(tr("TasteSong Explore 02"));
    QCoreApplication::setApplicationName(QObject::tr("TasteSong Explore 02"));
    QCoreApplication::setApplicationVersion("0.1");

    //QCoreApplication::setOrganizationName("OneNet Inc.");
    QCoreApplication::setOrganizationName("OneNet");
    QCoreApplication::setOrganizationDomain("onenet.wiki");

    // 系统AppLocalData + 组织名/应用名
    //dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    Config::dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    dataPath = Config::dataPath;

    if(!dataPath.isEmpty()){
        QDir dirObj(dataPath);
        if(!dirObj.exists()){
            qDebug() << "目录不存在：" << dataPath;
            dirObj.mkpath(dataPath);
        }else{
            qDebug() << "目录存在：" << dataPath;
        }
    }

    DBInit dbInit;
    if(!dbInit.succeed()){
        //qDebug() << DB::instance().error();
        QMessageBox::critical(0, QString("TasteSong Player"), QString("初始化失败"));
        return -1;
    }


    // 添加右键“打开方式”支持，双击打开（关联格式之后）
    // right click "open with" & double click(after association)
    QCommandLineParser parser;
    parser.setApplicationDescription("TasteSong Explore02");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    QStringList files;

    //if(!parser.positionalArguments().isEmpty() && player.isPlayerAvailable()) {

    if(!parser.positionalArguments().isEmpty()) {
        /*
        QList<QUrl> urls;
        foreach(const QString &a, parser.positionalArguments()) {
            urls.append(QUrl::fromUserInput(a, QDir::currentPath(), QUrl::AssumeLocalFile));
        }
        // player.addToPlaylist(urls);
        player.addToPlaylist(urls, true, true);
        */
        foreach(const QString &a, parser.positionalArguments()){
            files.append(a);
        }
    }



    //QString file = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("walking.lock");
    QString file = QString("%1/%2").arg(dataPath).arg("walking.lock");

    DWORD processId = GetCurrentProcessId();
    //QString pidFile = QCoreApplication::applicationDirPath() + "/pid";
    QString pidFile = dataPath + "/pid";

    // QLockFile::tryLock本身会创建文件，不需要提前创建
    // 自动创建的文件会写入pid、项目名称、系统名称

    //QLockFile lock(file);
    QLockFile *lock = new QLockFile(file);

    while(true) {

        bool result = lock->tryLock(50);
        QFile pidObj(pidFile);

        if (result) {
            qDebug() << "获得了锁定：" << file;
            qDebug() << "新进程ID：" << processId;

            pidObj.open(QFile::WriteOnly | QFile::Truncate);


            QTextStream out(&pidObj);
            out << processId;

            pidObj.close();

            break;

        } else {

            qDebug() << "没有获得锁：" << file;

            // 读取已打开窗口对应进程的pid
            // 判断本次启动是否有参数
            // 切换窗口

            pidObj.open(QFile::ReadOnly);
            QTextStream in(&pidObj);
            //int oldPid;
            DWORD oldPid;
            in >> oldPid;
            pidObj.close();

            qDebug() << "已打开播放器窗口的进程ID：" << oldPid;

            if(files.isEmpty()){
                qDebug() << "没有参数，显示原有窗口";
                HWND hwndOldPlayer = GetWindowHwndByPid(oldPid);
                SetForegroundWindow(hwndOldPlayer);
                return 0;
            }else{
                qDebug() << "有参数，关闭原有窗口";
                killProcess(oldPid);
            }

            //Sleep(1000);
            Sleep(100);
            //Sleep(300);
        }

    }

    Player player;


    //player.setGeometry(100, 200, 700, 300);
    //player.showFullScreen();
    //player.showMaximized();

    //QMap<QString, QString> windowConfig = Config::windowConfig();
    //if(windowConfig["windowType"] == "max"){
    //    player.showMaximized();
    //}else if(windowConfig["windowType"] == "min"){
    //    player.showMinimized();
    //}else if(windowConfig["windowType"] == "normal"){
    //    player.setGeometry(windowConfig["x"].toInt(), windowConfig["y"].toInt(), windowConfig["width"].toInt(), windowConfig["height"].toInt());
    //}else if(windowConfig["windowType"] == "fullscreen") {
    //    player.showFullScreen();
    //}

#if defined(Q_WS_SIMULATOR)
    player.setAttribute(Qt::WA_LockLandscapeOrientation);
    //player.showMaximized();
    player.show();


#else
    player.show();
    //player.showMaximized();
#endif

    QSettings settings;
    int width = settings.value("window/width").toInt();
    int height = settings.value("window/height").toInt();
    //player.resize(width, height);

    //player.setFixedWidth()

    // player initiation finished
    player.initiated = true;

    if(!files.isEmpty() && player.isPlayerAvailable()){
        player.addToPlaylist(files, true);
    }

    return app.exec();


}

