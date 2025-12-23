cmake_minimum_required(VERSION 3.15)

# -------------------------------
# 1. 核心：添加 OpenCV 按需查找的开关（默认关闭，仅手动开启时查找）
# -------------------------------
option(ENABLE_OPENCV "是否启用 OpenCV 功能（启用则查找，禁用则跳过）" OFF)
# 缓存 OpenCV 路径（手动指定一次，后续无需重复查找）
set(OpenCV_DIR "D:/vcpkg/installed/x64-windows/share/opencv4" CACHE PATH "OpenCV Config 目录（vcpkg 安装路径）")

# 添加测试选项
option(ENABLE_TESTS "是否启用测试" ON)

# -------------------------------
# 2. vcpkg 基础配置
# -------------------------------
set(VCPKG_ROOT "D:/vcpkg" CACHE PATH "vcpkg 根目录")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg 工具链文件")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "强制使用 x64 架构")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)  # 优先使用config模式
option(SAVE_RESULT_IMG ON)

# -------------------------------
# 3. Qt 查找（原有逻辑保留）
# -------------------------------
set(OpenCV_CUDA OFF CACHE BOOL "Disable OpenCV CUDA detection" FORCE)
set(OpenCV_OPENCL OFF CACHE BOOL "Disable OpenCV OpenCL detection" FORCE)

set(QT_DIR $ENV{QT_DIR})
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_Qt_Dependency.cmake) # 确保模块路径正确

project(DvpDetectDemo)

set(CMAKE_CXX_STANDARD 17)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 在WINDOWS上禁用minmax宏
if (WIN32)
    add_definitions(-DNOMINMAX)
    if(MSVC)
        # 天杀的QT和这个冲突了，所以我们只能对SDK的所有目标应用这个了。
        #add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
        # add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")
    endif()
endif()

# 添加cmake模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 查找DVP包
find_package(DVP2 REQUIRED)

# -------------------------------
# 4. 核心优化：按需查找 OpenCV + 缓存结果
# -------------------------------
if (ENABLE_OPENCV)
    # 1. 手动指定 OpenCV 路径（仅需配置一次，根据你的 vcpkg 路径调整）
    set(OpenCV_ROOT "D:/vcpkg/installed/x64-windows")
    set(OpenCV_INCLUDE_DIRS 
        "${OpenCV_ROOT}/include/opencv4"  # OpenCV 头文件主目录
    )
    # 2. 手动指定 OpenCV 库文件（区分 Debug/Release）
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OpenCV_LIBS
            "${OpenCV_ROOT}/debug/lib/opencv_core4d.lib"
            "${OpenCV_ROOT}/debug/lib/opencv_imgproc4d.lib"
            "${OpenCV_ROOT}/debug/lib/opencv_highgui4d.lib"
            "${OpenCV_ROOT}/debug/lib/opencv_imgcodecs4d.lib"
        )
    else()
        set(OpenCV_LIBS
            "${OpenCV_ROOT}/lib/opencv_core4.lib"
            "${OpenCV_ROOT}/lib/opencv_imgproc4.lib"
            "${OpenCV_ROOT}/lib/opencv_highgui4.lib"
            "${OpenCV_ROOT}/lib/opencv_imgcodecs4.lib"
        )
    endif()

    # 3. 缓存路径（后续构建直接复用）
    set(OpenCV_FOUND TRUE CACHE BOOL "OpenCV 查找状态" FORCE)
    set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} CACHE PATH "OpenCV 头文件目录" FORCE)
    set(OpenCV_LIBS ${OpenCV_LIBS} CACHE PATH "OpenCV 库文件列表" FORCE)

    message(STATUS "✅ 已启用 OpenCV（手动硬编码路径）：${OpenCV_ROOT}")
else()
    set(OpenCV_FOUND FALSE CACHE BOOL "OpenCV 查找状态" FORCE)
    message(STATUS "❌ 未启用 OpenCV，跳过所有操作")
endif()

set(OPENCV_INCLUDE_DIRS "D:/vcpkg/installed/x64-windows/include/opencv4")
include_directories(${OPENCV_INCLUDE_DIRS})
message(STATUS "OpenCV_INCLUDE_DIRS: " ${OPENCV_INCLUDE_DIRS})

# 启用测试
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# -------------------------------
# 5. 原有第三方库逻辑（保留）
# -------------------------------
# 包含concurrentqueue库
add_subdirectory(third_party/concurrentqueue)

# 包含BS::thread_pool库
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/BS_THREAD_POOL/include")

# 包含QtAntDesign库
add_subdirectory(third_party/AntDesign)

add_subdirectory(src)

