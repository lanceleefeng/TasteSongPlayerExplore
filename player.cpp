#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QMediaMetaData>
#include <QtWidgets>
#include <QDialog>
#include <QVBoxLayout>
#include <QKeySequence>
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

//#include "lineedit.h"
#include "config.h"
#include "db.h"
#include "musicmodel.h"
#include "settingmodel.h"



Player::Player(QWidget *parent)
    : QWidget(parent)
    , listSearchInput(0)
    , listSearchDialog(0)
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

    listSearchInput = new LineEdit(this);
    QPushButton *listSearchButton = new QPushButton(tr("搜索列表"), this);

    connect(listSearchButton, &QPushButton::clicked, this, &Player::doListSearch);


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

    QBoxLayout *leftLayout = new QVBoxLayout;
    QBoxLayout *listSearchLayout = new QHBoxLayout;

    listSearchLayout->addWidget(listSearchInput);
    listSearchLayout->addWidget(listSearchButton);

    //leftLayout->addLayout(listSearchLayout);
    leftLayout->addWidget(playlistView);
    leftLayout->addLayout(listSearchLayout);


    //displayLayout->addWidget(playlistView);
    displayLayout->addLayout(leftLayout);

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

    // 快捷键操作：

    // 空格暂停、播放， Press Space to Play/Pause
    QShortcut *playOrPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    //connect(playOrPauseShortcut, &QShortcut::activated, player, &QMediaPlayer::play);
    //connect(playOrPauseShortcut, &QShortcut::activated, &Player::playOrPause);
    connect(playOrPauseShortcut, &QShortcut::activated, this, &Player::playOrPause);


    QShortcut *prevShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QShortcut *nextShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);

    QShortcut *prevShortcut2 = new QShortcut(QKeySequence(Qt::Key_PageUp), this);
    QShortcut *nextShortcut2 = new QShortcut(QKeySequence(Qt::Key_PageDown), this);


    QShortcut *volumeIncShortcut = new QShortcut(QKeySequence(Qt::Key_Up), this);
    QShortcut *volumeDecShortcut = new QShortcut(QKeySequence(Qt::Key_Down), this);

    connect(prevShortcut, &QShortcut::activated, this, &Player::previous);
    connect(nextShortcut, &QShortcut::activated, this, &Player::next);

    connect(prevShortcut2, &QShortcut::activated, this, &Player::previous);
    connect(nextShortcut2, &QShortcut::activated, this, &Player::next);

    connect(volumeIncShortcut, &QShortcut::activated, controls, &PlayerControls::volumeIncrease);
    connect(volumeDecShortcut, &QShortcut::activated, controls, &PlayerControls::volumeDecrease);



    // Qt::CTRL跟Qt::Key_Control完全不一样：
    // Qt::CTRL是Qt::Modifier之一，而Qt::Key_Control只是ctrl键
    // 参考：https://doc.qt.io/qt-5/qt.html#Modifier-enum

    QShortcut *prevModeShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this);
    //QShortcut *nextModeShortcut = new QShortcut(QKeySequence(Qt::Key_Control + Qt::Key_Right), this);
    //QShortcut *nextModeShortcut = new QShortcut(QKeySequence(Qt::Key_Control + Qt::Key_Right), controls);
    QShortcut *nextModeShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), controls);

    QShortcut *prevModeShortcut2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this);
    QShortcut *nextModeShortcut2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this);

    connect(prevModeShortcut, &QShortcut::activated, controls, &PlayerControls::prevMode);
    connect(prevModeShortcut2, &QShortcut::activated, controls, &PlayerControls::prevMode);

    connect(nextModeShortcut, &QShortcut::activated, controls, &PlayerControls::nextMode);
    connect(nextModeShortcut2, &QShortcut::activated, controls, &PlayerControls::nextMode);



    QShortcut *listSearchShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);

    // Return是主键盘的回车键，Enter是小键盘的Enter
    QShortcut *returnShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), this);

    connect(listSearchShortcut, SIGNAL(activated()), this, SLOT(doListSearch()));

    connect(returnShortcut, SIGNAL(activated()), this, SLOT(enterFilter()));
    connect(enterShortcut, SIGNAL(activated()), this, SLOT(enterFilter()));


    setWindowInfo();

    controls->initiated = true; // 完成初始化

}

Player::~Player()
{
}

void Player::setWindowInfo()
{

    QMap<QString, QString> config = Config::windowConfig();

    //qDebug() << __FUNCTION__;
    Tools::pf(config);

    this->resize(config["width"].toInt(), config["height"].toInt());
    //this->move(config["x"].toInt(), config["y"].toInt());

    int x, y;

    y = config["y"].toInt();

    if(config["x"] == "center"){

        QDesktopWidget *desktop = QApplication::desktop();
        QRect availableRect = desktop->availableGeometry();

        x = (availableRect.width() - config["width"].toInt())/2;

    }else{
        x = config["x"].toInt();
    }

    this->move(x, y);

    if(config["windowType"] == "max"){
        this->showMaximized();
    }


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

    if(player->state() != QMediaPlayer::PlayingState){
        player->play();
    }

}

