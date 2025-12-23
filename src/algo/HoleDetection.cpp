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
 *  - File: HoleDetection.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

/**
 * @file HoleDetection.cpp
 * @brief Implementation of hole detection algorithm for industrial inspection
 *
 * Processing flow:
 * 1. Entry points:
 *    - process(const CapturedFrame&) - Public interface called by
 * FrameProcessor
 *    - process_single_image(const std::string&, const std::string&) - Process
 * image file
 *    - process_single_image(const Mat&) - Process video frame
 *
 * 2. Main processing chain:
 *    process() -> process_single_image(Mat) -> process_single_image_impl()
 *    process_single_image(string, string) -> process_single_image_impl()
 *
 * 3. Detailed steps in process_single_image_impl():
 *    a. Preprocessing:
 *       - preprocess_for_hole_detection() -> preprocess_image_fast()
 *         * Convert to grayscale if needed
 *         * Crop image to remove mostly white borders
 *    b. Thresholding:
 *       - threshold_image() -> apply_partitioned_threshold_parallel()
 *         * Apply different thresholds to different image partitions
 *         * Uses parallel processing for large images
 *    c. Hole extraction:
 *       - extract_holes() -> connectedComponentsWithStats()
 *         * Identify connected components
 *         * Filter by minimum area and edge margins
 *         * Calculate hole properties (center, diameter, area, etc.)
 *    d. Hole merging:
 *       - merge_holes() -> merge_close_holes()
 *         * Combine nearby holes based on distance threshold
 *    e. Visualization and output:
 *       - For file processing: create_visualizations() and save_results()
 *       - For video frames: Just output statistics
 */

// 是否启用详细日志输出和计时功能的编译期开关
#ifndef ENABLE_HOLE_DETECTION_LOGGING
#define ENABLE_HOLE_DETECTION_LOGGING 1
#endif

#include "algo/HoleDetection.hpp"
//
#include <algorithm>
#include <chrono>
#include <filesystem>  //NOLINT
#include <ios>
#include <iostream>
#include <ratio>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// for opencv
#include <opencv2/opencv.hpp>
// utils

using namespace algo;         // NOLINT
using namespace cv;           // NOLINT
using namespace std::chrono;  // NOLINT
using std::cout, std::endl, std::setprecision, std::fixed, std::cerr;
namespace fs = std::filesystem;

// 条件编译宏定义，用于控制日志输出和计时功能
#if ENABLE_HOLE_DETECTION_LOGGING
#define HOLE_DETECTION_LOG(x) cout << x
#define HOLE_DETECTION_TIMING_START(name) \
  auto name##_start = high_resolution_clock::now()
