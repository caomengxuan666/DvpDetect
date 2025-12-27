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
 *  - File: DvpCapture.cpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpCameraCapture.hpp"

#include <DVPCamera.h>

#include <cstring>
#include <memory>

#include "FrameProcessor.hpp"
#include "config/CameraConfig.hpp"
#include "dvpParam.h"

DvpCameraCapture::DvpCameraCapture(dvpHandle handle) : handle_(handle) {
  if (handle_) {
    // 初始化配置
    config_ = std::make_shared<DvpConfig>();

    // 创建事件管理器
    event_manager_ = std::make_unique<DvpEventManager>(handle_);

    // 注册帧回调
    dvpRegisterStreamCallback(handle_, OnFrameCallback, STREAM_EVENT_PROCESSED,
                              this);
  }
}

DvpCameraCapture::~DvpCameraCapture() {
  if (handle_) {
    stop();
    dvpClose(handle_);
  }
}

bool DvpCameraCapture::start(const FrameProcessor& processor) {
  if (!handle_) {
    return false;
  }

  user_processor_ = processor;
  running_ = true;

  dvpStatus status = dvpStart(handle_);
  if (status != DVP_STATUS_OK) {
    running_ = false;
    return false;
  }

  return true;
}

bool DvpCameraCapture::start() {
  if (!handle_) {
    return false;
  }

  running_ = true;

  dvpStatus status = dvpStart(handle_);
  if (status != DVP_STATUS_OK) {
    running_ = false;
    return false;
  }

  return true;
}

void DvpCameraCapture::stop() {
  if (running_) {
    running_ = false;
    dvpStop(handle_);
  }
}

void DvpCameraCapture::set_config(const DvpConfig& cfg) {
  {
    std::unique_lock<std::shared_mutex> lock(config_mutex_);
    *config_ = cfg;
  }
  update_camera_params();
}

DvpConfig DvpCameraCapture::get_config() const {
  std::shared_lock<std::shared_mutex> lock(config_mutex_);
  return *config_;
}

void DvpCameraCapture::register_event_handler(DvpEventType event,
                                              DvpEventHandler handler) {
  event_manager_->register_handler(event, handler);
}
void DvpCameraCapture::add_frame_processor(const FrameProcessor& processor) {
  user_processor_ = processor;
}
DvpEventManager* DvpCameraCapture::get_event_manager() const {
  return event_manager_.get();
}

int DvpCameraCapture::OnFrameCallback([[maybe_unused]] dvpHandle handle,
                                      dvpStreamEvent event, void* context,
                                      dvpFrame* frame, void* buffer) {
  auto* capture = static_cast<DvpCameraCapture*>(context);
  if (capture && capture->running_) {
    capture->process_frame(*frame, buffer);
  }
  return 0;
}

void DvpCameraCapture::process_frame(const dvpFrame& frame,
                                     const void* buffer) {
  CapturedFrame captured_frame;
  captured_frame.meta = frame;
  captured_frame.data.assign(
      static_cast<const uint8_t*>(buffer),
      static_cast<const uint8_t*>(buffer) + frame.uBytes);

  // 在线程池中处理帧
  thread_pool_.detach_task([this, captured_frame]() {
    user_processor_.process(captured_frame);
#ifdef SAVE_RESULT_IMAGE_QUEUE
    result_queue_.enqueue(captured);
#endif
  });
  // 提交算法处理之后把原始图像放到队列里面
  auto captured = std::make_shared<CapturedFrame>(captured_frame);
  frame_queue_.enqueue(captured);
}

