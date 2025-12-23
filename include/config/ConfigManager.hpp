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
 *  - File: ConfigManager.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

// config/ConfigManager.hpp
#pragma once
#include <memory>
#include <utility>
#include <vector>

#include "AlogoParams.hpp"
#include "algo/GenericAlgorithmConfigObserver.hpp"
#include "config/AlogoParams.hpp"
#include "config/ConfigObserver.hpp"

namespace config {

class ConfigManager {
 public:
  static ConfigManager& instance() {
    static ConfigManager mgr;
    return mgr;
  }

  void start();
  GlobalConfig get_current_config() const;

  // ===== 自动注册核心 API =====
  template <typename AlgoType>
  std::shared_ptr<AlgoType> create_algorithm() {
    // we read the global config here
    auto config = get_current_config();
    // And we extract the concrete config from the global config by the Algo
    // type For each Algo type,we must implement the specific extract function
    // You can see the template in algo/AlgorithmConfigTraits.hpp
    // @AlgorithmConfigExtractor
    auto algo_config = extract_algo_config<AlgoType>(config);
    auto algo = std::make_shared<AlgoType>(algo_config);

    // 自动创建并注册观察者
    auto observer =
        std::make_unique<algo::GenericAlgorithmConfigObserver<AlgoType>>();
    observer->add_algorithm(algo);
    addObserver(observer.get());

    // 保持观察者存活
    observers_.push_back(std::move(observer));
    return algo;
  }

 private:
  ConfigManager() = default;
  void addObserver(ConfigObserver* obs);

  // Trait: 从 GlobalConfig 提取特定算法配置
  template <typename AlgoType>
  static auto extract_algo_config(const GlobalConfig& cfg) {
    return algo::AlgorithmConfigExtractor<typename AlgoType::Config>::extract(
        cfg);
  }

  std::unique_ptr<ConfigLoader> loader_;
  std::unique_ptr<GlobalConfig> current_config_;
  std::vector<std::unique_ptr<ConfigObserver>> observers_;  // 保持观察者存活
  mutable std::mutex mutex_;
};

void ConfigManager::start() {
  if (loader_) {
    return;
  }
  loader_ = std::make_unique<ConfigLoader>();
  current_config_ = loader_->load();
}

GlobalConfig ConfigManager::get_current_config() const {
  std::lock_guard<std::mutex> lock(mutex_);
  if (current_config_) {
    return *current_config_;
  }
  return {};
}

void ConfigManager::addObserver(ConfigObserver* obs) {
  if (loader_ && obs) {
    loader_->addObserver(obs);
  }
}
}  // namespace config