#define HOLE_DETECTION_TIMING_END(name, msg)                                 \
  auto name##_end = high_resolution_clock::now();                            \
  const double name##_ms =                                                   \
      duration_cast<duration<double, std::milli>>(name##_end - name##_start) \
          .count();                                                          \
  cout << (msg) << name##_ms << " ms" << endl

#define HOLE_DETECTION_TIMING_ONLY(name)                                     \
  auto name##_end = high_resolution_clock::now();                            \
  const double name##_ms =                                                   \
      duration_cast<duration<double, std::milli>>(name##_end - name##_start) \
          .count()
#else
#define HOLE_DETECTION_LOG(x)
#define HOLE_DETECTION_TIMING_START(name)
#define HOLE_DETECTION_TIMING_END(name, msg)
#define HOLE_DETECTION_TIMING_ONLY(name)
#endif

constexpr double M_PI{3.1415926535897932384626433832795};

__forceinline static bool is_big_image(const Mat& image) noexcept {
  return image.cols > 1000 && image.rows > 1000;
}

__forceinline static double calculate_real_world_diameter(
    double pixel_diameter, double pixel_per_mm) noexcept {
  return pixel_diameter / pixel_per_mm;
}

__forceinline double euclidean_distance(const Point& a,
                                        const Point& b) noexcept {
  double dx = static_cast<double>(a.x - b.x);
  double dy = static_cast<double>(a.y - b.y);
  return std::sqrt(dx * dx + dy * dy);
}

static cv::Mat CapturedFrame2Mat(const CapturedFrame& frame) {
  // 可选：校验数据大小（根据 CV_8UC3 假设）
  // assert(frame.data.size() ==
  //        static_cast<size_t>(frame.width() * frame.height() * 3));

  return cv::Mat(frame.height(), frame.width(), CV_8UC3,
                 const_cast<uint8_t*>(frame.data.data()));
}

static std::vector<std::string> get_image_files(const std::string& dir) {
  std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp",
                                         ".tiff"};
  std::vector<std::string> files;
  for (const auto& entry : fs::directory_iterator(dir)) {
    if (entry.is_regular_file()) {
      std::string ext = entry.path().extension().string();
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      if (find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
        files.push_back(entry.path().string());
      }
    }
  }
  sort(files.begin(), files.end());
  return files;
}

static std::pair<int, int> find_horizontal_content_bounds_gray(
    const Mat& gray_image, double threshold_ratio = 0.1) noexcept {
  int height = gray_image.rows;
  int width = gray_image.cols;
  if (!is_big_image(gray_image)) {
    return {0, width - 1};
  }
  // 采样行数（For 2600行，sample 325行）
  const int SAMPLE_STEP = height > 1000 ? 8 : 1;
  const int SAMPLED_HEIGHT = (height + SAMPLE_STEP - 1) / SAMPLE_STEP;

  // 预计算采样行的指针
  std::vector<const uchar*> row_ptrs;
  row_ptrs.reserve(SAMPLED_HEIGHT);
  for (int y = 0; y < height; y += SAMPLE_STEP) {
    row_ptrs.push_back(gray_image.ptr<uchar>(y));
  }

  // 白色边的阈值
  const uchar WHITE_THRESHOLD = 200;
  const double threshold_count = SAMPLED_HEIGHT * threshold_ratio;

  // 检查左边是否需要裁剪
  int x_min = 0;
  int left_white_count = 0;
  for (const auto* row : row_ptrs) {
    if (row[0] > WHITE_THRESHOLD) {
      ++left_white_count;
    }
  }

  if (static_cast<double>(left_white_count) / SAMPLED_HEIGHT >
      threshold_ratio) {
    // 二分查左边界
    int left = 0, right = width - 1;
    while (left < right) {
      int mid = (left + right) / 2;
      int white_count = 0;
      for (const auto* row : row_ptrs) {
        if (row[mid] > WHITE_THRESHOLD) {
          ++white_count;
        }
      }

      if (static_cast<double>(white_count) / SAMPLED_HEIGHT > threshold_ratio) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }
    x_min = left;
  }

  // 检查右边是否需要裁剪
  int x_max = width - 1;
  int right_white_count = 0;
  for (const auto* row : row_ptrs) {
    if (row[width - 1] > WHITE_THRESHOLD) {
      ++right_white_count;
    }
  }

  if (static_cast<double>(right_white_count) / SAMPLED_HEIGHT >
      threshold_ratio) {
    // 二分查找右边界
    int left = 0, right = width - 1;
    while (left < right) {
      int mid = (left + right + 1) / 2;
      int white_count = 0;
      for (const auto* row : row_ptrs) {
        if (row[mid] > WHITE_THRESHOLD) {
          ++white_count;
        }
      }

      if (static_cast<double>(white_count) / SAMPLED_HEIGHT > threshold_ratio) {
        right = mid - 1;
      } else {
        left = mid;
      }
    }
    x_max = left;
  }

  if (x_min == 0 && x_max >= width - 5) {
    return {-1, -1};
  }

  constexpr int offset = 100;
  x_min = max(0, x_min + offset);
  x_max = min(width - 1, x_max - offset);

  if (x_min >= x_max) {
    return {-1, -1};
  }

  return {x_min, x_max};
}

static Mat preprocess_image_fast(const Mat& image) noexcept {
  HOLE_DETECTION_TIMING_START(total);

  // 直接获取灰度图（如果是彩色才转换）
  Mat gray;
  if (image.channels() == 3) {
    HOLE_DETECTION_TIMING_START(cvt);
    cvtColor(image, gray, COLOR_BGR2GRAY);
    HOLE_DETECTION_TIMING_END(cvt, "    Color conversion: ");
  } else {
    gray = image;  // 直接使用，不拷贝
  }

  // 直接在灰度图上找边界（跳过二值化！）
  HOLE_DETECTION_TIMING_START(bounds);
  auto [x_min, x_max] = find_horizontal_content_bounds_gray(gray);
  HOLE_DETECTION_TIMING_END(bounds, "    Bounds search: ");

  if (x_min == -1 || x_max == -1) {
    HOLE_DETECTION_TIMING_END(total, "    Total preprocessing: ");
    HOLE_DETECTION_LOG("    Total preprocessing: " << total_ms
                                                   << " ms (no crop)" << endl);
    return image;
  }

  constexpr int margin = 10;
  x_min = max(0, x_min - margin);
  x_max = min(gray.cols - 1, x_max + margin);

  HOLE_DETECTION_TIMING_START(crop);
  Mat cropped = image(Range::all(), Range(x_min, x_max + 1));
  HOLE_DETECTION_TIMING_END(crop, "    Cropping:      ");

  HOLE_DETECTION_TIMING_END(total, "    Total preprocessing: ");
  HOLE_DETECTION_LOG("    Cropped from " << image.cols << "x" << image.rows
                                         << " to " << cropped.cols << "x"
                                         << cropped.rows << endl);

  return cropped;
}

// ==================== PARALLEL THRESHOLDING ====================
struct ParallelThresholdTask : public cv::ParallelLoopBody {
  const Mat& src;
  Mat& dst;
  int start_col, end_col;
  int thresh;

  ParallelThresholdTask(const Mat& src_, Mat& dst_, int s, int e, int t)
      : src(src_), dst(dst_), start_col(s), end_col(e), thresh(t) {}

  void operator()(const cv::Range& rowRange) const override {
    for (int r = rowRange.start; r < rowRange.end; ++r) {
      Mat src_row = src.row(r).colRange(start_col, end_col);
      Mat dst_row = dst.row(r).colRange(start_col, end_col);
      threshold(src_row, dst_row, thresh, 255, THRESH_BINARY);
    }
  }
};

static Mat apply_partitioned_threshold_parallel(
    const cv::Mat& image, const PartitionConfig& params) noexcept {
  if (!is_big_image(image)) {
    // 处理整个图像，使用中间阈值（参数中为7.0）
    Mat binary;
    cv::threshold(image, binary, params.mid_thresh, 255, cv::THRESH_BINARY);
    return binary;
  }

  int width = image.cols;
  Mat binary = Mat::zeros(image.size(), CV_8UC1);

  int left_end = static_cast<int>(width * params.left_ratio);
  int mid_end =
      static_cast<int>(width * (params.left_ratio + params.mid_ratio));

  int rows = image.rows;

  if (left_end > 0) {
    cv::parallel_for_(
        cv::Range(0, rows),
        ParallelThresholdTask(image, binary, 0, left_end, params.left_thresh));
  }
  if (mid_end > left_end) {
    cv::parallel_for_(cv::Range(0, rows),
                      ParallelThresholdTask(image, binary, left_end, mid_end,
                                            params.mid_thresh));
  }
  if (width > mid_end) {
    cv::parallel_for_(cv::Range(0, rows),
                      ParallelThresholdTask(image, binary, mid_end, width,
                                            params.right_thresh));
  }

  return binary;
}
// =======================================================

struct HoleInfo {
  int index;
  Point center;
  double pixel_diameter;
  int area;
  bool merged = false;
  int merged_count = 1;
  double real_diameter = -1.0;
  int width;                  // 添加宽度属性
  int height;                 // 添加高度属性
  double real_area = -1.0;    // 添加实际面积属性
  double real_width = -1.0;   // 添加实际宽度属性
  double real_height = -1.0;  // 添加实际高度属性
  int top_y;                  // 添加上边界Y坐标
  int bottom_y;               // 添加下边界Y坐标
};

static std::vector<HoleInfo> merge_close_holes(
    std::vector<HoleInfo>& holes, int distance_threshold,
    const HoleDetection::Config& config) noexcept {
  if (holes.size() <= 1) {
    return holes;
  }

  std::vector<bool> merged(holes.size(), false);
  std::vector<HoleInfo> merged_holes;

  for (size_t i = 0; i < holes.size(); ++i) {
    if (merged[i]) {
      continue;
    }

    std::vector<size_t> close_indices;
    for (size_t j = 0; j < holes.size(); ++j) {
      if (!merged[j] && euclidean_distance(holes[i].center, holes[j].center) <=
                            distance_threshold) {
        close_indices.push_back(j);
        merged[j] = true;
      }
    }

    if (close_indices.size() == 1) {
      merged_holes.push_back(holes[close_indices[0]]);
    } else {
      double total_area = 0;
      double total_cx = 0, total_cy = 0;
      // 计算边界框
      int min_x = INT_MAX, min_y = INT_MAX;
      int max_x = INT_MIN, max_y = INT_MIN;

      for (size_t idx : close_indices) {
        total_area += holes[idx].area;
        total_cx += holes[idx].center.x * holes[idx].area;
        total_cy += holes[idx].center.y * holes[idx].area;

        // 更新边界框
        min_x = std::min(min_x, holes[idx].center.x - holes[idx].width / 2);
        min_y = std::min(min_y, holes[idx].center.y - holes[idx].height / 2);
        max_x = std::max(max_x, holes[idx].center.x + holes[idx].width / 2);
        max_y = std::max(max_y, holes[idx].center.y + holes[idx].height / 2);
      }
      int avg_cx = static_cast<int>(std::round(total_cx / total_area));
      int avg_cy = static_cast<int>(std::round(total_cy / total_area));
      double equiv_diam = 2.0 * std::sqrt(total_area / M_PI);

      // 计算合并后的宽度和高度
      int merged_width = max_x - min_x;
      int merged_height = max_y - min_y;

      HoleInfo merged_hole;
      merged_hole.index = static_cast<int>(merged_holes.size()) + 1;
      merged_hole.center = Point(avg_cx, avg_cy);
      merged_hole.pixel_diameter = equiv_diam;
      merged_hole.area = static_cast<int>(total_area);
      merged_hole.width = merged_width;    // 设置合并后的宽度
      merged_hole.height = merged_height;  // 设置合并后的高度
      merged_hole.merged = true;
      merged_hole.merged_count = static_cast<int>(close_indices.size());
      merged_hole.top_y = min_y;     // 设置上边界Y坐标
      merged_hole.bottom_y = max_y;  // 设置下边界Y坐标

      // 计算实际尺寸
      if (config.enable_real_world_calculation) {
        merged_hole.real_width = merged_width * config.pixel_to_mm_width;
        merged_hole.real_height = merged_height * config.pixel_to_mm_height;
        merged_hole.real_area =
            total_area * config.pixel_to_mm_width * config.pixel_to_mm_height;
        for (size_t idx : close_indices) {
          if (holes[idx].real_diameter > 0) {
            merged_hole.real_diameter =
                holes[idx].real_diameter * (total_area / holes[idx].area);
            break;
          }
        }
      }

      merged_holes.push_back(merged_hole);
    }
  }

  for (size_t i = 0; i < merged_holes.size(); ++i) {
    merged_holes[i].index = static_cast<int>(i + 1);
  }
  return merged_holes;
}

// Preprocess image for hole detection
static Mat preprocess_for_hole_detection(const Mat& processed_image) noexcept {
  HOLE_DETECTION_TIMING_START(prep);
  Mat image = preprocess_image_fast(processed_image);
  HOLE_DETECTION_TIMING_END(prep, "    Preprocessing:    ");
  return image;
}

// Apply threshold to image
static Mat threshold_image(const Mat& image, bool is_small_image,
                           const HoleDetection::Config& config,
                           const PartitionConfig& parsed_params) noexcept {
  // --- Adjust parameters for small images (like Python) ---
  PartitionConfig params = parsed_params;  // 使用解析后的参数
  if (is_small_image) {
    // 小图像使用统一的中间阈值
    params.left_thresh = params.mid_thresh;
    params.right_thresh = params.mid_thresh;
  }

  // --- Partitioned Threshold ---
  HOLE_DETECTION_TIMING_START(thresh);
  Mat binary = apply_partitioned_threshold_parallel(image, params);
  HOLE_DETECTION_TIMING_END(thresh, "    Thresholding:     ");
  return binary;
}

// Extract hole information from binary image
static std::vector<HoleInfo> extract_holes(
    const Mat& image, const Mat& binary, bool is_small_image,
    bool skip_edge_detection, const HoleDetection::Config& config) noexcept {
  // --- Connected Components ---
  HOLE_DETECTION_TIMING_START(cc);
  Mat labels, stats, centroids;
  int num_labels =
      connectedComponentsWithStats(binary, labels, stats, centroids, 8, CV_32S);
  HOLE_DETECTION_TIMING_END(cc, "    ConnectedComps:   ");

  // --- Adjust parameters for small images (like Python) ---
  int current_min_area = is_small_image ? 1 : config.min_defect_area;

  // --- Collect holes ---
  std::vector<HoleInfo> hole_data;
  int edge_count = 0;
  int height = image.rows;
  int width = image.cols;

  if (num_labels > 1) {
    hole_data.reserve(num_labels - 1);
  }

  const int* stats_ptr = stats.ptr<int>(0);
  const double* centroids_ptr = centroids.ptr<double>(0);
  int stats_cols = stats.cols;
  int centroids_cols = centroids.cols;

  for (int i = 1; i < num_labels; ++i) {
    int area = stats_ptr[i * stats_cols + CC_STAT_AREA];
    if (area < current_min_area) {
      continue;
    }

    int x = stats_ptr[i * stats_cols + CC_STAT_LEFT];
    int y = stats_ptr[i * stats_cols + CC_STAT_TOP];
    int w = stats_ptr[i * stats_cols + CC_STAT_WIDTH];
    int h = stats_ptr[i * stats_cols + CC_STAT_HEIGHT];

    // --- Edge filtering: only for large images ---
    bool near_edge = false;
    if (!skip_edge_detection) {
      if (x < config.edge_margin || y < config.edge_margin ||
          x + w > width - config.edge_margin ||
          y + h > height - config.edge_margin) {
        near_edge = true;
      }
    }
    // For small images: do NOT filter by edge (align with Python)

    if (near_edge) {
      edge_count++;
      continue;
    }

    // Get centroid (x, y) - note: centroids[i][0] = x, [1] = y
    double cx_d = centroids_ptr[i * centroids_cols + 0];
    double cy_d = centroids_ptr[i * centroids_cols + 1];
    int cx = static_cast<int>(cx_d + 0.5);  // Round properly
    int cy = static_cast<int>(cy_d + 0.5);

    double equiv_diam = 2.0 * std::sqrt(static_cast<double>(area) / M_PI);
    HoleInfo hole;
    hole.index = static_cast<int>(hole_data.size()) + 1;
    hole.center = Point(cx, cy);
    hole.pixel_diameter = equiv_diam;
    hole.area = area;
    hole.width = w;         // 设置宽度
    hole.height = h;        // 设置高度
    hole.top_y = y;         // 设置上边界Y坐标
    hole.bottom_y = y + h;  // 设置下边界Y坐标
    if (config.enable_real_world_calculation) {
      hole.real_diameter =
          equiv_diam * config.pixel_to_mm_width;  // 使用宽度转换因子计算直径
      hole.real_area = area * config.pixel_to_mm_width *
                       config.pixel_to_mm_height;  // 计算实际面积
      hole.real_width =
          w * config.pixel_to_mm_width;  // 使用固定转换因子计算实际宽度
      hole.real_height =
          h * config.pixel_to_mm_height;  // 使用固定转换因子计算实际高度
    }
    hole_data.emplace_back(hole);
  }

  return hole_data;
}

// Merge nearby holes
static std::vector<HoleInfo> merge_holes(
    std::vector<HoleInfo>& hole_data, bool is_small_image,
    const HoleDetection::Config& config) noexcept {
  int current_merge_distance =
      is_small_image ? 5 : config.merge_distance_threshold;

  // --- Merge close holes ---
  HOLE_DETECTION_TIMING_START(merge);
  auto merged_hole_data =
      merge_close_holes(hole_data, current_merge_distance, config);
  HOLE_DETECTION_TIMING_END(merge, "    Merging:          ");

  return merged_hole_data;
}

// Create visualization images
static std::pair<Mat, Mat> create_visualizations(
    const Mat& image, std::vector<HoleInfo>& merged_hole_data,
    const HoleDetection::Config& config) noexcept {
  // --- Visualization (with adaptive radius for small images) ---
  HOLE_DETECTION_TIMING_START(vis);
  Mat result_image;
  cvtColor(image, result_image, COLOR_GRAY2BGR);

  // 创建两个用于可视化的图像副本
  Mat contour_visualization = result_image.clone();  // 显示质心和轮廓
  Mat bbox_visualization = result_image.clone();     // 显示边界框

  int img_min_dim = std::min(image.rows, image.cols);
  double max_radius_ratio = 0.1;  // Default for very小 images
  if (img_min_dim >= 500) {
    max_radius_ratio = 0.2;
  } else if (img_min_dim >= 100) {
    max_radius_ratio = 0.15;
  }

  // 按Y坐标排序孔洞，以便计算纵向距离
  std::sort(merged_hole_data.begin(), merged_hole_data.end(),
            [](const HoleInfo& a, const HoleInfo& b) {
              return a.center.y < b.center.y;
            });

  for (size_t i = 0; i < merged_hole_data.size(); ++i) {
    const auto& hole = merged_hole_data[i];
    int calc_radius = static_cast<int>(hole.pixel_diameter / 2.0);
    int max_allowed_radius = static_cast<int>(img_min_dim * max_radius_ratio);
    int radius = std::max(1, std::min(calc_radius, max_allowed_radius));
    int border_thickness = std::min(2, std::max(1, radius / 4));

    // 在contour_visualization上绘制质心和轮廓（红色圆和白色边框）
    circle(contour_visualization, hole.center, radius, Scalar(0, 0, 255),
           -1);  // Solid red
    circle(contour_visualization, hole.center, radius, Scalar(255, 255, 255),
           border_thickness);  // White border

    // 在bbox_visualization上绘制边界框（蓝色矩形）
    rectangle(
        bbox_visualization,
        Point(hole.center.x - hole.width / 2, hole.center.y - hole.height / 2),
        Point(hole.center.x + hole.width / 2, hole.center.y + hole.height / 2),
        Scalar(255, 0, 0), 1);

    // 添加文本标注到两个图像上
    std::stringstream info_stream;
    info_stream << fixed << setprecision(1);
    if (config.enable_real_world_calculation && hole.real_width > 0 &&
        hole.real_height > 0 && hole.real_area > 0) {
      // 显示真实尺寸（毫米），使用您提供的转换因子
      info_stream << fixed << setprecision(4);
      info_stream << "W:" << hole.real_width << " H:" << hole.real_height
                  << " A:" << hole.real_area;
    } else {
      // 显示像素尺寸
      info_stream << fixed << setprecision(1);
      info_stream << "W:" << hole.width << " H:" << hole.height
                  << " A:" << hole.area;
    }

    // 获取文本大小，使用更小的字体
    int baseline = 0;
    Size text_size = getTextSize(info_stream.str(), FONT_HERSHEY_SIMPLEX, 0.3,
                                 1, &baseline);  // 更小的字体大小 (0.3)

    // 确定文本位置，确保在图像边界内
    int text_x = hole.center.x + radius + 3;  // 减小偏移量
    int text_y = hole.center.y - radius - 3;  // 减小偏移量

    // 检查右侧边界
    if (text_x + text_size.width > contour_visualization.cols - 3) {
      text_x = contour_visualization.cols - text_size.width - 3;
    }

    // 检查左侧边界
    if (text_x < 3) {
      text_x = 3;
    }

    // 检查底部边界
    if (text_y + text_size.height > contour_visualization.rows - 3) {
      text_y = contour_visualization.rows - text_size.height - 3;
    }

    // 检查顶部边界
    if (text_y < text_size.height + 3) {
      text_y = text_size.height + 3;
    }

    // 在contour_visualization上绘制文本背景和文字，使用更小的字体
    rectangle(
        contour_visualization, Point(text_x - 1, text_y - text_size.height - 1),
        Point(text_x + text_size.width + 1, text_y + 1), Scalar(0, 0, 0), -1);
    putText(contour_visualization, info_stream.str(), Point(text_x, text_y),
            FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 255),
            1);  // 更小的字体大小 (0.3)

    // 在bbox_visualization上绘制相同的文本背景和文字，使用更小的字体
    rectangle(
        bbox_visualization, Point(text_x - 1, text_y - text_size.height - 1),
        Point(text_x + text_size.width + 1, text_y + 1), Scalar(0, 0, 0), -1);
    putText(bbox_visualization, info_stream.str(), Point(text_x, text_y),
            FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 255),
            1);  // 更小的字体大小 (0.3)
  }

  // 绘制纵向距离箭头（仅在有多个孔洞时）
  if (merged_hole_data.size() > 1) {
    for (size_t i = 0; i < merged_hole_data.size() - 1; ++i) {
      const auto& upper_hole = merged_hole_data[i];
      const auto& lower_hole = merged_hole_data[i + 1];

      // 计算纵向距离：上一个孔的下端到下一个孔的上端
      int vertical_distance = lower_hole.top_y - upper_hole.bottom_y;

      // 只有当孔洞在垂直方向上有一定程度的分离时才绘制箭头
      // 添加额外的检查，确保箭头不会过长
      if (vertical_distance > 0 && vertical_distance < image.rows) {
        // 检查横向距离，避免绘制过长的斜线
        int horizontal_distance =
            abs(lower_hole.center.x - upper_hole.center.x);
        if (horizontal_distance < image.cols / 3) {  // 限制横向距离
          Point arrow_start(upper_hole.center.x, upper_hole.bottom_y);
          Point arrow_end(lower_hole.center.x, lower_hole.top_y);

          // 在两个可视化图像上都绘制绿色箭头，增加线条粗细
          arrowedLine(contour_visualization, arrow_start, arrow_end,
                      Scalar(0, 255, 0), 2, LINE_AA, 0, 0.1);
          arrowedLine(bbox_visualization, arrow_start, arrow_end,
                      Scalar(0, 255, 0), 2, LINE_AA, 0, 0.1);

          // 在箭头旁边添加距离标注
          Point text_pos((arrow_start.x + arrow_end.x) / 2,
                         (arrow_start.y + arrow_end.y) / 2);

          std::stringstream dist_stream;
          if (config.enable_real_world_calculation) {
            double real_dist = vertical_distance * config.pixel_to_mm_height;
            dist_stream << fixed << setprecision(2) << real_dist << "mm";
          } else {
            dist_stream << vertical_distance << "px";
          }

          // 确保文本在图像边界内
          int baseline = 0;
          Size text_size = getTextSize(dist_stream.str(), FONT_HERSHEY_SIMPLEX,
                                       0.3, 1, &baseline);

          if (text_pos.x + text_size.width / 2 >
              contour_visualization.cols - 5) {
            text_pos.x = contour_visualization.cols - text_size.width / 2 - 5;
          } else if (text_pos.x - text_size.width / 2 < 5) {
            text_pos.x = text_size.width / 2 + 5;
          }

          if (text_pos.y + text_size.height / 2 >
              contour_visualization.rows - 5) {
            text_pos.y = contour_visualization.rows - text_size.height / 2 - 5;
          } else if (text_pos.y - text_size.height / 2 < 5) {
            text_pos.y = text_size.height / 2 + 5;
          }

          // 绘制文本背景和文字
          rectangle(contour_visualization,
                    Point(text_pos.x - text_size.width / 2 - 1,
                          text_pos.y - text_size.height / 2 - 1),
                    Point(text_pos.x + text_size.width / 2 + 1,
                          text_pos.y + text_size.height / 2 + 1),
                    Scalar(0, 0, 0), -1);
          putText(contour_visualization, dist_stream.str(),
                  Point(text_pos.x - text_size.width / 2,
                        text_pos.y + text_size.height / 2),
                  FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 255), 1);

          rectangle(bbox_visualization,
                    Point(text_pos.x - text_size.width / 2 - 1,
                          text_pos.y - text_size.height / 2 - 1),
                    Point(text_pos.x + text_size.width / 2 + 1,
                          text_pos.y + text_size.height / 2 + 1),
                    Scalar(0, 0, 0), -1);
          putText(bbox_visualization, dist_stream.str(),
                  Point(text_pos.x - text_size.width / 2,
                        text_pos.y + text_size.height / 2),
                  FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 255), 1);
        }
      }
    }
  }

  HOLE_DETECTION_TIMING_END(vis, "    Visualization:    ");

  return std::make_pair(contour_visualization, bbox_visualization);
}

