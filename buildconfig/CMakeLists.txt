# SPDX-FileCopyrightText: 2022 Sefa Eyeoglu <contact@scrumplex.net>
# SPDX-FileCopyrightText: 2014-2021 MultiMC Contributors
#
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0

######## Configure the file with build properties ########

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/BuildConfig.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.cpp")

add_library(BuildConfig STATIC
    BuildConfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.cpp
)

target_link_libraries(BuildConfig Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(BuildConfig PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
