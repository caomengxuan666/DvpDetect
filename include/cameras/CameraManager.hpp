#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "CameraCapture.hpp"
#include "config/CameraConfig.hpp"

class CameraManager {
 public:
  CameraManager() = default;
  ~CameraManager() = default;

  // 相机管理
  bool add_camera(const std::string& camera_id,
                  std::unique_ptr<CameraCapture> capture);
  void remove_camera(const std::string& camera_id);
  bool has_camera(const std::string& camera_id) const;
  CameraCapture* get_camera(const std::string& camera_id) const;
  std::vector<std::string> get_all_camera_ids() const;

  // 相机配置管理
  void set_camera_config(const std::string& camera_id,
                         const CameraConfig& config);
  CameraConfig get_camera_config(const std::string& camera_id) const;
  bool save_camera_config(const std::string& camera_id,
                          const std::string& file_path) const;
  bool load_camera_config(const std::string& camera_id,
                          const std::string& file_path);

  // 相机操作
  bool start_camera(const std::string& camera_id,
                    const FrameProcessor& processor);
  void stop_camera(const std::string& camera_id);

 private:
  struct CameraData {
    std::unique_ptr<CameraCapture> capture;
    CameraConfig config;
    std::string config_path;

    // 添加默认构造函数
    CameraData() = default;
    explicit CameraData(std::unique_ptr<CameraCapture> cap)
        : capture(std::move(cap)) {}
  };

  std::unordered_map<std::string, std::unique_ptr<CameraData>> cameras_;
};