// Save result images
static void save_results(const Mat& result_image,
                         const Mat& contour_visualization,
                         const Mat& bbox_visualization,
                         const std::string& image_path,
                         const std::string& output_dir) noexcept {
  // --- Save results ---
  fs::path img_path(image_path);
  std::string base_name = img_path.stem().string();
  std::string original_result_path =
      output_dir + "/processed_" + base_name + ".jpg";
  std::string contour_result_path =
      output_dir + "/contours_" + base_name + ".jpg";
  std::string bbox_result_path = output_dir + "/bbox_" + base_name + ".jpg";

  HOLE_DETECTION_TIMING_START(save);
  // 保存原始处理图像（无标注）
  imwrite(original_result_path, result_image);
  // 保存显示质心和轮廓的图像
  imwrite(contour_result_path, contour_visualization);
  // 保存显示边界框的图像
  imwrite(bbox_result_path, bbox_visualization);
  HOLE_DETECTION_TIMING_END(save, "    Image saving:     ");

  HOLE_DETECTION_LOG("  Results saved to: " << endl);
  HOLE_DETECTION_LOG("    Original:   " << original_result_path << endl);
  HOLE_DETECTION_LOG("    Contours:   " << contour_result_path << endl);
  HOLE_DETECTION_LOG("    Bounding boxes: " << bbox_result_path << endl);
}

