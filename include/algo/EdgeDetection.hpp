#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "AlgoBase.hpp"

namespace algo {

/**
 * @brief 边缘检测算法（示例）
 *
 * 使用仿函数模式，可直接作为 FrameProcessor 绑定到采集器。
 */
class EdgeDetection : public AlgoBase {
 public:
  EdgeDetection() {
    // 初始化配置映射表
    configMap_ = {{"lowThreshold",
                   [this](const std::string& value) {
                     lowThreshold_ = std::stoi(value);
                   }},
                  {"ratio", [this](const std::string& value) {
                     ratio_ = std::stof(value);
                   }}};
  }

  void process(const CapturedFrame& frame) override {
    // TODO: 替换为你的边缘检测实现
    // 示例：使用 OpenCV
    // cv::Mat img(frame.height, frame.width, CV_8UC3, frame.data);
    // cv::Canny(img, edges, lowThreshold_, lowThreshold_*ratio_, kernel_size);

    // 模拟处理耗时
    std::this_thread::sleep_for(std::chrono::milliseconds(8));

    // 打印结果
    static int count = 0;
    if (++count % 25 == 0) {
      std::cout << "[EdgeDetection] Processed frame - Width: " << frame.width()
                << ", Height: " << frame.height()
                << ", Format: " << frame.format()
                << ", Exposure: " << frame.exposure_us() << "us"
                << ", Timestamp: " << frame.timestamp_us() << "us\n";
    }
  }

 private:
  int lowThreshold_ = 50;  // 低阈值
  float ratio_ = 3.0f;     // 高低阈值比例
  int kernel_size = 3;     // 卷积核大小
};

}  // namespace algo