void DvpCameraCapture::update_camera_params() {
  // 应用配置到相机
  std::shared_lock<std::shared_mutex> lock(config_mutex_);

  if (config_->exposure_us > 0) {
    dvpSetExposure(handle_, config_->exposure_us);
  }

  if (config_->gain > 0) {
    dvpSetAnalogGain(handle_, config_->gain);
  }

  // ROI配置需要所有值都有效
  if (config_->roi_w > 0 && config_->roi_h > 0) {
    dvpRegion roi{
        config_->roi_x, config_->roi_y, config_->roi_w, config_->roi_h, {}};
    dvpSetRoi(handle_, roi);
  }

  dvpSetTriggerState(handle_, config_->trigger_mode);
  dvpSetHardwareIspState(handle_, config_->hardware_isp);

  // 应用新增的图像处理参数
  dvpSetInverseState(handle_, config_->inverse_state);
  dvpSetFlipHorizontalState(handle_, config_->flip_horizontal_state);
  dvpSetFlipVerticalState(handle_, config_->flip_vertical_state);
  dvpSetRotateState(handle_, config_->rotate_state);
  dvpSetRotateOpposite(handle_, config_->rotate_opposite);
  dvpSetBlackLevel(handle_, config_->black_level);
  dvpSetColorTemperature(handle_, config_->color_temperature);
  dvpSetFlatFieldState(handle_, config_->flat_field_state);
  dvpSetDefectFixState(handle_, config_->defect_fix_state);

  // 应用图像增强参数
  dvpSetContrast(handle_, config_->contrast);
  dvpSetGamma(handle_, config_->gamma);
  dvpSetSaturation(handle_, config_->saturation);
  dvpSetSharpnessState(handle_, config_->sharpness_enable);
  dvpSetSharpness(handle_, config_->sharpness);

  // 应用新增参数
  dvpSetMonoState(handle_, config_->mono_state);

  // 应用自动曝光ROI
  if (config_->ae_roi_w > 0 && config_->ae_roi_h > 0) {
    dvpRegion ae_roi{config_->ae_roi_x,
                     config_->ae_roi_y,
                     config_->ae_roi_w,
                     config_->ae_roi_h,
                     {}};
    dvpSetAeRoi(handle_, ae_roi);
  }

  // 应用自动白平衡ROI
  if (config_->awb_roi_w > 0 && config_->awb_roi_h > 0) {
    dvpRegion awb_roi{config_->awb_roi_x,
                      config_->awb_roi_y,
                      config_->awb_roi_w,
                      config_->awb_roi_h,
                      {}};
    dvpSetAwbRoi(handle_, awb_roi);
  }

  dvpSetCoolerState(handle_, config_->cooler_state);

  if (config_->buffer_queue_size > 0) {
    dvpSetBufferQueueSize(handle_, config_->buffer_queue_size);
  }

  dvpSetLinkTimeout(handle_, config_->link_timeout);

  // 应用白平衡相关参数
  dvpAwbOperation awbOp = AWB_OP_OFF;
  if (config_->awb_operation == 1) {
    awbOp = AWB_OP_CONTINUOUS;
  }
  dvpSetAwbOperation(handle_, awbOp);

  // 应用触发相关参数
  dvpTriggerInputType trigInputType = TRIGGER_IN_OFF;
  switch (config_->trigger_activation) {
    case 0:
      trigInputType = TRIGGER_POS_EDGE;  // 上升沿触发
      break;
    case 1:
      trigInputType = TRIGGER_NEG_EDGE;  // 下降沿触发
      break;
    case 2:
      trigInputType = TRIGGER_LOW_LEVEL;  // 低电平触发
      break;
    case 3:
      trigInputType = TRIGGER_HIGH_LEVEL;  // 高电平触发
      break;
    default:
      trigInputType = TRIGGER_IN_OFF;  // 关闭触发
      break;
  }
  dvpSetTriggerInputType(handle_, trigInputType);

  dvpSetFramesPerTrigger(handle_, config_->trigger_count);
  dvpSetTriggerJitterFilter(handle_, config_->trigger_debouncer);

  dvpStrobeOutputType strobeOutputType = STROBE_OUT_OFF;
  switch (config_->strobe_source) {
    case 0:
      strobeOutputType = STROBE_OUT_OFF;
      break;
    case 1:
      strobeOutputType = STROBE_OUT_LOW;
      break;
    case 2:
      strobeOutputType = STROBE_OUT_HIGH;
      break;
    default:
      strobeOutputType = STROBE_OUT_OFF;
      break;
  }
  dvpSetStrobeOutputType(handle_, strobeOutputType);

  dvpSetStrobeDelay(handle_, config_->strobe_delay);
  dvpSetStrobeDuration(handle_, config_->strobe_duration);

  // 应用线扫相机专用参数
  dvpSetEnumValue(handle_, V_LINE_TRIG_ENABLE_B,
                  config_->line_trig_enable ? 1 : 0);

  // 线扫触发源设置需要通过通用参数设置函数
  dvpSetEnumValue(handle_, V_LINE_TRIG_SOURCE_E, config_->line_trig_source);

  // 线扫触发过滤设置需要通过通用参数设置函数
  dvpSetFloatValue(handle_, V_LINE_TRIG_FILTER_F, config_->line_trig_filter);

  // 线扫触发边沿选择设置需要通过通用参数设置函数
  dvpSetEnumValue(handle_, V_LINE_TRIG_EDGE_SEL_E, config_->line_trig_edge_sel);

  // 线扫触发延时设置需要通过通用参数设置函数
  dvpSetFloatValue(handle_, V_LINE_TRIG_DELAY_F, config_->line_trig_delay);

  // 线扫触发消抖设置需要通过通用参数设置函数
  dvpSetFloatValue(handle_, V_LINE_TRIG_DEBOUNCER_F,
                   config_->line_trig_debouncer);

  // 应用其他高级参数
  dvpSetFloatValue(handle_, V_ACQ_FRAME_RATE_F,
                   config_->acquisition_frame_rate);

  dvpSetEnumValue(handle_, V_ACQ_FRAME_RATE_ENABLE_B,
                  config_->acquisition_frame_rate_enable ? 1 : 0);

  dvpSetEnumValue(handle_, V_FLAT_FIELD_ENABLE_B,
                  config_->flat_field_enable ? 1 : 0);
}

