/*
 *  Copyright © 2025 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: AlogoParams.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#pragma once
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "ConfigObserver.hpp"
#include "utils/executable_path.h"
#include "utils/inicpp.hpp"

namespace config {
constexpr const char *CONFIG_FILE = "config.ini";
inline std::string g_config_file_path;

inline std::string get_config_file_path() {
  static std::string config_file_path = []() {
    std::filesystem::path exe_dir(DvpUtils::getExecutableDirectory());
    return (exe_dir / CONFIG_FILE).string();
  }();
  return config_file_path;
}
inline void set_config_file_path(const std::string &path) {
  g_config_file_path = path;
}

inline std::string get_default_config_path() {
  if (!g_config_file_path.empty()) {
    return g_config_file_path;
  }
  return get_config_file_path();
}

struct HoleDetectionConfig {
  float pixel_per_mm;
  bool enable_real_world_calculation;
  int min_defect_area;
  int edge_margin;
  int merge_distance_threshold;
  float pixel_to_mm_width;
  float pixel_to_mm_height;
  std::string partition_params;

  static HoleDetectionConfig load(inicpp::IniManager &ini) {
    try {
      auto hole_section = ini["hole_detection"];
      HoleDetectionConfig config;

      config.pixel_per_mm =
          hole_section["pixel_per_mm"].String().empty()
              ? 50.0f
              : std::stof(hole_section["pixel_per_mm"].String());

      config.enable_real_world_calculation =
          hole_section["enable_real_world_calculation"].String().empty()
              ? true
              : static_cast<bool>(
                    hole_section["enable_real_world_calculation"]);

      config.min_defect_area =
          hole_section["min_defect_area"].String().empty()
              ? 1
              : std::stoi(hole_section["min_defect_area"].String());

      config.edge_margin = hole_section["edge_margin"].String().empty()
                               ? 10
                               : static_cast<int>(hole_section["edge_margin"]);

      config.merge_distance_threshold =
          hole_section["merge_distance_threshold"].String().empty()
              ? 20
              : static_cast<int>(hole_section["merge_distance_threshold"]);

      config.pixel_to_mm_width =
          hole_section["pixel_to_mm_width"].String().empty()
              ? 0.05586f
              : std::stof(hole_section["pixel_to_mm_width"]);

      config.pixel_to_mm_height =
          hole_section["pixel_to_mm_height"].String().empty()
              ? 0.061f
              : std::stof(hole_section["pixel_to_mm_height"]);

      config.partition_params =
          hole_section["partition_params"].String().empty()
              ? "0.3,0.4,0.3,20,23,20"
              : hole_section["partition_params"].String();

      return config;
    } catch (const std::exception &e) {
      std::cerr << "Exception: " << e.what() << "when loading params"
                << std::endl;
      throw;
    } catch (...) {
      std::cerr << "Unknown exception when loading params from ini"
                << std::endl;
      throw;
    }
  }

  static void saveDefaults(inicpp::IniManager &ini) {
    ini.set("hole_detection", "pixel_per_mm", 50.0f, "每毫米像素数");
    ini.set("hole_detection", "enable_real_world_calculation", true,
            "是否启用真实世界尺寸计算");
    ini.set("hole_detection", "min_defect_area", 1, "最小缺陷面积(像素)");
    ini.set("hole_detection", "edge_margin", 10, "边缘边距(像素)");
    ini.set("hole_detection", "merge_distance_threshold", 20,
            "合并距离阈值(像素)");
    ini.set("hole_detection", "pixel_to_mm_width", 0.05586f,
            "像素到毫米宽度转换系数");
    ini.set("hole_detection", "pixel_to_mm_height", 0.061f,
            "像素到毫米高度转换系数");
    ini.set("hole_detection", "partition_params", "0.3,0.4,0.3,20,23,20",
            "分区参数(左中右比例和阈值)");
  }
};

struct GlobalConfig {
  std::string title;
  HoleDetectionConfig hole_detection;
  static GlobalConfig load() {
    try {
      inicpp::IniManager ini(get_config_file_path());

      GlobalConfig config;
      config.title = ini[""]["title"].String().empty()
                         ? "DvpDetect"
                         : ini[""]["title"].String();
      config.hole_detection = HoleDetectionConfig::load(ini);
      return config;
    } catch (std::exception &e) {
      throw;
    }
  }

  static void saveDefaults(inicpp::IniManager &ini) {
    ini.set("", "title", "DvpDetect", "应用标题");
    HoleDetectionConfig::saveDefaults(ini);
  }
};

class ConfigLoader;

inline std::unique_ptr<ConfigLoader> g_config_loader;
inline std::unique_ptr<GlobalConfig> g_current_config;
inline std::mutex g_config_mutex;
inline std::atomic<bool> g_config_initialized{false};

class ConfigLoader {
 protected:
  std::vector<ConfigObserver *> observers_;
  std::atomic<bool> monitoring_active{false};
  std::thread monitor_thread;

  virtual std::unique_ptr<GlobalConfig> loadFromStaticFile() {
    return std::make_unique<GlobalConfig>(GlobalConfig::load());
  }
  virtual void startMonitoring(GlobalConfig *config) {
    if (monitoring_active.load()) {
      return;
    }
    monitoring_active = true;

    monitor_thread = std::thread([this, config]() {
      std::filesystem::file_time_type last_write = {};
      while (monitoring_active.load()) {
        try {
          auto path = std::filesystem::path(get_config_file_path());
          if (std::filesystem::exists(path)) {
            auto curr_time = std::filesystem::last_write_time(path);
            if (curr_time != last_write) {
              std::cout << "Config file changed , Reloading..." << std::endl;
              auto newConfig = loadFromStaticFile();
              if (newConfig) {
                std::lock_guard<std::mutex> lock(g_config_mutex);
                *config = *newConfig;
                notifyObservers(*config);
                last_write = curr_time;
              }
            }
          }
          std::this_thread::sleep_for(std::chrono::seconds(2));
        } catch (...) {
          if (monitoring_active.load()) {
            std::cout << "Error while monitoring config file" << std::endl;
          }
          break;
        }
      }
    });
  }

  void notifyObservers(const GlobalConfig &config) {
    for (auto *obs : observers_) {
      try {
        obs->onConfigReloaded(config);
      } catch (const std::exception &e) {
        std::cout << "Exception in observer: " << e.what() << std::endl;
      }
    }
  }

 public:
  virtual ~ConfigLoader() {
    monitoring_active = false;
    if (monitor_thread.joinable()) {
      monitor_thread.join();
    }
  }

  void addObserver(ConfigObserver *obs) {
    if (obs) {
      std::lock_guard<std::mutex> lock(g_config_mutex);
      if (std::find(observers_.begin(), observers_.end(), obs) ==
          observers_.end()) {
        observers_.push_back(obs);
      }
    }
  }

  void removeObserver(ConfigObserver *obs) {
    std::lock_guard<std::mutex> lock(g_config_mutex);
    observers_.erase(std::remove(observers_.begin(), observers_.end(), obs),
                     observers_.end());
  }

  std::unique_ptr<GlobalConfig> load() {
    std::unique_ptr<GlobalConfig> config;

    config = loadFromStaticFile();
    startMonitoring(config.get());
    notifyObservers(*config);
    return config;
  }
};

inline GlobalConfig get_current_config() {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  return *g_current_config;
}

inline void update_current_config(const GlobalConfig &newConfig) {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  *g_current_config = newConfig;
}

}  // namespace config
