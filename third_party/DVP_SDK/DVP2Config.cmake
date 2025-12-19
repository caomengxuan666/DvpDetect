# 获取当前文件所在的目录
get_filename_component(DVP2_CURRENT_CONFIG_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(DVP2_SDK_ROOT "${DVP2_CURRENT_CONFIG_DIR}/../../.." ABSOLUTE)

# 定义包含、库和二进制文件目录
set(DVP2_INCLUDE_DIRS "${DVP2_SDK_ROOT}/include")
set(DVP2_LIBRARY_DIRS "${DVP2_SDK_ROOT}/lib")
set(DVP2_RUNTIME_DIRS "${DVP2_SDK_ROOT}/bin")

# 创建导入库目标
if(NOT TARGET DVPCamera)
    add_library(DVPCamera SHARED IMPORTED)
    set_target_properties(DVPCamera PROPERTIES
        IMPORTED_LOCATION "${DVP2_RUNTIME_DIRS}/x64/DVPCamera64.dll"
        IMPORTED_IMPLIB "${DVP2_LIBRARY_DIRS}/x64/DVPCamera64.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${DVP2_INCLUDE_DIRS}"
        LINKER_LANGUAGE CXX
    )
endif()

if(NOT TARGET dvpir)
    add_library(dvpir SHARED IMPORTED)
    set_target_properties(dvpir PROPERTIES
        IMPORTED_LOCATION "${DVP2_RUNTIME_DIRS}/x64/dvpir64.dll"
        IMPORTED_IMPLIB "${DVP2_LIBRARY_DIRS}/x64/dvpir64.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${DVP2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES DVPCamera
        LINKER_LANGUAGE CXX
    )
endif()

# 创建命名空间别名
if(NOT TARGET DVP2::DVPCamera AND TARGET DVPCamera)
    add_library(DVP2::DVPCamera ALIAS DVPCamera)
endif()

if(NOT TARGET DVP2::dvpir AND TARGET dvpir)
    add_library(DVP2::dvpir ALIAS dvpir)
endif()

# 设置兼容性变量
set(DVP2_LIBRARIES DVP2::DVPCamera DVP2::dvpir)
set(DVP2_INCLUDE_DIRS ${DVP2_INCLUDE_DIRS})

set(DVP2_SDK_FOUND TRUE)