include(FetchContent)

# Download Gtest to third_party directory

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0  # 固定版本
  GIT_SHALLOW    ON       # 浅克隆
  SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/googletest"  # 下载到third_party目录
)

FetchContent_MakeAvailable(googletest)

# 获取主项目包含目录
get_filename_component(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE)
get_filename_component(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party" ABSOLUTE)

file(GLOB_RECURSE TEST_SOURCES *.h *.cc *.cpp *.hpp)

# 创建测试可执行文件
add_executable(DvpDetectTests
    ${TEST_SOURCES}
)

# 添加包含目录
target_include_directories(DvpDetectTests PRIVATE 
    ${PROJECT_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    "${THIRD_PARTY_DIR}/googletest/googletest/include"
    "${THIRD_PARTY_DIR}/BS_THREAD_POOL/include"
    "${THIRD_PARTY_DIR}/concurrentqueue"
    "${THIRD_PARTY_DIR}/DVP_SDK/include"
)

# 链接gtest和项目库
target_link_libraries(DvpDetectTests 
    gtest_main
    gmock_main
    DVP2::DVPCamera 
    DVP2::dvpir
    DVPDETECT    concurrentqueue
    BS_thread_pool
    ${OpenCV_LIBS}
)

if(MSVC)
    target_compile_options(DvpDetectTests PRIVATE
        /source-charset:utf-8
        /execution-charset:utf-8
    )
endif()

# 发现并添加所有测试
include(GoogleTest)
gtest_discover_tests(DvpDetectTests)