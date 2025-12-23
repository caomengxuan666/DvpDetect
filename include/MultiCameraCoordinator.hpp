#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "FrameProcessor.hpp"

class CapturedFrame;

// MultiCameraCoordinator.hpp
class MultiCameraCoordinator {
 public:
  using FusionFunc =
      std::function<CapturedFrame(const std::vector<CapturedFrame>&)>;

  MultiCameraCoordinator(size_t num_cams, FusionFunc fuse_func)
      : frames_(num_cams),
        received_(num_cams, false),
        fuse_func_(std::move(fuse_func)) {}

  // 返回 FrameProcessor（实际是 FunctionFrameProcessor）
  FrameProcessor make_processor_for(size_t cam_index) {
    // 创建 lambda
    auto lambda = [this, cam_index](const CapturedFrame& frame) {
      this->on_frame(cam_index, frame);
    };
    // 用工具函数包装成 FrameProcessor
    return make_function_processor(std::move(lambda));
  }

  // 重载make_processor_for的括号函数
  FrameProcessor operator[](size_t cam_index) {
    return make_processor_for(cam_index);
  }

 private:
  void on_frame(size_t cam_index, const CapturedFrame& frame) {
    std::lock_guard lock(mutex_);
    frames_[cam_index] = frame;
    received_[cam_index] = true;

    if (std::all_of(received_.begin(), received_.end(),
                    [](bool b) { return b; })) {
      // 所有帧到齐，执行融合
      auto fused = fuse_func_(frames_);

      // 融合后做什么？例如：
      // - 交给算法处理
      // - 发送到 ImageSignalBus
      // - 入队供 GUI 显示

      // 处理完后，最后还是要交由通用算法处理
      if (downstream_processor_) {
        downstream_processor_->process(fused);
      }

      // 重置状态（或加超时机制）
      std::fill(received_.begin(), received_.end(), false);
    }
  }

  std::vector<CapturedFrame> frames_;
  std::vector<bool> received_;
  FusionFunc fuse_func_;
  std::unique_ptr<FrameProcessor> downstream_processor_;
  std::mutex mutex_;

 public:
  void set_downstream_processor(std::unique_ptr<FrameProcessor> proc) {
    downstream_processor_ = std::move(proc);
  }
};
