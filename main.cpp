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
#include "tools.h";

//#include <string>
//#include "string.h"
#include "Psapi.h"
#pragma comment (lib,"Psapi.lib")



// 看说明，QLockFile的意思，本身会创建一个文件
// 用局部变量时，关掉窗口出现过获取锁定失败
// 改成用指针，只要退出程序，锁定占用就直接被撤销了


//public static QLockFile lock;
//static QLockFile lock; // 是一种实例化方式
static QLockFile *lock; // 只是定义一个指针



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // QCoreApplication::setApplicationName("TasteSong Explore 02");
    // QCoreApplication::setApplicationName(tr("TasteSong Explore 02"));
    QCoreApplication::setApplicationName(QObject::tr("TasteSong Explore 02"));
    
    QCoreApplication::setOrganizationName("OneNet Inc.");
    QCoreApplication::setApplicationVersion("0.1");

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



    QString file = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("walking.lock");
    //QFile fileObj;
    //fileObj.setFileName(file);

    QFile fileObj(file);
    if(!fileObj.exists()){
        qDebug() << "文件不存在：" << file;
        fileObj.open(QIODevice::ReadWrite);
        //fileObj.open(QIODevice::Append);
        fileObj.write("walking through the green fields"); // 会覆盖原有文件
        //fileObj.append();
        fileObj.close();
    }else{
        qDebug() << "文件存在：" << file;
    }


    //EnumWindows((WNDENUMPROC)EnumWindowsProc, 0);

    /*LPARAM lParam = 0;
    EnumWindows(EnumFunc,lParam);

    app.quit();
    return -2;
    */

    // 就没有必要在退出程序中加去掉锁定的代码了，不过还是试试能不能加


    DWORD processId = GetCurrentProcessId();
    QString pidFile = QCoreApplication::applicationDirPath() + "/pid";

    //QString pid = QString(processId);
    //QString pid = processId.toString();


    //QLockFile lock(file);
    QLockFile *lock = new QLockFile(file);


    while(true) {
        //lock.lock();
        //bool result = lock.tryLock(50);
        bool result = lock->tryLock(50);
        QFile pidObj(pidFile);

        if (result) {
            qDebug() << "获得了锁定：" << file;
            qDebug() << "新进程ID：" << processId;

            pidObj.open(QFile::WriteOnly | QFile::Truncate);
            //pidObj.write(QString("%s").arg(processId));
            //pidObj.write(pid);
            QTextStream out(&pidObj);
            out << processId;
            //out << "\n";
            //out << "test";
            pidObj.close();

            // 这儿好像新窗口还没有建立？
            // 确实无效..
            /*HWND hwndNewPlayer = GetWindowHwndByPid(processId);
            if(!hwndNewPlayer){
                qDebug() << "未能获得新窗口...";
            }else{
                SetForegroundWindow(hwndNewPlayer);
            }*/

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

                //http://www.blogbus.com/flyxxtt-logs/43973152.html
                killProcess(oldPid);

                // qt 直接开进程调用命令太麻烦，
                // 把命令写入bat文件，用WinExec调用
                // http://www.cnblogs.com/wangqiguo/p/4609228.html


                // 测试，直接干掉已打开窗口
                // 测试结束


                /*if(!killObj.exists()){
                    killObj.open(QFile::WriteOnly | QFile::Truncate);
                    QTextStream killCmd(&killObj);
                    //killCmd << "@echo off";
                    //killCmd << "\n";
                    //killCmd  << "tskill " << oldPid;
                    killCmd  << QCoreApplication::applicationDirPath() << "/tskill.exe " << oldPid;
                    killObj.close();

                    qDebug() << "kill文件：" << killFile;

                    LPCSTR winKillCmd = killFile.toStdString().c_str();
                    int resKill = WinExec(winKillCmd, SW_HIDE);
                    qDebug() << "kill result: " << resKill;

                    //Sleep(3000);

                }*/
            }

            //Sleep(1000);
            Sleep(100);
            //Sleep(300);
        }

    }



    Player player;

#if defined(Q_WS_SIMULATOR)
    player.setAttribute(Qt::WA_LockLandscapeOrientation);
    player.showMaximized();
#else
    player.show();
#endif

    // player initiation finished
    player.initiated = true;


    // 手动切换显示新窗口不是必须的
    // 不必手动显示当前窗口
    /*HWND hwndNewPlayer = GetWindowHwndByPid(processId);
    if(!hwndNewPlayer){
        qDebug() << "未能获得新窗口...";
    }else{
        qDebug() << "获得新窗口！";
        // 设置还是无效，只是在任务栏的窗口会闪动..
        // 只有试试用命令结束掉原有窗口了？？
        // 有问题了..
        // 任务栏上的窗口会一直闪...
        //SetForegroundWindow(hwndNewPlayer);
    }*/


    if(!files.isEmpty() && player.isPlayerAvailable()){
        player.addToPlaylist(files, true);
    }

    /*
    QString codecPath = QCoreApplication::applicationDirPath();
    //QString codecPath = QCoreApplication::applicationDirPath() + "/XLDapCodecsLite_5.2.3.4962";
    //QCoreApplication::addLibraryPath(codecPath);

    QStringList paths;
    paths << "./XLDapCodecsLite_5.2.3.4962";
    paths << "./lavfilters";
    QCoreApplication::setLibraryPaths(paths);

    //qDebug() << "library path: ";
    QString path;
    foreach(path, QCoreApplication::libraryPaths()){
            //qDebug() << path;
        }

     */

    return app.exec();
}

