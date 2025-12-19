#pragma once
#include <array>

#include "AlgoBase.hpp"

namespace algo {

// 分区配置结构体，用于定义图像不同区域的比例和阈值
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
  HoleDetection();

  void process(const CapturedFrame& frame) override;

  // ==================== Static Configuration ====================
  static double DEFAULT_PIXEL_PER_MM;
  static bool ENABLE_REAL_WORLD_CALCULATION;
  static int MIN_DEFECT_AREA;
  static int EDGE_MARGIN;
  static int MERGE_DISTANCE_THRESHOLD;
  static double PIXEL_TO_MM_WIDTH;
  static double PIXEL_TO_MM_HEIGHT;
  static PartitionConfig PARTITION_PARAMS;
};

}  // namespace algo