void Player::playSearchItem(QListWidgetItem *item) const
{
    qDebug() << __FUNCTION__ << item->text();
    playSearchItem(item->text());
}

void Player::playSearchItem(QString text) const
{
    qDebug() << __FUNCTION__ << text;

    listSearchDialog->hide();

    if(searchResultIndex.contains(text)){
        int index = searchResultIndex[text];
        playlist->setCurrentIndex(index);
        player->play();
    }
}


void Player::jump(const QModelIndex &index)
{
    if(index.isValid()){
        playlist->setCurrentIndex(index.row());
        player->play();
    }
}


void Player::enterFilter()
{

    qDebug() << __FUNCTION__ << sender();

    // QObject::sender()
    // sender() 是触发信号的对象，是QShortcut..
    //if(sender() == listSearch){
    //    doListSearch();
    //}

    if(listSearchInput->hasFocus()){
        doListSearch();
    }


}

void Player::doListSearch()
{
    QString keywords = listSearchInput->text();
    qDebug() << __FUNCTION__ << ": " << keywords;

    if(keywords.isEmpty()){
        return;
    }

    if(keywords == previousKeywords){
        listSearchDialog->show();
        listSearchDialog->raise();
        listSearchDialog->activateWindow();
        return;
    }
    previousKeywords = keywords;

    QStringList searchResult;
    //QMap<QString, int> searchResultIndex;

    //Tools::pf(m_datas);
    //Tools::pf(m_index_path);

    QMap<QString, QVariant> item;
    QString name;

    int size = m_datas.size();
    for(int j = 0; j < size; ++j){
        item = m_datas.at(j);

        name = item["name"].toString();

        //qDebug() << item["path"];
        //qDebug() << item["name"];

        if(name.contains(keywords, Qt::CaseInsensitive)){
            qDebug() << name << ": match";
            //searchResult << name;
            QString resultItem = QString("%1 %2").arg(j + 1).arg(name);
            searchResult << resultItem;
            searchResultIndex[resultItem] = j;
        }
    }


    // 需要在Player::Player中加初始化代码，否则程序崩溃
    //, listSearchDialog(0)



    if(!listSearchDialog){
        qDebug() << "enter if";

        //listSearchDialog = new QDialog(this);
        //listSearchDialog = new QDialog();
        listSearchDialog = new ListSearchDialog(this);

        listSearchDialog->setModal(false);

        listSearchDialog->resize(300, 500);
        //listSearchDialog->move(0, 0);

        // listSearchListWidget应该写成listSearchDialog的属性！
        // 显示在Dialog中，如果Dialog要加快捷键，比如方向键、Enter播放等等，都需要与listSearchListWidget交互
        // 如果不放到Dialog，则无法交互..

        //listSearchListWidget = new QListWidget(listSearchDialog);
        listSearchDialog->listSearchListWidget = new QListWidget(listSearchDialog);

        //connect(listSearchListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *item)), this, SLOT(playSearchItem(QListWidgetItem *item)));
        connect(listSearchDialog->listSearchListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(playSearchItem(QListWidgetItem *)));

        connect(listSearchDialog, SIGNAL(playItem(QListWidgetItem *)), this, SLOT(playSearchItem(QListWidgetItem *)));
        connect(listSearchDialog, SIGNAL(playItem(QString)), this, SLOT(playSearchItem(QString)));

        QVBoxLayout *searchLayout = new QVBoxLayout;

        searchLayout->addWidget(listSearchDialog->listSearchListWidget);


        qDebug() << "before setLayout";
        listSearchDialog->setLayout(searchLayout);
        qDebug() << "after setLayout";

    }else{

        listSearchDialog->listSearchListWidget->clear();


    }
    qDebug() << "after if";


    QString searchTitlePrefix = tr("Search Result of ");
    QString title = QString("%1\"%2\"").arg(searchTitlePrefix).arg(keywords);
    listSearchDialog->setWindowTitle(title);


    //listSearchListWidget = new QListWidget;


    foreach(name, searchResult){
        new QListWidgetItem(name, listSearchDialog->listSearchListWidget);
    }

    listSearchDialog->show();
    listSearchDialog->raise();
    listSearchDialog->activateWindow();
    //listSearchDialog->exec();

    listSearchDialog->listSearchListWidget->setCurrentRow(0);

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


void Player::playOrPause()
{
    //qDebug() << player->state();

    if(player->state() == QMediaPlayer::PlayingState){
        player->pause();
    }else{
        player->play();
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


void Player::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() << event->key();
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        qDebug() << "Enter or Return pressed";
        //if(listSearch->hasFocus()) doListSearch();
    }
}


void Player::mousePressEvent(QMouseEvent *event)
{
    if(listSearchDialog && !listSearchDialog->isHidden()) listSearchDialog->hide();
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

    //qDebug() << __FUNCTION__;

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

    // 已经使用了延时保存，没必要再检查配置是否有变化了，都执行保存操作
    Config::saveWindowConfig(windowConfig);
    //Tools::pf(windowConfig);

    //QSettings settings;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.setValue("window/width", newSize.width());
    settings.setValue("window/height", newSize.height());

}
