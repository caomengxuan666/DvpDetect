#pragma once

#include <memory>
#include <shared_mutex>

#include "BS_thread_pool.hpp"
#include "DvpConfig.hpp"
#include "DvpEventManager.hpp"
#include "FrameProcessor.hpp"
#include "concurrentqueue.h"

class DvpCapture {
 public:
  explicit DvpCapture(dvpHandle handle);
  ~DvpCapture();

  bool start(const FrameProcessor& processor);
  void stop();

  // 动态配置（线程安全）
  void set_config(const DvpConfig& cfg);
  DvpConfig get_config() const;

  void register_event_handler(DvpEventType event, DvpEventHandler handler);
  void add_frame_processor(const FrameProcessor& processor);
  DvpEventManager* get_event_manager() const;

  // 获取图像队列的引用
  moodycamel::ConcurrentQueue<std::shared_ptr<CapturedFrame>>&
  get_frame_queue() {
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
  static int OnFrameCallback(dvpHandle, dvpStreamEvent, void*, dvpFrame*,
                             void*);
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
  FrameProcessor user_processor_;  // 用户自定义的帧处理器，可以是算法
  std::unique_ptr<DvpEventManager> event_manager_;
};