// load from local directory for debug
static void process_single_image_impl(
    const Mat& processed_image, const std::string& image_path,
    const std::string& output_dir, const HoleDetection::Config& config,
    const PartitionConfig& parsed_params) noexcept {
  HOLE_DETECTION_TIMING_START(total);

  // --- Preprocessing ---
  Mat image = preprocess_for_hole_detection(processed_image);

  // --- Check image size ---
  bool is_small_image = (image.rows <= 100 && image.cols <= 100);
  bool skip_edge_detection = (image.rows < 1000 || image.cols < 1000);

  // --- Threshold ---
  Mat binary = threshold_image(image, is_small_image, config, parsed_params);

  // --- Extract holes ---
  auto hole_data =
      extract_holes(image, binary, is_small_image, skip_edge_detection, config);

  // --- Merge holes ---
  auto merged_hole_data = merge_holes(hole_data, is_small_image, config);

  // 如果是视频帧处理，则不需要保存结果图像
  if (!image_path.empty() && !output_dir.empty()) {
    // --- Create visualizations ---
    auto [contour_visualization, bbox_visualization] =
        create_visualizations(image, merged_hole_data, config);

    // Create base result image
    Mat result_image;
    cvtColor(image, result_image, COLOR_GRAY2BGR);

    // --- Save results ---
    save_results(result_image, contour_visualization, bbox_visualization,
                 image_path, output_dir);

    // --- Timing & output ---
    HOLE_DETECTION_TIMING_END(total, "  Total time:         ");

    HOLE_DETECTION_LOG("Image: " << fs::path(image_path).filename().string()
                                 << endl);
    HOLE_DETECTION_LOG("  Timing breakdown:" << endl);
    HOLE_DETECTION_LOG("  Total time:         " << total_ms << " ms" << endl);
  } else {
    // 视频帧处理情况，仅输出基本统计信息
    HOLE_DETECTION_TIMING_ONLY(total);

    HOLE_DETECTION_LOG("Video frame processed: " << merged_hole_data.size()
                                                 << " holes detected" << endl);
    HOLE_DETECTION_LOG("  Processing time: " << total_ms << " ms" << endl);
  }
}

