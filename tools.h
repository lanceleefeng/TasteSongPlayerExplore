#ifndef TOOLS_H
#define TOOLS_H


#include <QApplication>
#include <QTranslator>

#include <QString>
#include <QDebug>
#include <QVariantMap>



class Tools
{
public:

    static QApplication *app;
    static QTranslator *translator;

    static void switchLanguage(QString language);


    static QMap<QString, QString> reverseMap(QMap<QString, QString> map);

    static QString paddingZero(qint64 i, int length = 2);

    /**
     * 输出空行，打印数据
     */
    static void pf();
    static void pf(int num, bool printNumber = true);
    static void pf(QVariantMap data);
    static void pf(QMap<QString, QString> data);

    static void pf(QStringList data);
    static void pf(QList<QVariantMap> data);


};



#include "windows.h"

//枚举窗口参数
typedef struct
{
    HWND hwndWindow;
    DWORD dwProcessId;

} EnumWindowArg;


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
HWND GetWindowHwndByPid(DWORD pid);


BOOL killProcess(DWORD processId);


#endif // TOOLS_H
