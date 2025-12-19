# auto moc auto rcc auto uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# -------------------------------
# 关键1：dvp_client_app 可执行文件需要查找 Qt
# -------------------------------
# 复用根目录缓存的 Qt6_DIR，避免重复查找
set(Qt6_DIR "${CMAKE_SOURCE_DIR}/../D:/qt/6.10.1/msvc2022_64/lib/cmake/Qt6" CACHE PATH "" FORCE)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    OpenGLWidgets
    Svg
)

#打印Qt路径
message(STATUS "Qt6_DIR: ${Qt6_DIR}")
message(STATUS "Qt6 include dirs: ${Qt6_INCLUDE_DIRS}") # 验证 Qt 头文件路径是否正确

qt_standard_project_setup()

# 设置目标名称
set(TARGET_NAME DvpClient)

# 设置源文件列表
set(SOURCES
    DvpMainWindow.cpp
    DvpConfigModel.cpp
    DvpConfigDelegate.cpp
    DvpCameraModel.cpp
    DvpCameraDelegate.cpp
    DvpCameraView.cpp
    DvpCameraManager.cpp
    DvpMultiCameraManager.cpp
    client.qrc
    main.cpp
)

set(HEADERS
    DvpMainWindow.h
    DvpConfigModel.h
    DvpConfigDelegate.h
    DvpCameraModel.h
    DvpCameraDelegate.h
    DvpCameraView.h
    DvpCameraManager.h
    DvpMultiCameraManager.h
)

# Add the main executable with all sources
qt6_add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})

# 设置头文件目录
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/BS_THREAD_POOL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/concurrentqueue
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/AntDesign/QtAntDesign
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/AntDesign/QtAntDesign/ThirdParty/FastGaussianBlur-main
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/AntDesign/QtAntDesign/ThirdParty/QR-Code-generator-master
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/DVP_SDK/include
)

target_sources(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../DvpCapture.cpp)
target_sources(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../DvpCameraBuilder.cpp)
target_sources(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../DvpEventManager.cpp)

# 链接所需库
target_link_libraries(${TARGET_NAME} PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::OpenGLWidgets
    Qt6::Svg
    QtAntDesign
    DVP2::DVPCamera 
    DVP2::dvpir
)

# Windows特定链接
if(WIN32)
    target_link_libraries(${TARGET_NAME} PRIVATE Dwmapi)
endif()

# 设置输出目录属性
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/bin
)

# 复制字体资源到输出目录
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/AntDesign/QtAntDesign/fonts
    $<TARGET_FILE_DIR:${TARGET_NAME}>/fonts
)

# 复制图片资源到输出目录
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/AntDesign/QtAntDesign/Imgs
    $<TARGET_FILE_DIR:${TARGET_NAME}>/Imgs
)

include(GNUInstallDirs)

install(TARGETS ${TARGET_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ${TARGET_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

include(${CMAKE_SOURCE_DIR}/cmake/DeployQt.cmake)

deploy_qt_deps(
${TARGET_NAME}                 # 要部署的可执行文件目标名
    "D:/qt/6.10.1/msvc2022_64"      # Qt 安装根目录（根据实际路径调整）
    ${CMAKE_BUILD_TYPE} STREQUAL "Debug"  # 自动判断 Debug/Release
)