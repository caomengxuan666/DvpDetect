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
 *  - File: DvpCapture.hpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#pragma once

#include <memory>
#include <shared_mutex>

#include "BS_thread_pool.hpp"
#include "CameraCapture.hpp"
#include "DvpConfig.hpp"
#include "DvpEventManager.hpp"
#include "FrameProcessor.hpp"
#include "concurrentqueue.h"

class DvpCameraCapture : public CameraCapture {
 public:
  explicit DvpCameraCapture(dvpHandle handle);
  ~DvpCameraCapture() override;

  bool start(const FrameProcessor& processor) override;
  void stop() override;
  void set_config(const CameraConfig& cfg) override;

  // 动态配置（线程安全）
  virtual void set_config(const DvpConfig& cfg);
  virtual DvpConfig get_config() const;

  virtual void register_event_handler(DvpEventType event,
                                      DvpEventHandler handler);
  virtual void add_frame_processor(const FrameProcessor& processor);
  virtual DvpEventManager* get_event_manager() const;

  auto& get_frame_processor() const { return user_processor_; }

  // 获取图像队列的引用
  moodycamel::ConcurrentQueue<std::shared_ptr<CapturedFrame>>& get_frame_queue()
      override {
    return frame_queue_;
  }

#ifdef SAVE_RESULT_IMAGE_QUEUE
  // 获取结果图像队列的引用
  moodycamel::ConcurrentQueue<std::shared_ptr<CapturedFrame>>&
  get_result_queue() {
    return result_queue_;
  }
#endif

 private:
  static int OnFrameCallback([[maybe_unused]] dvpHandle, dvpStreamEvent, void*,
                             dvpFrame*, void*);
  void process_frame(const dvpFrame& frame, const void* buffer);
  void update_camera_params();  // 应用配置到 SDK

  dvpHandle handle_ = 0;
  std::atomic<bool> running_{false};
  std::shared_ptr<DvpConfig> config_;
  mutable std::shared_mutex config_mutex_;
  moodycamel::ConcurrentQueue<std::shared_ptr<CapturedFrame>> frame_queue_;

// 结果队列
#ifdef SAVE_RESULT_IMAGE_QUEUE
  moodycamel::ConcurrentQueue<std::shared_ptr<CapturedFrame>> result_queue_;
#endif

  BS::thread_pool<> thread_pool_{std::thread::hardware_concurrency()};
  FrameProcessor user_processor_;  // 用户自定义的帧处理器，目前最多只有一个
  std::unique_ptr<DvpEventManager> event_manager_;
};
