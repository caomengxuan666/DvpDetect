#pragma once

// 这个文件的意图是直接从配置文件中读取相机的配置信息且应用
// 虽然每个相机都有自己的配置文件，我们也需要管理多个相机的实例。
#include <memory>
#include <string>

struct CameraConfig;
class CameraCapture;
enum class CameraBrand { DVP, IK, MIND };

std::unique_ptr<CameraCapture> create_camera(const CameraBrand& brand,
                                             const std::string& identifier,
                                             const CameraConfig& config);
