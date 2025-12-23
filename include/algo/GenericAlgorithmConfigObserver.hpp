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
 *  - File: GenericAlgorithmConfigObserver.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

// algo/GenericAlgorithmConfigObserver.hpp
#pragma once
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "algo/AlgorithmConfigTraits.hpp"
#include "config/ConfigObserver.hpp"

namespace algo {

template <typename AlgoType>
class GenericAlgorithmConfigObserver : public config::ConfigObserver {
 public:
  using AlgoConfigType = typename AlgoType::Config;

  void add_algorithm(std::shared_ptr<AlgoType> algo) {
    if (!algo) {
      return;
    }
    std::lock_guard lock(mutex_);
    algorithms_.push_back(std::move(algo));
  }

  void remove_algorithm(const std::shared_ptr<AlgoType>& algo) {
    std::lock_guard lock(mutex_);
    algorithms_.erase(
        std::remove_if(algorithms_.begin(), algorithms_.end(),
                       [&algo](const auto& ptr) { return ptr == algo; }),
        algorithms_.end());
  }

  void onConfigReloaded(const config::GlobalConfig& new_config) override {
    // 1. 提取配置（类型安全）
    const auto& algo_config =
        AlgorithmConfigExtractor<AlgoConfigType>::extract(new_config);

    // 2. 批量更新所有算法实例
    std::lock_guard lock(mutex_);
    for (auto& algo : algorithms_) {
      if (algo) {
        algo->update_config(algo_config);
      }
    }
  }

 private:
  std::vector<std::shared_ptr<AlgoType>> algorithms_;
  mutable std::mutex mutex_;
};

}  // namespace algo