// 从文件路径加载图像并处理的接口
static void process_single_image(
    const std::string& image_path, const std::string& output_dir,
    const HoleDetection::Config& config,
    const PartitionConfig& parsed_params) noexcept {
  // --- Load image ---
  HOLE_DETECTION_TIMING_START(load);
  Mat image = imread(image_path, IMREAD_GRAYSCALE);
  HOLE_DETECTION_TIMING_END(load, "  Loading image:    ");

  if (image.empty()) {
    cerr << "Warning: Unable to load image '" << image_path << "'" << endl;
    return;
  }

  // 调用公共实现函数
  process_single_image_impl(image, image_path, output_dir, config,
                            parsed_params);
}

// 从Mat对象处理图像的接口（用于视频帧处理）
static void process_single_image(
    const Mat& frame, const HoleDetection::Config& config,
    const PartitionConfig& parsed_params) noexcept {
  // 对于视频帧，我们不需要文件路径和输出目录
  std::string dummy_path = "";
  std::string dummy_output_dir = "";
  process_single_image_impl(frame, dummy_path, dummy_output_dir, config,
                            parsed_params);
}

// 新增：从CapturedFrame处理图像的接口，这是process()函数实际调用的版本
static void process_single_image(
    const CapturedFrame& frame, const HoleDetection::Config& config,
    const PartitionConfig& parsed_params) noexcept {
  // 转换为Mat并调用通用实现
  std::string dummy_path = "";
  std::string dummy_output_dir = "";
  Mat image = CapturedFrame2Mat(frame);
  process_single_image_impl(image, dummy_path, dummy_output_dir, config,
                            parsed_params);
}

