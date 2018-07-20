#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QMediaMetaData>
#include <QtWidgets>
#include <QShortcut>

#include <QCloseEvent>
#include <QFile>
#include <QCoreApplication>
#include <QSettings>


#include "config.h"
#include "player.h"
#include "playercontrols.h"
#include "playlistmodel.h"
//#include "histogramwidget.h"


#include "db.h"
#include "musicmodel.h"
#include "settingmodel.h"



Player::Player(QWidget *parent)
    : QWidget(parent)
    , videoWidget(0)
    , coverLabel(0)
    , slider(0)
    , colorDialog(0)
    , m_max_sort(0)
{
    
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist();
    player->setPlaylist(playlist);
    
    SettingModel settingModel;
    QVariantMap setting = settingModel.getSetting();


    /*
    // 查看设置
    qDebug() << __FILE__ << ", " << __LINE__ << ": ";
    qDebug() << __FUNCTION__;
    QMapIterator<QString, QVariant> i(setting);
    while(i.hasNext()){
        i.next();
        qDebug() << i.key() << ": " << i.value();
    }*/

    //playlist->setPlaybackMode(QMediaPlaylist::Loop);
    if(!setting.isEmpty()){
        player->setVolume(setting["volume"].toInt());
        //playlist->setPlaybackMode(setting["mode"].toInt());
        playlist->setPlaybackMode((QMediaPlaylist::PlaybackMode)setting["mode"].toInt());
    }else{
        player->setVolume(79);
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    
    connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
    
    
    videoWidget = new VideoWidget(this);
    player->setVideoOutput(videoWidget);

    // 启用更新，否则最大化时右侧视频区域出现画面残留
    videoWidget->setUpdatesEnabled(true);


    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(playlist);
    
    playlistView = new QListView(this);
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));
    
    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));
    
    slider = new QSlider(Qt::Horizontal, this);
    // slider = setRange(0, player->duration() / 1000);
    slider->setRange(0, player->duration() / 1000);
    
    labelDuration = new QLabel(this);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    
    
    probe = new QVideoProbe(this);
    // connect(probe, SIGNAL(), )
    probe->setSource(player);
    
    // QHBoxLayout *histogramLayout = new QHBoxLayout;
    
    // QPushButton *openButton = new QPushButton(tr("打开"), this);
    QPushButton *openButton = new QPushButton(tr("添加"), this);
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    //http://doc.qt.io/qt-5.8/qshortcut.html#details
    //shortcut = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), parent);
    QShortcut *openShortcut = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), openButton);
    connect(openShortcut, SIGNAL(activated()), openButton, SIGNAL(clicked()));

    //PlayerControls controls = new PlayerControls(this);
    PlayerControls *controls = new PlayerControls(this);

    controls->setState(player->state());
    //controls->setMode(playlist->playbackMode());
    controls->setMode(int(playlist->playbackMode()));
    controls->setVolume(player->volume());
    controls->setMuted(player->isMuted());


    // controls中的playButton触发play() SIGNAL，要再绑定到player的play() SLOT，才能最终播放！
    
    connect(controls, SIGNAL(play()), player, SLOT(play()));
    connect(controls, SIGNAL(pause()), player, SLOT(pause()));
    connect(controls, SIGNAL(stop()), player, SLOT(stop()));
    
    // 上一首、下一首完善：
    // 在单曲循环模式下，上一首、下一首是重新播放当前歌曲；
    // 在单曲播放模式下，上一首、下一首会停止播放；
    // 顺序播放模式下，最后一首时点下一首会停止播放，第一首时点上一首也会停止；
    // 只有循环播放和随机播放上一首、下一首一直可用
    
    // connect(controls, SIGNAL(next()), playlist, SLOT(next()));
    // connect(controls, SIGNAL(previous()), playlist, SLOT(previous()));
    
    connect(controls, SIGNAL(next()), this, SLOT(next()));
    connect(controls, SIGNAL(previous()), this, SLOT(previous()));
    
    connect(controls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));


    //保存音量也不能由volumeChanged()触发？
    //初始化的时候也会设置音量
    //初始化移到前面，然后再关联
    //http://doc.qt.io/qt-5/qmediaplayer.html#volumeChanged
    //connect(player, SIGNAL(volumeChanged(int)), SLOT(saveVolume(int)));

    //由于单曲播放等几个模式中改写了playbackMode实现播放下一首，
    //造成可能会触发多次，需要优化
    //不必了，不是由playlist的信号触发，而是在修改播放模式前保存即可
    //在setMode中

    //http://doc.qt.io/qt-5/qmediaplaylist.html#playbackModeChanged
    //connect(playlist, SIGNAL(playbackModeChanged(QMediaPlaylist::PlaylistMode)), SLOT(saveMode(QMediaPlaylist::PlaybackMode)));

    // setMode()并不是在player对象上，player对象是QMediaPlayer，
    // 而setMode定义在自定义的Player类中，要设置的是QMediaPlaylist对象
    // 应该是this的slot，怪不得用player按ctrl点击setMode不提示定义
    
    // connect(controls, SIGNAL(changeMode(int)), player, SLOT(setMode(int)));
    connect(controls, SIGNAL(changeMode(int)), this, SLOT(setMode(int)));
    
    connect(controls, SIGNAL(changeMute(bool)), player, SLOT(setMuted(bool)));
    connect(player, SIGNAL(mutedChanged(bool)), controls, SLOT(setMuted(bool)));
    
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), controls, SLOT(setState(QMediaPlayer::State)));
    
    fullScreenButton = new QPushButton(tr("全屏"), this);
    fullScreenButton->setCheckable(true);
    
    colorButton = new QPushButton(tr("配色..."), this);
    colorButton->setEnabled(false);


    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(playlistView);

    displayLayout->addWidget(videoWidget, 2); // videoWidget留着占位，以后改成歌词

    // displayLayout->addWidget(videoWidget, 1);
    // displayLayout->addWidget(playlistView);
    
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);

    controlLayout->addStretch(1);
    controlLayout->addWidget(fullScreenButton);    
    controlLayout->addWidget(colorButton);
    
    
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(slider);
    hLayout->addWidget(labelDuration);
    
    
    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    layout->addLayout(hLayout);
    layout->addLayout(controlLayout);
    // layout->addLayout(histogramLayout);

    setLayout(layout);
    metaDataChanged();
    
    //m_datas = DB::instance().getSongs();
    MusicModel musicModel;
    m_datas = musicModel.getSongs();

    QList<QUrl> songs;
    QMap<QString, QVariant> item;
    foreach(item, m_datas){
        QString path = item["path"].toString();

        if(m_max_sort < item["sort"].toInt()){
            m_max_sort = item["sort"].toInt();
        }

        //songs.append(QUrl::fromLocalFile(path));
        if(item["skip"].toInt() == 0){
            songs.append(QUrl::fromLocalFile(path));
            m_index_path.append(path);
        }

    }
    
    m_max_sort = m_max_sort > m_index_path.size() ? m_max_sort : m_index_path.size();
    
    //qDebug() << "最大排序数：" << m_max_sort;
    // addToPlaylist(songs, false, false);
    //addToPlaylist(m_index_path, false, false);
    addToPlaylist(m_index_path, false);

    setAcceptDrops(true); // 启用拖放


    controls->initiated = true; // 完成初始化

}

