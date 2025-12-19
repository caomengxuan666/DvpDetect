# FindDVP2.cmake
# 查找DVP2 SDK

# 获取当前源码目录
set(DVP2_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/DVP_SDK")

# 查找头文件目录
find_path(DVP2_INCLUDE_DIR
    NAMES DVPCamera.h
    PATHS ${DVP2_SDK_ROOT}/include
    NO_DEFAULT_PATH
)

# 查找库文件
find_library(DVP2_CAMERA_LIBRARY
    NAMES DVPCamera64
    PATHS ${DVP2_SDK_ROOT}/lib/x64
    NO_DEFAULT_PATH
)

find_library(DVP2_IR_LIBRARY
    NAMES dvpir64
    PATHS ${DVP2_SDK_ROOT}/lib/x64
    NO_DEFAULT_PATH
)

# 查找DLL文件
find_file(DVP2_CAMERA_RUNTIME
    NAMES DVPCamera64.dll
    PATHS ${DVP2_SDK_ROOT}/bin/x64
    NO_DEFAULT_PATH
)

find_file(DVP2_IR_RUNTIME
    NAMES dvpir64.dll
    PATHS ${DVP2_SDK_ROOT}/bin/x64
    NO_DEFAULT_PATH
)

# 处理标准参数
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DVP2
    REQUIRED_VARS DVP2_INCLUDE_DIR DVP2_CAMERA_LIBRARY DVP2_IR_LIBRARY
)

# 设置公共变量
if(DVP2_FOUND)
    set(DVP2_INCLUDE_DIRS ${DVP2_INCLUDE_DIR})
    set(DVP2_LIBRARIES ${DVP2_CAMERA_LIBRARY} ${DVP2_IR_LIBRARY})
    
    if(NOT TARGET DVP2::DVPCamera)
        add_library(DVP2::DVPCamera UNKNOWN IMPORTED)
        set_target_properties(DVP2::DVPCamera PROPERTIES
            IMPORTED_LOCATION "${DVP2_CAMERA_LIBRARY}"
            IMPORTED_IMPLIB "${DVP2_CAMERA_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DVP2_INCLUDE_DIR}"
        )
    endif()
    
    if(NOT TARGET DVP2::dvpir)
        add_library(DVP2::dvpir UNKNOWN IMPORTED)
        set_target_properties(DVP2::dvpir PROPERTIES
            IMPORTED_LOCATION "${DVP2_IR_LIBRARY}"
            IMPORTED_IMPLIB "${DVP2_IR_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DVP2_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES DVP2::DVPCamera
        )
    endif()
endif()

# 隐藏内部变量
mark_as_advanced(DVP2_INCLUDE_DIR DVP2_CAMERA_LIBRARY DVP2_IR_LIBRARY)