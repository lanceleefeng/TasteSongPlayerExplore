#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QMediaPlaylist>
#include <QAudio>
#include <QTimer>

#include "playercontrols.h"
#include "settingmodel.h"


PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent)
    , playerState(QMediaPlayer::StoppedState)
    , playerMuted(0)
    , playButton(0)
    , stopButton(0)
    , pauseButton(0)
    , nextButton(0)
    , previousButton(0)
    , muteButton(0)
    , volumeSlider(0)
    , rateBox(0)
    
{

    playButton = new QToolButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    
    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    
    stopButton = new QToolButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setEnabled(false);
    
    // connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));
    
    // pauseButton 创建但不加到layout上，影响了排版
    // pauseButton = new QToolButton(this);
    // pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    
    nextButton = new QToolButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    
    // connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(nextButton, SIGNAL(clicked()), this, SIGNAL(next()));
    
    previousButton = new QToolButton(this);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    
    connect(previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));

    muteButton = new QToolButton(this);
    muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    
    connect(muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));
    
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    
    connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeSliderValueChanged(int)));
    //connect(volumeSlider, SIGNAL(sliderReleased()), this, SLOT(saveVolume()));
    //connect(volumeSlider, SIGNAL(sliderPressed()), this, SLOT(saveVolume()));

    modeBox = new QComboBox(this);
    modeBox->addItem("单曲播放", QVariant(QMediaPlaylist::CurrentItemOnce));    // 0
    modeBox->addItem("单曲循环", QVariant(QMediaPlaylist::CurrentItemInLoop));  // 1
    modeBox->addItem("顺序播放", QVariant(QMediaPlaylist::Sequential));         // 2
    modeBox->addItem("列表循环", QVariant(QMediaPlaylist::Loop));               // 3
    modeBox->addItem("随机播放", QVariant(QMediaPlaylist::Random));             // 4
    
    // 默认播放模式是列表循环
    // modeBox->setCurrentIndex(0);
    // modeBox->setCurrentIndex(QMediaPlaylist::Loop);

    // 原来使用currentIndexChanged更合适
    // activated只要选择了，即使没有改变，也会触发
    // 而currentIndexChanged只有改变了才会触发。
    
    // connect(modeBox, SIGNAL(activated(int)), this, SLOT(modeChanged()));
    connect(modeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged()));

    //modeBox->setCurrentIndex(QMediaPlaylist::Loop);

    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(stopButton);
    layout->addWidget(previousButton);
    layout->addWidget(playButton);
    layout->addWidget(nextButton);
    
    layout->addWidget(muteButton);
    layout->addWidget(volumeSlider);
    layout->addWidget(modeBox);
    
    // layout->addWidget(pauseButton);
    
    setLayout(layout);
}


QMediaPlayer::State PlayerControls::state() const
{
    return playerState;
}

void PlayerControls::setState(QMediaPlayer::State state)
{
    if(state != playerState){
        //qDebug() << "New State: " << state;
        playerState = state;
        
        switch(state){
        case QMediaPlayer::StoppedState:
            stopButton->setEnabled(false);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
            
        case QMediaPlayer::PlayingState:
            stopButton->setEnabled(true);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            stopButton->setEnabled(true);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        }
    }
}


int PlayerControls::volume() const
{
    //qreal linearVolume = QAudio::convertVolume(
    //    volumeSlider->value() / qreal(100)
    //    , QAudio::LinearVolumeScale
    //    //, QAudio::LogarithmicVolumeScale
    //    , QAudio::CubicVolumeScale
    //);

    qreal linearVolume = QAudio::convertVolume(
        volumeSlider->value() / qreal(100)
        //, QAudio::LogarithmicVolumeScale
        , QAudio::CubicVolumeScale
        , QAudio::LinearVolumeScale
    );

    int vo = qRound(linearVolume * 100);
    qDebug() << __FUNCTION__ << ", slider volume: " << volumeSlider->value()
             << ", linear volume: " << vo ;
    return vo;
    //return volumeSlider->value();
}


