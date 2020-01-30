cmake_minimum_required(VERSION 3.5)

project(micsig LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

include(FindPkgConfig)

pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWS REQUIRED libswscale)

find_package(Qt5 COMPONENTS Core Quick Concurrent Multimedia REQUIRED)

if(ANDROID)
    add_library(micsig SHARED
        customvideosurface.hpp
        customvideosurface.cpp
        socket.hpp
        socket.cpp
        scope.cpp
        scope.hpp
        main.cpp
        qml.qrc
    )
else()
    add_executable(micsig
        customvideosurface.hpp
        customvideosurface.cpp
        socket.hpp
        socket.cpp
        scope.cpp
        scope.hpp
        main.cpp
        qml.qrc
    )
endif()

target_compile_definitions(micsig
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(micsig
    PRIVATE Qt5::Core Qt5::Quick Qt5::Concurrent Qt5::Multimedia ${AVCODEC_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWS_LIBRARIES})