Player::~Player()
{
}


void Player::closeEvent(QCloseEvent *event)
{

    delPid();

}

void Player::delPid()
{
    QString pidFile;
    //QString pidFile = "pid";
    //QString pidFile = QDir::currentPath() + "/pid";

    pidFile = Config::dataPath + "/pid";

    QFile pidObj(pidFile);

    qDebug() << pidFile;
    if(pidObj.exists()){
        qDebug() << "pid文件存在";
        QFile::remove(pidFile);
    }else{
        qDebug() << "pid文件不存在";
    }
    pidObj.close();

}

bool Player::isPlayerAvailable() const 
{
    return player->isAvailable();
}


void Player::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Player::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    if(urls.isEmpty()){
        return;
    }

    QStringList files;
    QUrl url;
    QString path;
    foreach(url, urls){
            path = url.path();
            path = path.remove(0, 1);
            //qDebug() << path;
            files << path;
        }

    addToPlaylist(files, true);
}



void Player::open()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    // fileDialog.setWindowTitle(tr("打开文件"));
    fileDialog.setWindowTitle(tr("添加文件"));
    
    QStringList supportedMimeTypes = player->supportedMimeTypes();
    
    if(!supportedMimeTypes.isEmpty()){
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
    
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    
    // 视频目录: QStandardPaths::MoviesLocation 
    // 音乐目录：QStandardPaths::MusicLocation
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    
    if(fileDialog.exec() == QDialog::Accepted){
        // addToPlaylist(fileDialog.selectedUrls());
        // addToPlaylist(fileDialog.selectedUrls(), true, true);
        // addToPlaylist(fileDialog.selectedUrls(), false, true);
        //addToPlaylist(fileDialog.selectedFiles(), false, true);
        addToPlaylist(fileDialog.selectedFiles(), false);

    }
    
}

