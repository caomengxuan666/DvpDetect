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
 *  - File: DvpEventManager.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpEventManager.hpp"

DvpEventManager::DvpEventManager(dvpHandle handle) : handle_(handle) {
  // 构造函数初始化列表已经设置了 handle_
}

DvpEventManager::~DvpEventManager() {
  // 析构时取消注册所有事件处理器
  for (const auto& pair : event_map_) {
    dvpUnregisterEventCallback(handle_, callback, pair.first, this);
  }
}

void DvpEventManager::register_handler(DvpEventType event,
                                       const DvpEventHandler& handler) {
  // 查找对应的 SDK 事件类型
  auto it =
      std::find_if(event_map_.begin(), event_map_.end(),
                   [event](const auto& pair) { return pair.second == event; });

  if (it != event_map_.end()) {
    // 保存处理器
    handlers_[event] = handler;
    // 注册回调函数
    dvpRegisterEventCallback(handle_, callback, it->first, this);
  }
}

dvpInt32 DvpEventManager::callback(dvpHandle h, dvpEvent e, void* ctx,
                                   dvpInt32 p, dvpVariant* v) {
  auto* self = static_cast<DvpEventManager*>(ctx);
  auto event_it = self->event_map_.find(e);
  if (event_it != self->event_map_.end()) {
    DvpEventContext context{h, event_it->second, p, v};
    auto handler_it = self->handlers_.find(event_it->second);
    if (handler_it != self->handlers_.end()) {
      handler_it->second(context);
    }
  }
  return 0;
}

void DvpEventManager::unregister_handler(DvpEventType event) {
  // 查找对应的 SDK 事件类型
  auto it =
      std::find_if(event_map_.begin(), event_map_.end(),
                   [event](const auto& pair) { return pair.second == event; });

  if (it != event_map_.end()) {
    // 从处理器映射中删除
    handlers_.erase(event);
    // 取消注册回调函数
    dvpUnregisterEventCallback(handle_, callback, it->first, this);
  }
}
