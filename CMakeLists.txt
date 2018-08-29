cmake_minimum_required(VERSION 3.6)
project(TasteSongExplore03)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp db.cpp db.h dbinit.cpp dbinit.h musicmodel.cpp musicmodel.h lineedit.cpp lineedit.h
        player.cpp player.h playercontrols.cpp playercontrols.h playlistmodel.cpp playlistmodel.h
        videowidget.cpp videowidget.h
        basemodel.cpp basemodel.h settingmodel.cpp settingmodel.h tools.h tools.cpp config.cpp config.h)

set (CMAKE_PREFIX_PATH "E:/develop/Qt/5.11.1/mingw53_32")
#set(Qt5Core_DIR "E:/develop/Qt/5.11.1/mingw53_32/lib/cmake/Qt5Core")
#set(Qt5_DIR "E:/develop/Qt/5.11.1/mingw53_32/lib/cmake/Qt5")
#set(QT_QMAKE_EXECUTABLE "E:/develop/Qt/5.11.1/mingw53_32/bin/qmake.exe")

find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

# 原来不是每个单房首字母大写，只有第一个大写
#find_package(Qt5MultiMedia REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Multimediawidgets REQUIRED)


find_package(Qt5Sql REQUIRED)
#find_package(Qt5SqlDatabase)


add_executable(TasteSongExplore03 ${SOURCE_FILES})
target_link_libraries(TasteSongExplore03 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Sql)