// TODO(cmx) 这个一定要重新写,只是因为现在还没确定好CameraConfig的通用范围。
void DvpCameraCapture::set_config(const CameraConfig& cfg) {
  // 将通用的CameraConfig转换为DvpConfig
  DvpConfig dvpCfg;

  // 基本图像参数
  dvpCfg.exposure_us = cfg.exposure_us;
  dvpCfg.gain = cfg.gain;
  dvpCfg.roi_x = cfg.roi_x;
  dvpCfg.roi_y = cfg.roi_y;
  dvpCfg.roi_w = cfg.roi_w;
  dvpCfg.roi_h = cfg.roi_h;

  // 模式设置
  dvpCfg.trigger_mode = cfg.trigger_mode;
  dvpCfg.hardware_isp = cfg.hardware_isp;

  // 自动调节参数
  dvpCfg.auto_exposure = cfg.auto_exposure;
  dvpCfg.auto_gain = cfg.auto_gain;
  dvpCfg.ae_target_brightness = cfg.ae_target_brightness;

  // 抗频闪设置
  dvpCfg.anti_flicker_mode = cfg.anti_flicker_mode;

  // 采集模式
  dvpCfg.acquisition_mode = cfg.acquisition_mode;

  // 图像增强参数
  dvpCfg.contrast = cfg.contrast;
  dvpCfg.gamma = cfg.gamma;
  dvpCfg.saturation = cfg.saturation;
  dvpCfg.sharpness_enable = cfg.sharpness_enable;
  dvpCfg.sharpness = cfg.sharpness;

  // 图像处理参数
  dvpCfg.inverse_state = cfg.inverse_state;
  dvpCfg.flip_horizontal_state = cfg.flip_horizontal_state;
  dvpCfg.flip_vertical_state = cfg.flip_vertical_state;
  dvpCfg.rotate_state = cfg.rotate_state;
  dvpCfg.rotate_opposite = cfg.rotate_opposite;
  dvpCfg.black_level = cfg.black_level;
  dvpCfg.color_temperature = cfg.color_temperature;
  dvpCfg.flat_field_state = cfg.flat_field_state;
  dvpCfg.defect_fix_state = cfg.defect_fix_state;

  // 其他高级参数
  dvpCfg.mono_state = cfg.mono_state;
  dvpCfg.ae_roi_x = cfg.ae_roi_x;
  dvpCfg.ae_roi_y = cfg.ae_roi_y;
  dvpCfg.ae_roi_w = cfg.ae_roi_w;
  dvpCfg.ae_roi_h = cfg.ae_roi_h;
  dvpCfg.awb_roi_x = cfg.awb_roi_x;
  dvpCfg.awb_roi_y = cfg.awb_roi_y;
  dvpCfg.awb_roi_w = cfg.awb_roi_w;
  dvpCfg.awb_roi_h = cfg.awb_roi_h;
  dvpCfg.cooler_state = cfg.cooler_state;
  dvpCfg.buffer_queue_size = cfg.buffer_queue_size;
  dvpCfg.stream_flow_ctrl_sel = cfg.stream_flow_ctrl_sel;
  dvpCfg.link_timeout = cfg.link_timeout;

  // 白平衡相关参数
  dvpCfg.awb_operation = cfg.awb_operation;

  // 触发相关参数
  dvpCfg.trigger_activation = cfg.trigger_activation;
  dvpCfg.trigger_count = cfg.trigger_count;
  dvpCfg.trigger_debouncer = cfg.trigger_debouncer;
  dvpCfg.strobe_source = cfg.strobe_source;
  dvpCfg.strobe_delay = cfg.strobe_delay;
  dvpCfg.strobe_duration = cfg.strobe_duration;

  // 线扫相机专用参数
  dvpCfg.line_trig_enable = cfg.line_trig_enable;
  dvpCfg.line_trig_source = cfg.line_trig_source;
  dvpCfg.line_trig_filter = cfg.line_trig_filter;
  dvpCfg.line_trig_edge_sel = cfg.line_trig_edge_sel;
  dvpCfg.line_trig_delay = cfg.line_trig_delay;
  dvpCfg.line_trig_debouncer = cfg.line_trig_debouncer;

  // 其他高级参数
  dvpCfg.acquisition_frame_rate = cfg.acquisition_frame_rate;
  dvpCfg.acquisition_frame_rate_enable = cfg.acquisition_frame_rate_enable;
  dvpCfg.flat_field_enable = cfg.flat_field_enable;

  // 使用转换后的DvpConfig设置相机
  set_config(dvpCfg);
}

void DvpCameraCapture::set_roi(int x, int y, int width, int height) {
  dvpRegion roi{x, y, width, height, {}};
  dvpSetRoi(handle_, roi);
}
