# 通用 Qt 依赖部署函数（适配 Qt 6.10+ Windows 专用）
# 参数说明：
#   TARGET_NAME: 要部署的可执行文件目标名（如 DvpClient）
#   QT_ROOT_DIR: Qt 安装根目录（如 D:/qt/6.10.1/msvc2022_64）
#   DEBUG_MODE: 是否为 Debug 模式
function(deploy_qt_deps TARGET_NAME QT_ROOT_DIR DEBUG_MODE)
    # 仅在 Windows 平台生效
    if(NOT WIN32)
        message(STATUS "部署函数仅支持 Windows 平台，跳过 Qt 依赖部署")
        return()
    endif()

    # 1. 强制指定系统 Qt 的 windeployqt（避开 vcpkg 的 Qt）
    set(WINDEPLOYQT_EXECUTABLE "${QT_ROOT_DIR}/bin/windeployqt.exe")
    if(NOT EXISTS "${WINDEPLOYQT_EXECUTABLE}")
        message(FATAL_ERROR "未找到系统 Qt 的 windeployqt.exe！路径：${WINDEPLOYQT_EXECUTABLE}")
    endif()
    message(STATUS "使用系统 Qt 的 windeployqt：${WINDEPLOYQT_EXECUTABLE}")

    # 2. 获取目标程序的完整路径
    set(TARGET_FILE_PATH "$<TARGET_FILE:${TARGET_NAME}>")
    set(TARGET_FILE_DIR "$<TARGET_FILE_DIR:${TARGET_NAME}>")

    # 3. 构造 windeployqt 命令参数（移除 Qt 6.10 不支持的 --no-angle）
    set(WINDEPLOYQT_ARGS
        --verbose 1                  # 输出日志
        --no-compiler-runtime        # 不复制 VS 运行时
        --no-system-d3d-compiler     # 不复制系统 D3D 编译器
        --no-opengl-sw               # Qt 6.10 仍支持该参数，保留
    )
    # Debug/Release 模式适配
    if(DEBUG_MODE)
        list(APPEND WINDEPLOYQT_ARGS --debug)
    else()
        list(APPEND WINDEPLOYQT_ARGS --release)
    endif()
    list(APPEND WINDEPLOYQT_ARGS "${TARGET_FILE_PATH}")

    # 4. 构建后执行 windeployqt（用 COMMAND_EXPAND_LISTS 解析参数列表）
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" ${WINDEPLOYQT_ARGS}
        WORKING_DIRECTORY "${TARGET_FILE_DIR}"
        COMMENT "自动部署 Qt 依赖库到 ${TARGET_NAME} 目录（Qt 6.10+）..."
        VERBATIM
        COMMAND_EXPAND_LISTS  # 关键：解析参数列表，避免空格问题
    )

    # 额外：复制 AntDesign 的字体/图片资源（你的项目需要）
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/third_party/AntDesign/QtAntDesign/fonts"
            "${TARGET_FILE_DIR}/fonts"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/third_party/AntDesign/QtAntDesign/Imgs"
            "${TARGET_FILE_DIR}/Imgs"
        COMMENT "复制 AntDesign 资源文件..."
        VERBATIM
    )

    message(STATUS "已为 ${TARGET_NAME} 配置 Qt 6.10+ 自动部署（${DEBUG_MODE} 模式）")
endfunction()