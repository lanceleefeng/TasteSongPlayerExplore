#include <QApplication>
#include <QDir>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QMessageBox>


#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "db.h"
#include "dbinit.h"

#include "player.h"


// 初始化标志
//bool initiating = true;
// 这儿定义，只有在main.cpp中能用..
// 只能给需要初始化设置的类不断传递？
// 先实现再说

// 这样就没有必要用全局的初始化标志了
// 每个类中增加一个公开属性，初始化完成后改成已初始化状态

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

        qDebug() << DB::instance().error();
        QMessageBox::critical(0, QString("TasteSong Player"), QString("初始化失败"));
        return -1;
    }

    Player player;

    // 获取已有播放列表应该在读取新的之前
    


#if defined(Q_WS_SIMULATOR)
    player.setAttribute(Qt::WA_LockLandscapeOrientation);
    player.showMaximized();
#else
    player.show();
#endif

    //initiating = false; // 初始化完成

    //原来修改只有在各个控制实例化的空间进行
    //player在main中实例化，因此可以在main中设置已实例化
    //而controls在Player::Player中实例化，设置初始化完成应该在这个函数中

    //另一个操作，也可以用初始化模式完善：
    //往播放列表加歌曲时，初始化由数据库取得，不应该再往数据库保存了
    //void Player::addToPlaylist(const QStringList files, bool beginToPlay, bool addToDb)
    //而？？
    //没用，还有一个右键打开，是在初始化之前要写入数据的
    //不对，两个应该区分开
    //也不对：
    //右键或双击打开文件时，添加到播放列表应该放到初始化之后
    //这样就不会冲突了，也符合初始化的意思
    //即：初始化完成，才能进行播放、交互操作

    player.initiated = true;
    //controls.initiated = true;


    //命令行处理放到了player.show()之后，不知道会不会影响

    // 添加右键“打开方式”支持
    QCommandLineParser parser;
    parser.setApplicationDescription("TasteSong Explore02");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    if(!parser.positionalArguments().isEmpty() && player.isPlayerAvailable()) {
        /*
        QList<QUrl> urls;
        foreach(const QString &a, parser.positionalArguments()) {
            urls.append(QUrl::fromUserInput(a, QDir::currentPath(), QUrl::AssumeLocalFile));
        }
        // player.addToPlaylist(urls);
        player.addToPlaylist(urls, true, true);
        */
        QStringList files;
                foreach(const QString &a, parser.positionalArguments()){

                // a 已经是绝对路径了，不需要加路径..
                // QString file = QString("%1/%2").arg(QDir::currentPath()).arg(a);

                // QMessageBox::information(0, QString("current path"), QDir::currentPath());
                // QMessageBox::information(0, QString("param a"), a);
                // QMessageBox::information(0, QString("文件"), file);

                // files.append(file);

                files.append(a);
            }
        // debug编译后双击打开文件报错，去掉就不报错
        //player.addToPlaylist(files, true, true);
        player.addToPlaylist(files, true);

    }


    //QCoreApplication::addLibraryPath("./XLDapCodecsLite_5.2.3.4962");
    //QCoreApplication::addLibraryPath("E:\\develop\\QtProjects\\build-TasteSongExplore02-Desktop_Qt_5_8_0_MinGW_32bit-Release\\TS\\XLDapCodecsLite_5.2.3.4962");

    QString codecPath = QCoreApplication::applicationDirPath();
    //QString codecPath = QCoreApplication::applicationDirPath() + "/XLDapCodecsLite_5.2.3.4962";
    //QCoreApplication::addLibraryPath(codecPath);



    QStringList paths;
    paths << "./XLDapCodecsLite_5.2.3.4962";
    paths << "./lavfilters";
    QCoreApplication::setLibraryPaths(paths);

    qDebug() << "library path: ";
    QString path;
    foreach(path, QCoreApplication::libraryPaths()){
            qDebug() << path;
        }


    return app.exec();
}
