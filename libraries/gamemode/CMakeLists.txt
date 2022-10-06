# SPDX-FileCopyrightText: 2022 Jan200101 <sentrycraft123@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.9.4)
project(gamemode
    VERSION 1.6.1)

add_library(gamemode)
target_include_directories(gamemode PUBLIC include)
target_link_libraries(gamemode PUBLIC ${CMAKE_DL_LIBS})
