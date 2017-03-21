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

    Player player;

#if defined(Q_WS_SIMULATOR)
    player.setAttribute(Qt::WA_LockLandscapeOrientation);
    player.showMaximized();
#else
    player.show();
#endif


    // player initiation finished
    player.initiated = true;


    // 添加右键“打开方式”支持，双击打开（关联格式之后）
    // right click "open with" & double click(after association)
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
            files.append(a);
        }
        player.addToPlaylist(files, true);

    }


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

    return app.exec();
}
