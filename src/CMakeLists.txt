include_directories(${CMAKE_SOURCE_DIR}/include)
# 创建测试可执行文件

# 1. 递归遍历所有 .cpp 文件（先获取全部文件）
file(GLOB_RECURSE DVPDETECT_SOURCE
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}  # 可选：使路径相对当前目录，更清晰
    "*.cpp"
)

# 2. 过滤掉所有包含 "/client/" 路径的文件（核心步骤）
list(FILTER DVPDETECT_SOURCE EXCLUDE REGEX "client/")

# 过滤文件,example.cpp
list(FILTER DVPDETECT_SOURCE EXCLUDE REGEX "example.cpp")

# 打印文件列表
message("DVPDETECT_SOURCE: ${DVPDETECT_SOURCE}")


add_library(DVPDETECT STATIC ${DVPDETECT_SOURCE})
if(MSVC)
    target_compile_options(DVPDETECT PRIVATE
        /source-charset:utf-8
        /execution-charset:utf-8
    )
endif()

target_link_libraries(DVPDETECT PRIVATE 
    DVP2::DVPCamera 
    DVP2::dvpir
    concurrentqueue
    BS_thread_pool
    PUBLIC
    ${OpenCV_LIBS}
    DVPUtils
    )

if(SAVE_RESULT_IMG)
    target_compile_definitions(DVPDETECT PRIVATE SAVE_RESULT_IMAGE_QUEUE)
endif()

add_executable(main ${DVPDETECT_SOURCE})

if(MSVC)
    target_compile_options(main PRIVATE
        /source-charset:utf-8
        /execution-charset:utf-8
    )
endif()


# 链接DVP库和第三方库
target_link_libraries(main PRIVATE 
    DVP2::DVPCamera 
    DVP2::dvpir
    DVPUtils
    concurrentqueue
    BS_thread_pool
    ${OpenCV_LIBS}
    )

if(SAVE_RESULT_IMG)
    target_compile_definitions(main PRIVATE SAVE_RESULT_IMAGE_QUEUE)
endif()

add_subdirectory(utils)
add_subdirectory(client)
