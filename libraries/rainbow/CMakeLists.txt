# SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
# SPDX-FileCopyrightText: 2007 Olaf Schmidt <ojschmidt@kde.org>
# SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
# SPDX-FileCopyrightText: 2007 Zack Rusin <zack@kde.org>
# SPDX-FileCopyrightText: 2015 Petr Mrazek <peterix@gmail.com>
# SPDX-License-Identifier: LGPL-2.0-or-later

cmake_minimum_required(VERSION 3.9.4)
project(rainbow)

if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5 COMPONENTS Core Gui REQUIRED)
elseif(Launcher_QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 COMPONENTS Core Gui REQUIRED)
endif()

set(RAINBOW_SOURCES
src/rainbow.cpp
)

add_library(Launcher_rainbow STATIC ${RAINBOW_SOURCES})
target_include_directories(Launcher_rainbow PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(Launcher_rainbow Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)
