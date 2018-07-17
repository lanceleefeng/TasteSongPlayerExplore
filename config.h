#ifndef CONFIG_H
#define CONFIG_H

#include <QString>


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


    Config();


};

#endif // CONFIG_H
