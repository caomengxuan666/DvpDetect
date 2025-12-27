#pragma once

#include <memory>

#include "concurrentqueue.h"

class FrameProcessor;
struct CameraConfig;  // 声明CameraConfig结构
class CapturedFrame;

class CameraCapture {
 public:
  virtual ~CameraCapture() = default;
  virtual bool start() = 0;
  virtual bool start(const FrameProcessor&) = 0;
  virtual void stop() = 0;
  virtual void set_config(const CameraConfig&) = 0;
  virtual void set_roi(int x, int y, int width, int height) = 0;
  virtual moodycamel::ConcurrentQueue<std::shared_ptr<CapturedFrame>>&
  get_frame_queue() = 0;
};
