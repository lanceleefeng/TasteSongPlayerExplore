cmake_minimum_required(VERSION 3.6)
project(TasteSongExplore02)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp db.cpp db.h dbinit.cpp dbinit.h musicmodel.cpp musicmodel.h
        player.cpp player.h playercontrols.cpp playercontrols.h playlistmodel.cpp playlistmodel.h
        videowidget.cpp videowidget.h
        basemodel.cpp basemodel.h settingmodel.cpp settingmodel.h)

find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

# 原来不是每个单房首字母大写，只有第一个大写
#find_package(Qt5MultiMedia REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Multimediawidgets REQUIRED)


find_package(Qt5Sql REQUIRED)
#find_package(Qt5SqlDatabase)



add_executable(TasteSongExplore02 ${SOURCE_FILES})
#target_link_libraries(TasteSongExplore02 Qt5::Core Qt5::Widgets Qt5::Gui)
#target_link_libraries(TasteSongExplore02 Qt5::Core Qt5::Widgets Qt5::MultiMedia Qt5::MultiMediaWidgets Qt5::Sql)
#target_link_libraries(TasteSongExplore02 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Multimediawidgets Qt5::Sql)
target_link_libraries(TasteSongExplore02 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Sql)