void HoleDetection::parse_partition_params() {
  std::stringstream ss(config_.partition_params);
  ss >> parsed_params_.left_ratio >> parsed_params_.mid_ratio >>
      parsed_params_.right_ratio >> parsed_params_.left_thresh >>
      parsed_params_.mid_thresh >> parsed_params_.right_thresh;
}

HoleDetection::HoleDetection() {
  config_.pixel_to_mm_height = 0.061;  // 修正默认值
  config_.partition_params = "0.3,0.4,0.3,20,23,20";
  parse_partition_params();  // 初始化时解析

  // 初始化配置映射表
  configMap_ = {
      {"pixel_per_mm",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.pixel_per_mm = std::stof(value);
       }},
      {"enable_real_world_calculation",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.enable_real_world_calculation = std::stoi(value) != 0;
       }},
      {"min_defect_area",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.min_defect_area = std::stoi(value);
       }},
      {"edge_margin",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.edge_margin = std::stoi(value);
       }},
      {"merge_distance_threshold",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.merge_distance_threshold = std::stoi(value);
       }},
      {"pixel_to_mm_width",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.pixel_to_mm_width = std::stof(value);
       }},
      {"pixel_to_mm_height",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.pixel_to_mm_height = std::stof(value);
       }},
      {"partition_params",
       [this](const std::string& value) {
         std::unique_lock lock(config_mutex_);
         config_.partition_params = value;
         parse_partition_params();
       }},
  };
}

