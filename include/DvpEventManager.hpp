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
 *  - File: DvpEventManager.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

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
