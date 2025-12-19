include_directories(${CMAKE_SOURCE_DIR}/include)
# 创建测试可执行文件

set(DVPDETECT_SOURCE
    DvpCameraBuilder.cpp
    DvpCapture.cpp
    DvpEventManager.cpp
)

set(DVPALGO_SOURCE
    algo/HoleDetection.cpp
)

add_executable(example example.cpp ${DVPDETECT_SOURCE})

if(MSVC)
    target_compile_options(example PRIVATE
        /source-charset:utf-8
        /execution-charset:utf-8
    )
endif()

target_sources(example PRIVATE ${DVPALGO_SOURCE})

# 链接DVP库和第三方库
target_link_libraries(example PRIVATE 
    DVP2::DVPCamera 
    DVP2::dvpir
    concurrentqueue
    BS_thread_pool
    ${OpenCV_LIBS}
    )

if(SAVE_RESULT_IMG)
    target_compile_definitions(example PRIVATE SAVE_RESULT_IMAGE_QUEUE)
endif()

add_subdirectory(client)