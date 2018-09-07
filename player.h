#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QCloseEvent>
#include <QKeyEvent>
//#include <QMouseEvent>


#include "lineedit.h"
#include "videowidget.h"
#include "listsearchdialog.h"


QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QVideoProbe;
class QVideoWidget;
class QDialog;
QT_END_NAMESPACE

class PlaylistModel;
class HistogramWidget;


class Player : public QWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

    void setWindowInfo();
    void closeEvent(QCloseEvent *event); // 关闭事件
    
    bool isPlayerAvailable() const;
    
    // void addToPlaylist(QList<QUrl> urls);
    // void addToPlaylist(const QList<QUrl> urls);
    // void addToPlaylist(const QList<QUrl> urls, bool beginToPlay);
    // void addToPlaylist(const QList<QUrl> urls, bool beginToPlay, bool addToDb);
    //void addToPlaylist(const QStringList files, bool beginToPlay, bool addToDb);
    void addToPlaylist(const QStringList files, bool beginToPlay);
    void beginSaveWindowConfig();

    bool initiated = false; // 初始化标志

protected:

    QSize size;
    QSize oldSize;

    QPoint pos;
    QPoint oldPos;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void delPid();

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void playOrPause();
    
    void next();
    void previous();

    void playSearchItem(QListWidgetItem *item) const;
    void playSearchItem(QString item) const;

    /**
     * Enter键动作过滤
     * Qt::Key_Enter: 小键盘Enter
     * Qt::Key_Return: 主键盘Enter（回车键）
     */
    void enterFilter();
    void doListSearch();

    void seek(int seconds);

    
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);
    
    void setMode(int mode);
    // void setMode(int);

    void endSaveWindowConfig();


private:
    
    void setTrackInfo(const QString &info);
    void updateDurationInfo(qint64 currentInfo);
    void playNext(QString direction);
    
    QMediaPlayer *player;
    QMediaPlaylist *playlist;

    PlaylistModel *playlistModel;
    QAbstractItemView *playlistView;

    LineEdit *listSearchInput;
    //QListWidget *listSearchListWidget; // 移到listSearchDialog中
    //QDialog *listSearchDialog;
    ListSearchDialog *listSearchDialog;

    VideoWidget *videoWidget;
    QLabel *coverLabel;
    QSlider *slider;
    QLabel *labelDuration;

    QMap<QString, QTimer *> timers;

    QPushButton *fullScreenButton;
    QPushButton *colorButton;
    
    QDialog *colorDialog;
    
    QVideoProbe *probe;

    QString trackInfo;
    QString statusInfo;
    qint64 duration;

    QList<QMap<QString, QVariant>> m_datas;
    QStringList m_index_path;
    qint32 m_max_sort;

    QString previousKeywords;
    QMap<QString, int> searchResultIndex;
    
};

#endif // PLAYER_H
