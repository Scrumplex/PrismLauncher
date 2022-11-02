# SPDX-FileCopyrightText: 2005-2011 Kitware, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause

# Copyright 2005-2011 Kitware, Inc.
# From Qt5CoreMacros.cmake

function(qt_generate_moc)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_generate_moc(${ARGV})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_generate_moc(${ARGV})
    endif()
endfunction()

function(qt_wrap_cpp outfiles)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_wrap_cpp("${outfiles}" ${ARGN})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_wrap_cpp("${outfiles}" ${ARGN})
    endif()
    set("${outfiles}" "${${outfiles}}" PARENT_SCOPE)
endfunction()

function(qt_add_binary_resources)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_add_binary_resources(${ARGV})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_binary_resources(${ARGV})
    endif()
endfunction()

function(qt_add_resources outfiles)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_add_resources("${outfiles}" ${ARGN})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_resources("${outfiles}" ${ARGN})
    endif()
    set("${outfiles}" "${${outfiles}}" PARENT_SCOPE)
endfunction()

function(qt_add_big_resources outfiles)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_add_big_resources(${outfiles} ${ARGN})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_big_resources(${outfiles} ${ARGN})
    endif()
    set("${outfiles}" "${${outfiles}}" PARENT_SCOPE)
endfunction()

function(qt_import_plugins)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_import_plugins(${ARGV})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_import_plugins(${ARGV})
    endif()
endfunction()


# From Qt5WidgetsMacros.cmake

function(qt_wrap_ui outfiles)
    if(QT_VERSION_MAJOR EQUAL 5)
        qt5_wrap_ui("${outfiles}" ${ARGN})
    elseif(QT_VERSION_MAJOR EQUAL 6)
        qt6_wrap_ui("${outfiles}" ${ARGN})
    endif()
    set("${outfiles}" "${${outfiles}}" PARENT_SCOPE)
endfunction()

