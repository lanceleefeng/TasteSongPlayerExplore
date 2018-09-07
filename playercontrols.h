#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H


#include <QMediaPlayer>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractSlider;
class QComboBox;

QT_END_NAMESPACE

class PlayerControls : public QWidget
{
    Q_OBJECT
public:
    PlayerControls(QWidget *parent = 0);
    
    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;
    
    int mode() const;

    bool initiated = false; // 未完成初始化
    
public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMode(int index);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);
    
signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();

    // 信号函数 SIGNAL 不需要实现..
    void changeVolume(int volume);


    void changeMute(bool muting);
    void changeRate(qreal rate);
    
    void changeMode(int mode);

public slots:

    void nextMode();
    void prevMode();

    void volumeIncrease();
    void volumeDecrease();

private slots:
    void playClicked();
    void muteClicked();
    void updateRate();
    //void onVolumeSliderValueChanged();
    void onVolumeSliderValueChanged(int sliderVolume);
    void modeChanged();

    //void saveMode(QMediaPlaylist::PlaybackMode mode);
    //void saveVolume(int volume);

    void doSaveMode();
    void doSaveVolume();

private:
    void saveMode();
    void saveVolume();

    QMediaPlayer::State playerState;
    bool playerMuted;
    QAbstractButton *playButton;
    QAbstractButton *stopButton;
    QAbstractButton *pauseButton;
    QAbstractButton *nextButton;
    QAbstractButton *previousButton;
    QAbstractButton *muteButton;
    
    QAbstractSlider *volumeSlider;
    QComboBox *rateBox;
    QComboBox *modeBox;

    // 保存数据计时开关
    bool timerSaveMode = false;
    bool timerSaveVolume = false;

};


#endif // PLAYERCONTROLS_H
