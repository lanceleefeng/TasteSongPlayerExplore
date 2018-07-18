#include "tools.h"


QApplication * Tools::app;
QTranslator * Tools::translator;

//void Tools::switchLanguage(QString language, QApplication *app, QTranslator *translator)
void Tools::switchLanguage(QString language)
{
    QString path = ":/languages";
    QString languageFile = path + "/" + language + ".qm";

    bool res = translator->load(languageFile);

    if(!res){
        qDebug() << "引入翻译文件失败:" << languageFile;
        return;
    }
    app->installTranslator(translator);

}


QMap<QString, QString> Tools::reverseMap(QMap<QString, QString> map)
{
    if(map.isEmpty()){
        return map;
    }
    QMap<QString, QString> newMap;
    QMap<QString, QString>::const_iterator i = map.constBegin();
    while(i != map.constEnd()){
        newMap[i.value()] = i.key();
        ++i;
    }
    return newMap;
};

/**
 * 补0
 * @param i
 * @param length
 * @return
 */
QString Tools::paddingZero(qint64 i, int length)
{
    QString s = QString("%1").arg(i, length, 10, QLatin1Char('0'));
    return s;
}

void Tools::pf(QVariantMap data)
{
    if(!data.isEmpty()){
        QMap<QString, QVariant>::const_iterator i = data.constBegin();
        while(i != data.constEnd()){
            qDebug() << i.key() << ":\t" << i.value();
            ++i;
        }
    }else{
        qDebug() << "empty map";
    }
    qDebug() << "\n";
}

void Tools::pf(QMap<QString, QString> data)
{
    if(!data.isEmpty()){
        QMap<QString, QString>::const_iterator i = data.constBegin();
        while(i != data.constEnd()){
            qDebug() << i.key() << ":\t" << i.value();
            ++i;
        }
    }else{
        qDebug() << "empty map";
    }
    qDebug() << "\n";
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    EnumWindowArg * pArg = (EnumWindowArg *)lParam;
    DWORD dwProcessId = 0;
    DWORD dwThreadId;
    dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);

    if(dwProcessId == pArg->dwProcessId){
        pArg->hwndWindow = hwnd;
        return FALSE;
    }

    //return true;
    return TRUE;

}


HWND GetWindowHwndByPid(DWORD pid)
{
    HWND hwndRet = NULL;
    EnumWindowArg ewArg;
    ewArg.dwProcessId = pid;
    //ewArg.hwndWindow = null; // 小写的null不能用？
    ewArg.hwndWindow = NULL;

    EnumWindows(EnumWindowsProc, (LPARAM)&ewArg);

    if(ewArg.hwndWindow){
        hwndRet = ewArg.hwndWindow;
    }
    return hwndRet;
}


//http://www.blogbus.com/flyxxtt-logs/43973152.html
BOOL killProcess(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if(hProcess == NULL){
        return false;
    }
    if(TerminateProcess(hProcess, 0)){
        return true;
    }else{
        return false;
    }
}


