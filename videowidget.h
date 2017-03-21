#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QVideoWidget>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    VideoWidget(QWidget *parent = 0);
    
};

#endif // VIDEOWIDGET_H
