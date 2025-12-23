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
 *  - File: DvpCameraManager.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "DvpCapture.hpp"

class DvpCameraManager {
 public:
  // 禁用默认构造（或允许空管理器）
  DvpCameraManager() {
    // 大概率就4个，预留一点空间,最多的情况是8个
    cameras_.reserve(4);
  }

  explicit DvpCameraManager(size_t number_of_cameras) {
    // 提前预留，可以防止临时扩容造成的迭代器失效
    // 虽然我真的不确定我以后会不会用到这里的迭代器
    cameras_.reserve(number_of_cameras);
  }

  ~DvpCameraManager() {
    stop_all();
    cameras_.clear();
  }

  // 添加相机：接收 builder，立即构建并存储 shared_ptr
  void add_camera(std::unique_ptr<DvpCapture> cam) {
    if (cam) {
      // 转为 shared_ptr 存储（因为需要共享给 GUI/算法）
      cameras_.emplace_back(std::move(cam));
    }
  }

  // 动态移除相机
  void remove_camera(size_t index) {
    if (index < cameras_.size()) {
      cameras_.erase(cameras_.begin() + static_cast<int>(index));
    }
  }
  // 获取相机（用于 GUI、算法等模块）
  std::shared_ptr<DvpCapture> get_camera(size_t index) {
    if (index < cameras_.size()) {
      return cameras_[index];
    }
    return nullptr;
  }

  size_t camera_count() const { return cameras_.size(); }

  // 启动所有相机
  void start_all() {
    for (auto& cam : cameras_) {
      // 我们这里直接启动，我们已经在builder中设置了他们的帧处理器
      if (!cam->start(cam->get_frame_processor())) {
        // 可选：记录失败，或抛异常
        std::cerr << "Failed to start camera\n";
      }
    }
  }

  void stop_all() {
    for (auto& cam : cameras_) {
      cam->stop();
    }
  }

 private:
  std::vector<std::shared_ptr<DvpCapture>> cameras_;
};