HoleDetection::HoleDetection(const Config& cfg) : config_(cfg) {
  parse_partition_params();  // 初始化时解析
}

void HoleDetection::update_config(const Config& new_cfg) {
  std::unique_lock lock(config_mutex_);
  config_ = new_cfg;
  parse_partition_params();  // 热更新时重新解析
}

void HoleDetection::process(const CapturedFrame& frame) {
  if (frame.data.empty()) {
    cout << "Image is empty" << "with function" << __func__ << "in file"
         << __FILE__ << ",at line" << __LINE__ << std::endl;
    return;
  }

  // 获取配置的本地副本以保证线程安全
  Config local_config;
  PartitionConfig local_parsed_params;
  {
    std::shared_lock lock(config_mutex_);
    local_config = config_;
    local_parsed_params = parsed_params_;  // 使用解析后的结构体
  }

  double pixel_per_mm = local_config.enable_real_world_calculation
                            ? local_config.pixel_per_mm
                            : -1;
  if (pixel_per_mm > 0) {
    HOLE_DETECTION_LOG("Using calibration parameters: "
                       << fixed << setprecision(2) << pixel_per_mm
                       << " pixels/mm" << endl);
  }

  HOLE_DETECTION_TIMING_START(total);
  // 直接处理CapturedFrame，不再需要保存结果到文件
  process_single_image(CapturedFrame2Mat(frame), local_config,
                       local_parsed_params);

  HOLE_DETECTION_TIMING_END(total, "Total time: ");
}

