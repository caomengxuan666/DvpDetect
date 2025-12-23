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
 *  - File: HoleDetection.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#pragma once

#include <shared_mutex>
#include <vector>

#include "AlgoBase.hpp"
#include "algo/AlgorithmConfigTraits.hpp"
#include "config/AlogoParams.hpp"
#include "config/ConfigObserver.hpp"
namespace algo {

template <>
struct AlgorithmConfigExtractor<config::HoleDetectionConfig> {
  static const config::HoleDetectionConfig& extract(
      const config::GlobalConfig& global_cfg) {
    return global_cfg.hole_detection;
  }
};

struct PartitionConfig {
  double left_ratio = 0.3;
  double mid_ratio = 0.4;
  double right_ratio = 0.3;
  int left_thresh = 20;
  int mid_thresh = 23;
  int right_thresh = 20;
};

class HoleDetection : public AlgoBase {
 public:
  ALGO_METADATA("HoleDetection", "针孔检测")
  using Config = config::HoleDetectionConfig;

  HoleDetection();
  explicit HoleDetection(const Config& cfg);
  void process(const CapturedFrame& frame) override;

  std::vector<AlgoParamInfo> get_parameter_info() const override;
  std::vector<AlgoSignalInfo> get_signal_info() const override;

  void update_config(const Config& new_cfg);

 private:
  void parse_partition_params();

 private:
  Config config_;
  PartitionConfig parsed_params_;
  mutable std::shared_mutex config_mutex_;
};

}  // namespace algo
