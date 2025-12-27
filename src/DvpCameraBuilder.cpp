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
 *  - File: DvpCameraBuilder.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpCameraBuilder.hpp"

#include <DVPCamera.h>

#include <iostream>
#include <memory>
#include <string>

#include "DvpConfig.hpp"
#include "dvpParam.h"

// 静态工厂方法
DvpCameraBuilder DvpCameraBuilder::fromUserId(const std::string& id) {
  DvpCameraBuilder builder;
  builder.config_.user_id = id;
  builder.config_.use_user_id = true;
  return builder;
}

DvpCameraBuilder DvpCameraBuilder::fromFriendlyName(const std::string& name) {
  DvpCameraBuilder builder;
  builder.config_.friendly_name = name;
  builder.config_.use_user_id = false;
  return builder;
}

DvpConfig DvpCameraBuilder::toDvpConfig() const {
  DvpConfig cfg;

  // ROI
  if (config_.roi.has_value()) {
    cfg.roi_x = config_.roi->X;
    cfg.roi_y = config_.roi->Y;
    cfg.roi_w = config_.roi->W;
    cfg.roi_h = config_.roi->H;
  }

  // 曝光/增益
  if (config_.exposure.has_value()) cfg.exposure_us = *config_.exposure;
  if (config_.gain.has_value()) cfg.gain = *config_.gain;

  // 触发模式
  if (config_.trigger_mode.has_value())
    cfg.trigger_mode = *config_.trigger_mode;
  if (config_.hardware_isp.has_value())
    cfg.hardware_isp = *config_.hardware_isp;

  // 自动曝光
  if (config_.auto_exposure.has_value())
    cfg.auto_exposure = *config_.auto_exposure;
  if (config_.auto_gain.has_value()) cfg.auto_gain = *config_.auto_gain;
  if (config_.ae_target_brightness.has_value())
    cfg.ae_target_brightness = *config_.ae_target_brightness;

  // 抗频闪
  if (config_.anti_flicker_mode.has_value())
    cfg.anti_flicker_mode = *config_.anti_flicker_mode;

  // 采集模式
  if (config_.acquisition_mode.has_value())
    cfg.acquisition_mode = *config_.acquisition_mode;

  // 曝光范围
  if (config_.exposure_min.has_value())
    cfg.exposure_min_us = *config_.exposure_min;
  if (config_.exposure_max.has_value())
    cfg.exposure_max_us = *config_.exposure_max;

  // 增益范围
  if (config_.gain_min.has_value()) cfg.gain_min = *config_.gain_min;
  if (config_.gain_max.has_value()) cfg.gain_max = *config_.gain_max;

  // 图像增强
  if (config_.contrast.has_value()) cfg.contrast = *config_.contrast;
  if (config_.gamma.has_value()) cfg.gamma = *config_.gamma;
  if (config_.saturation.has_value()) cfg.saturation = *config_.saturation;
  if (config_.sharpness_enable.has_value())
    cfg.sharpness_enable = *config_.sharpness_enable;
  if (config_.sharpness.has_value()) cfg.sharpness = *config_.sharpness;

  // 图像处理
  if (config_.inverse_state.has_value())
    cfg.inverse_state = *config_.inverse_state;
  if (config_.flip_horizontal_state.has_value())
    cfg.flip_horizontal_state = *config_.flip_horizontal_state;
  if (config_.flip_vertical_state.has_value())
    cfg.flip_vertical_state = *config_.flip_vertical_state;
  if (config_.rotate_state.has_value())
    cfg.rotate_state = *config_.rotate_state;
  if (config_.rotate_opposite.has_value())
    cfg.rotate_opposite = *config_.rotate_opposite;
  if (config_.black_level.has_value()) cfg.black_level = *config_.black_level;
  if (config_.color_temperature.has_value())
    cfg.color_temperature = *config_.color_temperature;
  if (config_.flat_field_state.has_value())
    cfg.flat_field_state = *config_.flat_field_state;
  if (config_.defect_fix_state.has_value())
    cfg.defect_fix_state = *config_.defect_fix_state;

  // 新增参数
  if (config_.mono_state.has_value()) cfg.mono_state = *config_.mono_state;
  if (config_.ae_roi.has_value()) {
    cfg.ae_roi_x = config_.ae_roi->X;
    cfg.ae_roi_y = config_.ae_roi->Y;
    cfg.ae_roi_w = config_.ae_roi->W;
    cfg.ae_roi_h = config_.ae_roi->H;
  }
  if (config_.awb_roi.has_value()) {
    cfg.awb_roi_x = config_.awb_roi->X;
    cfg.awb_roi_y = config_.awb_roi->Y;
    cfg.awb_roi_w = config_.awb_roi->W;
    cfg.awb_roi_h = config_.awb_roi->H;
  }
  if (config_.cooler_state.has_value())
    cfg.cooler_state = *config_.cooler_state;
  if (config_.buffer_queue_size.has_value())
    cfg.buffer_queue_size = *config_.buffer_queue_size;
  if (config_.stream_flow_ctrl_sel.has_value())
    cfg.stream_flow_ctrl_sel = *config_.stream_flow_ctrl_sel;
  if (config_.link_timeout.has_value())
    cfg.link_timeout = *config_.link_timeout;

  // 白平衡
  if (config_.awb_operation.has_value())
    cfg.awb_operation = *config_.awb_operation;

  // 触发相关
  if (config_.trigger_activation.has_value())
    cfg.trigger_activation = *config_.trigger_activation;
  if (config_.trigger_count.has_value())
    cfg.trigger_count = *config_.trigger_count;
  if (config_.trigger_debouncer.has_value())
    cfg.trigger_debouncer = *config_.trigger_debouncer;
  if (config_.strobe_source.has_value())
    cfg.strobe_source = *config_.strobe_source;
  if (config_.strobe_delay.has_value())
    cfg.strobe_delay = *config_.strobe_delay;
  if (config_.strobe_duration.has_value())
    cfg.strobe_duration = *config_.strobe_duration;

  // 线扫相机
  if (config_.line_trig_enable.has_value())
    cfg.line_trig_enable = *config_.line_trig_enable;
  if (config_.line_trig_source.has_value())
    cfg.line_trig_source = *config_.line_trig_source;
  if (config_.line_trig_filter.has_value())
    cfg.line_trig_filter = *config_.line_trig_filter;
  if (config_.line_trig_edge_sel.has_value())
    cfg.line_trig_edge_sel = *config_.line_trig_edge_sel;
  if (config_.line_trig_delay.has_value())
    cfg.line_trig_delay = *config_.line_trig_delay;
  if (config_.line_trig_debouncer.has_value())
    cfg.line_trig_debouncer = *config_.line_trig_debouncer;

  // 高级参数
  if (config_.acquisition_frame_rate.has_value())
    cfg.acquisition_frame_rate = *config_.acquisition_frame_rate;
  if (config_.acquisition_frame_rate_enable.has_value())
    cfg.acquisition_frame_rate_enable = *config_.acquisition_frame_rate_enable;
  if (config_.flat_field_enable.has_value())
    cfg.flat_field_enable = *config_.flat_field_enable;

  // 触发源/延迟
  if (config_.trigger_source.has_value())
    cfg.trigger_source = *config_.trigger_source;
  if (config_.trigger_delay.has_value())
    cfg.trigger_delay_us = *config_.trigger_delay;

  return cfg;
}

