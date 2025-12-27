
#pragma once

#include <string>

// TODO 由于我还没有看具体的需求，先复制了一个DvpConfig,实际不会有这么多共同参数

// 通用相机配置结构
struct CameraConfig {
  // 基本图像参数
  double exposure_us = 10000.0;  // 曝光时间（微秒）
  float gain = 1.0f;             // 增益
  int roi_x = 0;                 // 感兴趣区域X坐标
  int roi_y = 0;                 // 感兴趣区域Y坐标
  int roi_w = 0;                 // 感兴趣区域宽度
  int roi_h = 0;                 // 感兴趣区域高度

  // 模式设置
  bool trigger_mode = false;  // 触发模式
  bool hardware_isp = true;   // 硬件ISP开关

  // 自动调节参数
  bool auto_exposure = false;      // 自动曝光
  bool auto_gain = false;          // 自动增益
  int ae_target_brightness = 128;  // AE目标亮度 (0-255)

  // 抗频闪设置
  int anti_flicker_mode = 0;  // 0: 关闭, 1: 50Hz, 2: 60Hz

  // 采集模式
  int acquisition_mode = 0;  // 0: 连续, 1: 单帧, 2: 多帧

  // 图像增强参数
  int contrast = 0;               // 对比度
  int gamma = 100;                // 伽马值
  int saturation = 100;           // 饱和度
  bool sharpness_enable = false;  // 锐度使能
  int sharpness = 0;              // 锐度值

  // 图像处理参数
  bool inverse_state = false;          // 反转状态
  bool flip_horizontal_state = false;  // 水平翻转
  bool flip_vertical_state = false;    // 垂直翻转
  bool rotate_state = false;           // 旋转状态
  bool rotate_opposite = false;        // 旋转反向
  float black_level = 0.0f;            // 黑电平
  int color_temperature = 6500;        // 色温
  bool flat_field_state = false;       // 平场校正状态
  bool defect_fix_state = false;       // 缺陷校正状态

  // 其他高级参数
  bool mono_state = false;           // 单色模式
  int ae_roi_x = 0;                  // AE ROI X坐标
  int ae_roi_y = 0;                  // AE ROI Y坐标
  int ae_roi_w = 0;                  // AE ROI 宽度
  int ae_roi_h = 0;                  // AE ROI 高度
  int awb_roi_x = 0;                 // AWB ROI X坐标
  int awb_roi_y = 0;                 // AWB ROI Y坐标
  int awb_roi_w = 0;                 // AWB ROI 宽度
  int awb_roi_h = 0;                 // AWB ROI 高度
  bool cooler_state = false;         // 制冷器状态
  int buffer_queue_size = 8;         // 缓存队列大小
  bool stream_flow_ctrl_sel = true;  // 流控制选择
  unsigned int link_timeout = 5000;  // 链接超时时间

  // 白平衡相关参数
  int awb_operation = 0;  // AWB操作模式

  // 触发相关参数
  int trigger_activation = 0;       // 触发激活方式
  int trigger_count = 0;            // 触发计数
  double trigger_debouncer = 0.0;   // 触发消抖时间
  int strobe_source = 0;            // 闪光源
  double strobe_delay = 0.0;        // 闪光延迟
  double strobe_duration = 1000.0;  // 闪光持续时间

  // 线扫相机专用参数
  bool line_trig_enable = false;     // 线触发使能
  int line_trig_source = 0;          // 线触发源
  double line_trig_filter = 0.0;     // 线触发滤波
  int line_trig_edge_sel = 0;        // 线触发边沿选择
  double line_trig_delay = 0.0;      // 线触发延迟
  double line_trig_debouncer = 0.0;  // 线触发消抖

  // 其他高级参数
  double acquisition_frame_rate = 0.0;         // 采集帧率
  bool acquisition_frame_rate_enable = false;  // 采集帧率使能
  bool flat_field_enable = false;              // 平场校正使能
};
