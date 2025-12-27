#include "cameras/CameraManager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "config/CameraConfig.hpp"

bool CameraManager::add_camera(const std::string& camera_id,
                               std::unique_ptr<CameraCapture> capture) {
  if (cameras_.find(camera_id) != cameras_.end()) {
    return false;  // 相机已存在
  }

  auto camera_data = std::make_unique<CameraData>(std::move(capture));
  cameras_.emplace(camera_id, std::move(camera_data));
  return true;
}

void CameraManager::remove_camera(const std::string& camera_id) {
  cameras_.erase(camera_id);
}

bool CameraManager::has_camera(const std::string& camera_id) const {
  return cameras_.find(camera_id) != cameras_.end();
}

CameraCapture* CameraManager::get_camera(const std::string& camera_id) const {
  auto it = cameras_.find(camera_id);
  if (it != cameras_.end()) {
    return it->second->capture.get();
  }
  return nullptr;
}

std::vector<std::string> CameraManager::get_all_camera_ids() const {
  std::vector<std::string> ids;
  for (const auto& pair : cameras_) {
    ids.push_back(pair.first);
  }
  return ids;
}

void CameraManager::set_camera_config(const std::string& camera_id,
                                      const CameraConfig& config) {
  auto it = cameras_.find(camera_id);
  if (it != cameras_.end()) {
    it->second->config = config;
    it->second->capture->set_config(config);
  }
}

CameraConfig CameraManager::get_camera_config(
    const std::string& camera_id) const {
  auto it = cameras_.find(camera_id);
  if (it != cameras_.end()) {
    return it->second->config;
  }
  return CameraConfig{};
}

bool CameraManager::save_camera_config(const std::string& camera_id,
                                       const std::string& file_path) const {
  auto it = cameras_.find(camera_id);
  if (it == cameras_.end()) {
    return false;
  }

  // TODO: 实现相机配置保存逻辑
  // 保存配置到文件
  (void)file_path;
  return true;
}

bool CameraManager::load_camera_config(const std::string& camera_id,
                                       const std::string& file_path) {
  auto it = cameras_.find(camera_id);
  if (it == cameras_.end()) {
    return false;
  }

  // TODO: 实现相机配置加载逻辑
  // 从文件加载配置
  (void)file_path;
  return true;
}

bool CameraManager::start_camera(const std::string& camera_id,
                                 const FrameProcessor& processor) {
  auto it = cameras_.find(camera_id);
  if (it != cameras_.end()) {
    return it->second->capture->start(processor);
  }
  return false;
}

void CameraManager::stop_camera(const std::string& camera_id) {
  auto it = cameras_.find(camera_id);
  if (it != cameras_.end()) {
    it->second->capture->stop();
  }
}
