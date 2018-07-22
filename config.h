#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

#include <QStandardPaths>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
//#include <QMessageBox>
#include <QDebug>

#include "config.h"
#include "tools.h"


class Config
{
public:


    /**
     * 应用名称
     * （改用语言文件 appName 表示
     */


    static QString appName;
    static int width;
    static int height;

    static QString dataPath;
    static QString windowConfigPath;

    Config();

    static QMap<QString, QString> windowConfig();
    static bool saveWindowConfig(QMap<QString, QString> data);

};

#endif // CONFIG_H