void PlayerControls::setVolume(int volume)
{
    //qDebug() << __FUNCTION__ << volume;

    //https://doc.qt.io/qt-5/qaudio.html#VolumeScale-enum
    //https://doc.qt.io/qt-5/qaudio.html#convertVolume

    // Qt官方播放器示例项目 player 中，是从Linear转换到Logarithmic
    // 而QAudio::convertVolume中的示例，是想到的，转换成Liner然后给Player设置音量；
    // 另外，在olumeScale-enum的文档中，也说Player使用Linear音量，而UI界面上设置音量的使用Log对数音量

    // 但是，如果Player使用Linear音量，20以下，很多音乐必须调到非常低的音量，稍微调高音量就感觉非常大
    // 而如果使用Logarithmic音量，则合适的音量范围在50左右，方便使用
    // 但是使用Logarithmic音量无法用快捷键调整到最大音量

    // 最终还是决定按player官方示例中那样使用，
    // 不过不用Logarithmic给Plyaer设置音量，换成Cubic，可以调到到100了

    //qreal logarithmicVolume = QAudio::convertVolume(
    //    volume / qreal(100)
    //    //, QAudio::LogarithmicVolumeScale
    //    , QAudio::CubicVolumeScale
    //    , QAudio::LinearVolumeScale
    //);

    qreal logarithmicVolume = QAudio::convertVolume(
        volume / qreal(100)
        , QAudio::LinearVolumeScale
        //, QAudio::LogarithmicVolumeScale
        , QAudio::CubicVolumeScale
    );



    int vo = qRound(logarithmicVolume * 100);
    qDebug() << __FUNCTION__ << ", linear volume: " << volume << ", logarithimc volume " << vo;
    volumeSlider->setValue(vo);

    //volumeSlider->setValue(volume);
}

void PlayerControls::volumeIncrease()
{
    int volume = this->volume();
    int newVolume = volume + 5;
    //int newVolume = qRound(volume*1.05);
    //int newVolume = qRound(volume/0.95);
    //int newVolume = qCeil(volume/0.95);

    qDebug() << __FUNCTION__ << ", old volume: " << volume << " -> new volume:  " << newVolume;

    this->setVolume(newVolume);
}

void PlayerControls::volumeDecrease()
{
    int volume = this->volume();
    int newVolume = volume - 5;
    //int newVolume = qRound(volume*0.95);
    //int newVolume = qRound(volume/1.05);
    //int newVolume = qFloor(volume/1.05);

    qDebug() << __FUNCTION__ << ", old volume: " << volume << " -> new volume " << newVolume;
    this->setVolume(newVolume);
}


void PlayerControls::onVolumeSliderValueChanged(int sliderVolume)
{
    qDebug() << __FUNCTION__ << ": " << sliderVolume;
    emit changeVolume(volume());
    saveVolume();
}


//void PlayerControls::setMode(QMediaPlaylist::PlaybackMode mode)
void PlayerControls::setMode(int index)
{
    modeBox->setCurrentIndex(index);
}


void PlayerControls::setMuted(bool muted)
{
    if(muted != playerMuted){
        playerMuted = muted;
        
        QStyle::StandardPixmap icon = muted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume;
        muteButton->setIcon(style()->standardIcon(icon));
    }
}

void PlayerControls::setPlaybackRate(float rate)
{
    
}


void PlayerControls::playClicked()
{
    //qDebug() << "playerState: " << playerState;
    // qDebug() << playerState;
    
    switch(playerState){
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
    
}

void PlayerControls::muteClicked()
{
    emit changeMute(!playerMuted);
}

void PlayerControls::updateRate()
{
    
}

int PlayerControls::mode() const
{
    //int mode = modeBox->itemData(modeBox->currentIndex()).toInt();
    int mode = modeBox->currentIndex();
    //qDebug() << "mode: " << mode;
    return mode;
}

void PlayerControls::modeChanged(){
    emit changeMode(mode());
    saveMode();
}

void PlayerControls::nextMode()
{
    //int mode = modeBox->currentIndex();
    //int mode = mode();
    int mode = this->mode();
    mode++;
    mode = mode <= 4 ? mode : 0;
    //modeBox->setCurrentIndex(mode);
    this->setMode(mode);
}

void PlayerControls::prevMode()
{
    int mode = this->mode();
    mode--;
    mode = mode >= 0 ? mode : 4;
    this->setMode(mode);
}

void PlayerControls::saveMode(){
    //qDebug() << __FUNCTION__ << " called";
    if(!initiated){
        //qDebug() << "initiating...: " << __FUNCTION__;
        return;
    }
    if(timerSaveMode){
        return;
    }
    timerSaveMode = true;
    QTimer::singleShot(3000, this, SLOT(doSaveMode()));
}

void PlayerControls::doSaveMode()
{
    //qDebug() << __FUNCTION__ << " called";
    timerSaveMode = false;

    SettingModel settingModel;
    settingModel.saveMode(mode());

}

void PlayerControls::saveVolume()
{
    //qDebug() << __FUNCTION__ << " called";
    if(!initiated){
        //qDebug() << "initiating...: " << __FUNCTION__;
        return;
    }

    //if(timer.contains(timerTagSaveVolume)){
    if(timerSaveVolume){
        return;
    }
    timerSaveVolume = true;
    QTimer::singleShot(500, this, SLOT(doSaveVolume()));
    //QTimer::singleShot(3000, this, SLOT(doSaveVolume()));
}
void PlayerControls::doSaveVolume(){
    //qDebug() << __FUNCTION__ << " called";
    qDebug() << __FUNCTION__ << ":" << volume();

    timerSaveVolume = false;

    SettingModel setting;
    setting.saveVolume(volume());
}
