/*
 *  This is the default license template.
 *
 *  File: DvpEventManager.cpp
 *  Author: Administrator
 *  Copyright (c) 2025 Administrator
 *
 *  To edit this license information: Press Ctrl+Shift+P and press 'Create new
 * License Template...'.
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