// 链式配置方法
DvpCameraBuilder& DvpCameraBuilder::roi(int x, int y, int w, int h) {
  config_.roi = dvpRegion{x, y, w, h, {}};
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::exposure(double us) {
  config_.exposure = us;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::gain(float gain) {
  config_.gain = gain;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::hardwareIsp(bool enable) {
  config_.hardware_isp = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::targetFormat(dvpStreamFormat fmt) {
  config_.target_format = fmt;
  return *this;
}

// 新增的配置方法实现
DvpCameraBuilder& DvpCameraBuilder::autoExposure(bool enable) {
  config_.auto_exposure = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::autoGain(bool enable) {
  config_.auto_gain = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::aeTargetBrightness(int brightness) {
  config_.ae_target_brightness = brightness;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::antiFlickerMode(int mode) {
  config_.anti_flicker_mode = mode;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::acquisitionMode(int mode) {
  config_.acquisition_mode = mode;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::exposureRange(double min_us,
                                                  double max_us) {
  config_.exposure_min = min_us;
  config_.exposure_max = max_us;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::gainRange(float min_gain, float max_gain) {
  config_.gain_min = min_gain;
  config_.gain_max = max_gain;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::contrast(int value) {
  config_.contrast = value;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::gamma(int value) {
  config_.gamma = value;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::saturation(int value) {
  config_.saturation = value;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::sharpness(int value) {
  config_.sharpness = value;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::sharpnessEnable(bool enable) {
  config_.sharpness_enable = enable;
  return *this;
}

// 新增的图像处理参数配置方法
DvpCameraBuilder& DvpCameraBuilder::inverseState(bool enable) {
  config_.inverse_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::flipHorizontalState(bool enable) {
  config_.flip_horizontal_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::flipVerticalState(bool enable) {
  config_.flip_vertical_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::rotateState(bool enable) {
  config_.rotate_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::rotateOpposite(bool opposite) {
  config_.rotate_opposite = opposite;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::blackLevel(float level) {
  config_.black_level = level;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::colorTemperature(int temperature) {
  config_.color_temperature = temperature;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::flatFieldState(bool enable) {
  config_.flat_field_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::defectFixState(bool enable) {
  config_.defect_fix_state = enable;
  return *this;
}

// 新增的参数配置方法实现
DvpCameraBuilder& DvpCameraBuilder::monoState(bool enable) {
  config_.mono_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::aeRoi(int x, int y, int w, int h) {
  config_.ae_roi = dvpRegion{x, y, w, h, {}};
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::awbRoi(int x, int y, int w, int h) {
  config_.awb_roi = dvpRegion{x, y, w, h, {}};
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::coolerState(bool enable) {
  config_.cooler_state = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::bufferQueueSize(int size) {
  config_.buffer_queue_size = size;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::streamFlowCtrlSel(bool sel) {
  config_.stream_flow_ctrl_sel = sel;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::linkTimeout(unsigned int timeout) {
  config_.link_timeout = timeout;
  return *this;
}

// 白平衡相关参数配置方法实现
DvpCameraBuilder& DvpCameraBuilder::awbOperation(int operation) {
  config_.awb_operation = operation;
  return *this;
}

// 触发相关参数配置方法实现
DvpCameraBuilder& DvpCameraBuilder::triggerActivation(int activation) {
  config_.trigger_activation = activation;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::triggerCount(int count) {
  config_.trigger_count = count;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::triggerDebouncer(double debouncer) {
  config_.trigger_debouncer = debouncer;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::strobeSource(int source) {
  config_.strobe_source = source;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::strobeDelay(double delay) {
  config_.strobe_delay = delay;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::strobeDuration(double duration) {
  config_.strobe_duration = duration;
  return *this;
}

// 线扫相机专用参数配置方法实现
DvpCameraBuilder& DvpCameraBuilder::lineTrigEnable(bool enable) {
  config_.line_trig_enable = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::lineTrigSource(int source) {
  config_.line_trig_source = source;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::lineTrigFilter(double filter) {
  config_.line_trig_filter = filter;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::lineTrigEdgeSel(int edge) {
  config_.line_trig_edge_sel = edge;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::lineTrigDelay(double delay) {
  config_.line_trig_delay = delay;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::lineTrigDebouncer(double debouncer) {
  config_.line_trig_debouncer = debouncer;
  return *this;
}

// 其他高级参数配置方法实现
DvpCameraBuilder& DvpCameraBuilder::acquisitionFrameRate(double rate) {
  config_.acquisition_frame_rate = rate;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::acquisitionFrameRateEnable(bool enable) {
  config_.acquisition_frame_rate_enable = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::flatFieldEnable(bool enable) {
  config_.flat_field_enable = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::triggerMode(bool enable) {
  config_.trigger_mode = enable;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::triggerSource(dvpTriggerSource src) {
  config_.trigger_source = src;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::triggerDelay(double us) {
  config_.trigger_delay = us;
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::onFrame(const FrameProcessor& proc) {
  config_.frame_processors.push_back(proc);
  return *this;
}

DvpCameraBuilder& DvpCameraBuilder::onEvent(DvpEventType event,
                                            const DvpEventHandler& handler) {
  config_.event_handlers[event] = handler;
  return *this;
}

// 构建方法
std::unique_ptr<DvpCameraCapture> DvpCameraBuilder::build() {
  dvpHandle handle;
  dvpStatus status = DVP_STATUS_UNKNOW;

  // 打开相机
  // 优先使用 UserID，其次使用 FriendlyName
  if (config_.use_user_id) {
    status = dvpOpenByUserId(config_.user_id.c_str(), OPEN_NORMAL, &handle);
  } else if (!config_.friendly_name.empty()) {
    status = dvpOpenByName(config_.friendly_name.c_str(), OPEN_NORMAL, &handle);
  }

  // fallback
  if (status != DVP_STATUS_OK) {
    std::cerr << "Search the nearby cams\n";
    // trying to use the enum index
    dvpUint32 cnt = 0;
    dvpRefresh(&cnt);
    dvpCameraInfo info;
    dvpEnum(cnt, &info);
    status = dvpOpen(cnt, OPEN_NORMAL, &handle);
    if (status != DVP_STATUS_OK) {
      std::cerr << "Failed to open camera by index\n";
      return nullptr;
    }
  }

  // 应用配置
  if (config_.roi.has_value()) {
    dvpSetRoi(handle, config_.roi.value());
  }

  if (config_.exposure.has_value()) {
    dvpSetExposure(handle, config_.exposure.value());
  }

  if (config_.gain.has_value()) {
    dvpSetAnalogGain(handle, config_.gain.value());
  }

  if (config_.hardware_isp.has_value()) {
    dvpSetHardwareIspState(handle, config_.hardware_isp.value());
  }

  if (config_.target_format.has_value()) {
    dvpSetTargetFormat(handle, config_.target_format.value());
  }

  // 应用新增配置
  if (config_.auto_exposure.has_value()) {
    // 使用通用参数设置函数设置自动曝光
    dvpSetEnumValue(handle, V_EXPOSURE_AUTO_E,
                    config_.auto_exposure.value() ? 1 : 0);
  }

  if (config_.auto_gain.has_value()) {
    // 使用通用参数设置函数设置自动增益
    dvpSetEnumValue(handle, V_GAIN_AUTO_E, config_.auto_gain.value() ? 1 : 0);
  }

  if (config_.ae_target_brightness.has_value()) {
    dvpSetAeTarget(handle, config_.ae_target_brightness.value());
  }

  if (config_.anti_flicker_mode.has_value()) {
    dvpAntiFlick mode = ANTIFLICK_DISABLE;
    switch (config_.anti_flicker_mode.value()) {
      case 1:
        mode = ANTIFLICK_50HZ;
        break;
      case 2:
        mode = ANTIFLICK_60HZ;
        break;
      default:
        mode = ANTIFLICK_DISABLE;
        break;
    }
    dvpSetAntiFlick(handle, mode);
  }

  if (config_.exposure_min.has_value() && config_.exposure_max.has_value()) {
    // 注意：DVP SDK中没有直接设置曝光范围的API，这里只是保存配置值
    // 实际应用中可能需要通过其他方式实现
  }

  if (config_.gain_min.has_value() && config_.gain_max.has_value()) {
    // 注意：DVP SDK中没有直接设置增益范围的API，这里只是保存配置值
    // 实际应用中可能需要通过其他方式实现
  }

  if (config_.contrast.has_value()) {
    dvpSetContrast(handle, config_.contrast.value());
  }

  if (config_.gamma.has_value()) {
    dvpSetGamma(handle, config_.gamma.value());
  }

  if (config_.saturation.has_value()) {
    dvpSetSaturation(handle, config_.saturation.value());
  }

  if (config_.sharpness_enable.has_value()) {
    dvpSetSharpnessState(handle, config_.sharpness_enable.value());
  }

  if (config_.sharpness.has_value()) {
    dvpSetSharpness(handle, config_.sharpness.value());
  }

  // 应用新增的图像处理参数配置
  if (config_.inverse_state.has_value()) {
    dvpSetInverseState(handle, config_.inverse_state.value());
  }

  if (config_.flip_horizontal_state.has_value()) {
    dvpSetFlipHorizontalState(handle, config_.flip_horizontal_state.value());
  }

  if (config_.flip_vertical_state.has_value()) {
    dvpSetFlipVerticalState(handle, config_.flip_vertical_state.value());
  }

  if (config_.rotate_state.has_value()) {
    dvpSetRotateState(handle, config_.rotate_state.value());
  }

  if (config_.rotate_opposite.has_value()) {
    dvpSetRotateOpposite(handle, config_.rotate_opposite.value());
  }

  if (config_.black_level.has_value()) {
    dvpSetBlackLevel(handle, config_.black_level.value());
  }

  if (config_.color_temperature.has_value()) {
    dvpSetColorTemperature(handle, config_.color_temperature.value());
  }

  if (config_.flat_field_state.has_value()) {
    dvpSetFlatFieldState(handle, config_.flat_field_state.value());
  }

  if (config_.defect_fix_state.has_value()) {
    dvpSetDefectFixState(handle, config_.defect_fix_state.value());
  }

  // 应用新增参数配置
  if (config_.mono_state.has_value()) {
    dvpSetMonoState(handle, config_.mono_state.value());
  }

  if (config_.ae_roi.has_value()) {
    dvpSetAeRoi(handle, config_.ae_roi.value());
  }

  if (config_.awb_roi.has_value()) {
    dvpSetAwbRoi(handle, config_.awb_roi.value());
  }

  if (config_.cooler_state.has_value()) {
    dvpSetCoolerState(handle, config_.cooler_state.value());
  }

  if (config_.buffer_queue_size.has_value() &&
      config_.buffer_queue_size.value() > 0) {
    dvpSetBufferQueueSize(handle, config_.buffer_queue_size.value());
  }

  // 应用连接超时设置
  if (config_.link_timeout.has_value()) {
    dvpSetLinkTimeout(handle, config_.link_timeout.value());
  }

  // 应用流控制方案选择
  if (config_.stream_flow_ctrl_sel.has_value()) {
    dvpSetStreamFlowCtrlSel(handle,
                            config_.stream_flow_ctrl_sel.value() ? 1 : 0);
  }

  // 应用白平衡相关参数配置
  if (config_.awb_operation.has_value()) {
    dvpAwbOperation awbOp = AWB_OP_OFF;
    if (config_.awb_operation.value() == 1) {
      awbOp = AWB_OP_CONTINUOUS;
    }
    dvpSetAwbOperation(handle, awbOp);
  }

  // 应用触发相关参数配置
  if (config_.trigger_activation.has_value()) {
    dvpTriggerInputType trigInputType = TRIGGER_IN_OFF;
    switch (config_.trigger_activation.value()) {
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
    dvpSetTriggerInputType(handle, trigInputType);
  }

  if (config_.trigger_count.has_value()) {
    dvpSetFramesPerTrigger(handle, config_.trigger_count.value());
  }

  if (config_.trigger_debouncer.has_value()) {
    dvpSetTriggerJitterFilter(handle, config_.trigger_debouncer.value());
  }

  if (config_.strobe_source.has_value()) {
    dvpStrobeOutputType strobeOutputType = STROBE_OUT_OFF;
    switch (config_.strobe_source.value()) {
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
    dvpSetStrobeOutputType(handle, strobeOutputType);
  }

  if (config_.strobe_delay.has_value()) {
    dvpSetStrobeDelay(handle, config_.strobe_delay.value());
  }

  if (config_.strobe_duration.has_value()) {
    dvpSetStrobeDuration(handle, config_.strobe_duration.value());
  }

  // 应用线扫相机专用参数配置
  if (config_.line_trig_enable.has_value()) {
    dvpSetEnumValue(handle, V_LINE_TRIG_ENABLE_B,
                    config_.line_trig_enable.value() ? 1 : 0);
  }

  if (config_.line_trig_source.has_value()) {
    // 线扫触发源设置需要通过通用参数设置函数
    dvpSetEnumValue(handle, V_LINE_TRIG_SOURCE_E,
                    config_.line_trig_source.value());
  }

  if (config_.line_trig_filter.has_value()) {
    // 线扫触发过滤设置需要通过通用参数设置函数
    dvpSetFloatValue(handle, V_LINE_TRIG_FILTER_F,
                     config_.line_trig_filter.value());
  }

  if (config_.line_trig_edge_sel.has_value()) {
    // 线扫触发边沿选择设置需要通过通用参数设置函数
    dvpSetEnumValue(handle, V_LINE_TRIG_EDGE_SEL_E,
                    config_.line_trig_edge_sel.value());
  }

  if (config_.line_trig_delay.has_value()) {
    // 线扫触发延时设置需要通过通用参数设置函数
    dvpSetFloatValue(handle, V_LINE_TRIG_DELAY_F,
                     config_.line_trig_delay.value());
  }

  if (config_.line_trig_debouncer.has_value()) {
    // 线扫触发消抖设置需要通过通用参数设置函数
    dvpSetFloatValue(handle, V_LINE_TRIG_DEBOUNCER_F,
                     config_.line_trig_debouncer.value());
  }

  // 应用其他高级参数配置
  if (config_.acquisition_frame_rate.has_value()) {
    dvpSetFloatValue(handle, V_ACQ_FRAME_RATE_F,
                     config_.acquisition_frame_rate.value());
  }

  if (config_.acquisition_frame_rate_enable.has_value()) {
    dvpSetEnumValue(handle, V_ACQ_FRAME_RATE_ENABLE_B,
                    config_.acquisition_frame_rate_enable.value() ? 1 : 0);
  }

  if (config_.flat_field_enable.has_value()) {
    dvpSetEnumValue(handle, V_FLAT_FIELD_ENABLE_B,
                    config_.flat_field_enable.value() ? 1 : 0);
  }

  if (config_.trigger_mode.has_value()) {
    dvpSetTriggerState(handle, config_.trigger_mode.value());
  }

  if (config_.trigger_source.has_value()) {
    dvpSetTriggerSource(handle, config_.trigger_source.value());
  }

  if (config_.trigger_delay.has_value()) {
    dvpSetTriggerDelay(handle, config_.trigger_delay.value());
  }

  // 创建捕获对象
  auto capture = std::make_unique<DvpCameraCapture>(handle);

  // 将builder的内部配置文件转换成外部可读，并且传递给捕获对象
  capture->set_config(toDvpConfig());

  // 注册帧处理器（我们只使用最后一个）
  if (!config_.frame_processors.empty()) {
    capture->add_frame_processor(config_.frame_processors.back());
  }

  // 注册事件处理器
  for (const auto& [event, handler] : config_.event_handlers) {
    capture->register_event_handler(event, handler);
  }

  // 启动相机和算法,目前放到统一管理器中启动
  return capture;
}
