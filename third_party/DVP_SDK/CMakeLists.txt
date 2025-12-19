cmake_minimum_required(VERSION 3.15)
project(DVP2_SDK NONE)

# 安装DVP2Config.cmake文件到lib/cmake/DVP2目录
install(FILES DVP2Config.cmake
        DESTINATION lib/cmake/DVP2)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 包含目录
set(DVP2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DVP2_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(DVP2_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 创建导入库目标
add_library(DVPCamera SHARED IMPORTED GLOBAL)
set_property(TARGET DVPCamera PROPERTY IMPORTED_LOCATION ${DVP2_BIN_DIR}/x64/DVPCamera64.dll)
set_property(TARGET DVPCamera PROPERTY IMPORTED_IMPLIB ${DVP2_LIB_DIR}/x64/DVPCamera64.lib)
set_property(TARGET DVPCamera PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${DVP2_INCLUDE_DIR})
set_property(TARGET DVPCamera PROPERTY LINKER_LANGUAGE CXX)

add_library(dvpir SHARED IMPORTED GLOBAL)
set_property(TARGET dvpir PROPERTY IMPORTED_LOCATION ${DVP2_BIN_DIR}/x64/dvpir64.dll)
set_property(TARGET dvpir PROPERTY IMPORTED_IMPLIB ${DVP2_LIB_DIR}/x64/dvpir64.lib)
set_property(TARGET dvpir PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${DVP2_INCLUDE_DIR})
set_property(TARGET dvpir PROPERTY INTERFACE_LINK_LIBRARIES DVPCamera)
set_property(TARGET dvpir PROPERTY LINKER_LANGUAGE CXX)

# 添加别名，提高兼容性
add_library(DVP2::DVPCamera ALIAS DVPCamera)
add_library(DVP2::dvpir ALIAS dvpir)

# 生成适用于 find_package 的配置文件
include(CMakePackageConfigHelpers)

# 直接生成 DVP2Config.cmake
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/DVP2Config.in
    ${CMAKE_CURRENT_BINARY_DIR}/DVP2Config.cmake
    INSTALL_DESTINATION lib/cmake/DVP2
    PATH_VARS DVP2_INCLUDE_DIR DVP2_LIB_DIR DVP2_BIN_DIR
)

# 写入版本兼容性文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/DVP2ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 安装头文件、库和二进制文件
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY lib/     DESTINATION lib)
install(DIRECTORY bin/     DESTINATION bin)

# 安装导出的目标以便外部项目链接
install(EXPORT DVP2Targets
    EXPORT_LINK_INTERFACE_LIBRARIES
    FILE DVP2Targets.cmake
    NAMESPACE DVP2::
    DESTINATION lib/cmake/DVP2
)

# 安装配置文件
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/DVP2Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/DVP2ConfigVersion.cmake
    DESTINATION lib/cmake/DVP2
)