static bool isPlaylist(const QUrl &url) // .m3u playlists
{
    if(!url.isLocalFile()){
        return false;
    }
    
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}


//void Player::addToPlaylist(const QStringList files, bool beginToPlay, bool addToDb)
void Player::addToPlaylist(const QStringList files, bool beginToPlay)
{
    //qDebug() << __FUNCTION__ << " called";
    if(!initiated){
        //qDebug() << "initiating...: " << __FUNCTION__;
    }

    QList<QMap<QString, QVariant>> newSongs;
    qint16 newCount = 0;
    int newIndex = -1;

    foreach(QString file, files){
        file.replace(QString("\\"), QString("/"));
        // qDebug() << file;
        
        QFileInfo fi(file);
        qint32 index = m_index_path.indexOf(file);
        
        if((0 <= index) && beginToPlay){
            if(0 > newIndex){
                newIndex = index;
            }
        }

        //if(addToDb){
        if(initiated){
            if(0 <= index){
                // QMessageBox::information(0, QString("提示"), QString("%1已在播放列表中").arg(fi.fileName()));
                continue;
            }
            
            newCount++;
            m_max_sort++;
            
            qDebug() << m_max_sort;

            QMap<QString, QVariant> newSong;

            newSong["pid"] = 0;
            newSong["path"] = file;
            newSong["name"] = fi.baseName();
            newSong["sort"] = m_max_sort;
            
            newSongs.append(newSong);
            m_index_path.append(file);
            
            if(0 > newIndex){
                // newIndex = m_index_path.size();
                newIndex = m_index_path.size() - 1;
            }
        }

        QUrl url = QUrl::fromLocalFile(file);
        playlist->addMedia(url);
    }

    if(0 <= newIndex){
        playlist->setCurrentIndex(newIndex);
    }
    
    if(beginToPlay){
        player->play();
    }
    
    //if(addToDb && !newSongs.isEmpty()){
    if(initiated && !newSongs.isEmpty()){
        //DB::instance().addSongs(newSongs);
        MusicModel musicModel;
        musicModel.addSongs(newSongs);
    }
}

void Player::next()
{
    playNext("next");
}

void Player::previous()
{
    playNext("prev");
}


/**
 * 完善“下一首”、“上一首”
 * 
 *   // 在单曲循环模式下，上一首、下一首是重新播放当前歌曲；
 *   // 在单曲播放模式下，上一首、下一首会停止播放；
 *   // 顺序播放模式下，最后一首时点下一首会停止播放，第一首时点上一首也会停止；
 *   // 只有循环播放和随机播放上一首、下一首一直可用
 * 
 * 改进：单曲循环、单曲播放、顺序播放一直可以播放下一首
 * 
 * @brief Player::playNext
 * @param direction 播放方向，next 下一首，prev或其他值，上一首
 */

