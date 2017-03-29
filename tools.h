#ifndef TOOLS_H
#define TOOLS_H

#include "windows.h"

//枚举窗口参数
typedef struct
{
    HWND hwndWindow;
    DWORD dwProcessId;

} EnumWindowArg;


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


#endif TOOLS_H
