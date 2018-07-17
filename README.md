# TasteSongPlayerExplore
该项目是 TasteSong Player 探索期项目，基于Qt官方的player示例，增加了单窗口运行、播放列表（sqlite）、保存音量、播放器模式设置等，支持拖拽批量添加文件。


## 界面
![界面](./images.git/ui.png)


## 开发环境

本地环境是MinGW，IDE用的Qt creator + Clion，Qt creator 仅用于编译，Clion中写代码。

注意CMakeLists.txt，仅仅为了实现语法高亮而引入了Qt的库，没有测试是否能够编译。但QtMultiMedia引入失败，有部分会有语法错误提示。


## 跨平台
由于最后加入的单窗口运行使用了windows API，没有做跨平台处理，最新的dev分支已不能在非win平台编译。

## 格式支持！
播放高清音质的MP3需要另外安装LAV Filters或其它解码器。其它格式是否需要LAV未知。


### LAV Filters

进入页面下载：  
LAV Filters - DirectShow Media Splitter and Decoders - Doom9's Forum  
http://forum.doom9.org/showthread.php?t=156191  
Releases · Nevcairiel/LAVFilters  
https://github.com/Nevcairiel/LAVFilters/releases  

或直接点击 exe 文件链接下载：  
https://files.1f0.de/lavf/LAVFilters-0.69.exe  
https://github.com/Nevcairiel/LAVFilters/releases/download/0.69/LAVFilters-0.69-Installer.exe


## 后续项目
项目本身只是为了学习Qt，实现基本的播放器功能，后续较完整功能的播放器，在另一个项目中：

https://github.com/lanceleefeng/TasteSongPlayer



## Others
images.git文件夹中的图片不是项目图片，是git说明文档中用的.

### 单窗口运行流程图

![单窗口运行流程图](./images.git/single-window-flow-chart.png)