void Player::playNext(QString direction)
{

    // QMediaPlaylist的playbackMode属性不能直接读取？
    // 用playbckMode()方法获取，用setPlaybackMode(mode)设置
    
    QMediaPlaylist::PlaybackMode currentMode = playlist->playbackMode();
    
    bool switchMode = false;
    
    switch(currentMode){
    case QMediaPlaylist::CurrentItemOnce:
    case QMediaPlaylist::CurrentItemInLoop:
    case QMediaPlaylist::Sequential:
        switchMode = true;
        break;
    default:
        switchMode = false;
    }

    if(switchMode){
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
    
    if("next" == direction){
        playlist->next();
    }else{
        playlist->previous();
    }
    
    if(switchMode){
        playlist->setPlaybackMode(currentMode);
    }
    
}

void Player::jump(const QModelIndex &index)
{
    if(index.isValid()){
        playlist->setCurrentIndex(index.row());
        player->play();
    }
}

void Player::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void Player::playlistPositionChanged(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void Player::durationChanged(qint64 duration)
{
    this->duration = duration/1000;
    slider->setMaximum(this->duration);
}

void Player::positionChanged(qint64 progress)
{
    if(!slider->isSliderDown()){
        slider->setValue(progress/1000);
    }
    updateDurationInfo(progress/1000);
}

void Player::setMode(int mode)
{
    QMediaPlaylist::PlaybackMode playbackMode;
    if(mode >= 0 && mode <= 4){
        playbackMode = (QMediaPlaylist::PlaybackMode)mode;
    }else{
        playbackMode = QMediaPlaylist::Loop;
    }

    /*switch(mode){
    case 0:
        playbackMode = QMediaPlaylist::CurrentItemOnce;
        break;
    case 1:
        playbackMode = QMediaPlaylist::CurrentItemInLoop;
        break;
    case 3:
        playbackMode = QMediaPlaylist::Loop;
        break;
    case 4:
        playbackMode = QMediaPlaylist::Random;
        break;
    default:
        playbackMode = QMediaPlaylist::Sequential;
    }*/
    
    //qDebug() << "set mode: " << playbackMode;
    playlist->setPlaybackMode(playbackMode);
    
}

void Player::metaDataChanged()
{
    if(player->isMetaDataAvailable()){
        setTrackInfo(QString("%1 - %2 - TasteSong Player")
                     .arg(player->metaData(QMediaMetaData::AlbumArtist).toString())
                     .arg(player->metaData(QMediaMetaData::Title).toString()));
        
        if(coverLabel){
            QUrl url = player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();
            coverLabel->setPixmap(!url.isEmpty() ? QPixmap(url.toString()) : QPixmap());
        }
    }
}

void Player::setTrackInfo(const QString &info)
{
    trackInfo = info;
    
    setWindowTitle(trackInfo);
    
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if(currentInfo || duration){
        QString format = duration > 3600 ? "hh:mm:ss" : "mm:ss";
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime wholeTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        
        tStr = currentTime.toString(format) + " / " + wholeTime.toString(format);
        
    }
    labelDuration->setText(tStr);
}



void Player::resizeEvent(QResizeEvent *event)
{
    // resizeEvent中最大化时isMaximized()读取的是false
    //if(this->isMaximized()){
    //    qDebug() << "最大化" << __FUNCTION__;
    //}

    //QSize size = event->size();
    size = event->size();
    oldSize = event->oldSize();

    //qDebug() << "width: " << oldSize.width() << " -> " << size.width();
    //qDebug() << "heigth: " << oldSize.height() << " -> " << size.height();

    beginSaveWindowConfig();
}

void Player::moveEvent(QMoveEvent *event)
{

    pos = event->pos();
    oldPos = event->oldPos();

    //qDebug() << "x: " << oldPos.x() << " -> " << pos.x();
    //qDebug() << "y: " << oldPos.y() << " -> " << pos.y();

    beginSaveWindowConfig();
}


/**
 * 延时保存
 */
void Player::beginSaveWindowConfig()
{

    QString key = "windowConfig";

    if(!timers.contains(key)){
        timers[key] = new QTimer();
        timers[key]->setSingleShot(true);
        connect(timers[key], SIGNAL(timeout()), this, SLOT(endSaveWindowConfig()));
    }

    if(timers[key]->isActive()){
        timers[key]->stop();
    }
    timers[key]->start(1200);

}

void Player::endSaveWindowConfig()
{

    qDebug() << __FUNCTION__;

    QMap<QString, QString> windowConfig;

    QSize newSize;
    QPoint newPos;

    QString windowType = "";
    if(this->isMaximized()){
        windowType = "max";
        newSize = oldSize;
        newPos = oldPos;
    }else{
        windowType = "normal";
        newSize = size;
        newPos = pos;
    }

    windowConfig["windowType"] = windowType;

    windowConfig["width"] = QString("%1").arg(newSize.width());
    windowConfig["height"] = QString("%1").arg(newSize.height());
    windowConfig["x"] = QString("%1").arg(newPos.x());
    windowConfig["y"] = QString("%1").arg(newPos.y());

    Tools::pf(windowConfig);

    QSettings settings;
    settings.setValue("window/width", newSize.width());
    settings.setValue("window/height", newSize.height());


}