std::vector<AlgoParamInfo> HoleDetection::get_parameter_info() const {
  // 获取配置的本地副本以保证线程安全
  Config local_config;
  {
    std::shared_lock lock(config_mutex_);
    local_config = config_;  // 返回原始字符串（用于 UI 显示和保存）
  }

  return {{"pixel_per_mm", "float", "像素/毫米转换因子", "50.0",
           std::to_string(local_config.pixel_per_mm)},
          {"enable_real_world_calculation", "bool", "是否启用真实尺寸计算", "1",
           local_config.enable_real_world_calculation ? "1" : "0"},
          {"min_defect_area", "int", "最小缺陷面积（像素）", "1",
           std::to_string(local_config.min_defect_area)},
          {"edge_margin", "int", "边缘过滤边距（像素）", "10",
           std::to_string(local_config.edge_margin)},
          {"merge_distance_threshold", "int", "孔洞合并距离阈值（像素）", "20",
           std::to_string(local_config.merge_distance_threshold)},
          {"pixel_to_mm_width", "float", "水平方向像素/毫米", "0.05586",
           std::to_string(local_config.pixel_to_mm_width)},
          {"pixel_to_mm_height", "float", "垂直方向像素/毫米", "0.061",
           std::to_string(local_config.pixel_to_mm_height)},
          {"partition_params", "string",
           "分区配置（left_ratio,mid_ratio,right_ratio,left_thresh,mid_thresh,"
           "right_thresh）",
           "0.3,0.4,0.3,20,23,20",
           local_config.partition_params}};  // 直接返回字符串
}

std::vector<AlgoSignalInfo> HoleDetection::get_signal_info() const {
  return {{"raw", "原始灰度图像"},
          {"preprocessed", "预处理后图像（裁剪+去噪）"},
          {"binary", "二值化结果（分区阈值）"},
          {"defect_map", "缺陷标注图（含合并孔洞）"}};
}
