# This module finds Qt installations from system directories rather than vcpkg
#
# It uses QT_DIR environment variable to locate Qt installation
# Example: QT_DIR=C:\Qt\6.9.1\msvc2022_64\lib\cmake
#
# Usage:
#   set(QT_DIR $ENV{QT_DIR})
#   include(find_Qt_Dependency)
#   find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 强制添加你指定的Qt路径（优先级最高）
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};D:/qt/6.10.1/msvc2022_64")
set(Qt6_DIR "D:/qt/6.10.1/msvc2022_64/lib/cmake/Qt6" CACHE PATH "Qt6 config directory" FORCE)

# Check if QT_DIR environment variable is set
if(NOT DEFINED ENV{QT_DIR} AND NOT DEFINED QT_DIR)
    message(WARNING "QT_DIR environment variable is not set. Attempting to locate Qt automatically.")
    
    # Try to find Qt in common locations
    file(GLOB QT_POSSIBLE_DIRS 
        "C:/Qt/*/msvc*/lib/cmake"
        "D:/Qt/*/msvc*/lib/cmake"
        "D:/qt/*/msvc*/lib/cmake"
        "$ENV{PROGRAMFILES}/Qt/*/msvc*/lib/cmake"
    )
    
    foreach(QT_CANDIDATE_DIR ${QT_POSSIBLE_DIRS})
        if(EXISTS "${QT_CANDIDATE_DIR}/Qt6/Qt6Config.cmake")
            set(QT_DIR "${QT_CANDIDATE_DIR}")
            message(STATUS "Found Qt at: ${QT_DIR}")
            break()
        endif()
    endforeach()
    
    if(NOT DEFINED QT_DIR)
        message(WARNING "Could not automatically locate Qt. Falling back to standard Qt detection.")
    endif()
else()
    # Use QT_DIR from environment variable if not explicitly set
    if(NOT DEFINED QT_DIR)
        set(QT_DIR $ENV{QT_DIR})
    endif()

    # Convert to absolute path if it's not already
    get_filename_component(QT_DIR_ABSOLUTE "${QT_DIR}" ABSOLUTE)
    
    # Check if the directory exists
    if(EXISTS "${QT_DIR_ABSOLUTE}")
        # Add the Qt directory to CMAKE_PREFIX_PATH so find_package can locate it
        set(QT_DIR "${QT_DIR_ABSOLUTE}")
        message(STATUS "Using Qt from system directory: ${QT_DIR_ABSOLUTE}")
    else()
        message(WARNING "Specified QT_DIR does not exist: ${QT_DIR_ABSOLUTE}. Falling back to standard Qt detection.")
        unset(QT_DIR)
    endif()
endif()

# If we found or set QT_DIR, add it to CMAKE_PREFIX_PATH
if(DEFINED QT_DIR)
    list(APPEND CMAKE_PREFIX_PATH "${QT_DIR}")
    
    # Also explicitly set Qt6_DIR to ensure it's used
    set(Qt6_DIR "${QT_DIR}/Qt6" CACHE PATH "Qt6 config directory" FORCE)
endif()

# Override default Qt search behavior to prioritize system Qt over vcpkg
# Move our Qt paths to the front of CMAKE_PREFIX_PATH to give them priority
if(CMAKE_PREFIX_PATH)
    list(REVERSE CMAKE_PREFIX_PATH)
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})
    list(REVERSE CMAKE_PREFIX_PATH)
endif()