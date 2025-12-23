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
 *  - File: ImageSignalBus.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

// ImageSignalBus.hpp
#pragma once
#include <functional>
#include <opencv2/core/mat.hpp>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>
class ImageSignalBus {
 public:
  using ImageCallback = std::function<void(const cv::Mat&)>;

  // 单例
  static ImageSignalBus& instance() {
    static ImageSignalBus bus;
    return bus;
  }

  // 算法调用：声明自己能提供哪些信号
  void declare_signal(const std::string& signal_name);

  // UI 或其他模块调用：订阅某个信号
  void subscribe(const std::string& signal_name, ImageCallback callback);

  // 算法内部调用：广播图像（自动深拷贝）
  void emit(const std::string& signal_name, const cv::Mat& img);

 private:
  ImageSignalBus() = default;
  std::unordered_map<std::string, std::vector<ImageCallback>> subscribers_;
  mutable std::shared_mutex mutex_;
};
