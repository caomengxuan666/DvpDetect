#pragma once
#include <algorithm>
#include <functional>
#include <unordered_map>

#include "DVPCamera.h"

enum class DvpEventType {
  StreamStarted,
  StreamStopped,
  FrameLost,
  FrameTimeout,
  Reconnected,
  FrameStart,
  FrameEnd
};

struct DvpEventContext {
  dvpHandle handle;
  DvpEventType type;
  dvpInt32 param;
  dvpVariant* variant;
};

using DvpEventHandler = std::function<void(const DvpEventContext&)>;

class DvpEventManager {
 public:
  explicit DvpEventManager(dvpHandle handle);
  ~DvpEventManager();
  void register_handler(DvpEventType event, const DvpEventHandler& handler);
  // 其实大概率不需要，析构函数会调用API将其全部注销的
  [[maybe_unused]] void unregister_handler(DvpEventType event);

 private:
  static dvpInt32 callback(dvpHandle, dvpEvent, void*, dvpInt32, dvpVariant*);
  dvpHandle handle_;
  std::unordered_map<DvpEventType, DvpEventHandler> handlers_;
  const std::unordered_map<dvpEvent, DvpEventType> event_map_ = {
      {EVENT_STREAM_STARTRD, DvpEventType::StreamStarted},
      {EVENT_STREAM_STOPPED, DvpEventType::StreamStopped},
      {EVENT_FRAME_LOST, DvpEventType::FrameLost},
      {EVENT_FRAME_TIMEOUT, DvpEventType::FrameTimeout},
      {EVENT_RECONNECTED, DvpEventType::Reconnected},
      {EVENT_FRAME_START, DvpEventType::FrameStart},
      {EVENT_FRAME_END, DvpEventType::FrameEnd},
  };
};
