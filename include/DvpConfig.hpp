#pragma once
#include <memory>

struct DvpConfig {
  // 基本图像参数
  double exposure_us = 10000.0;
  float gain = 1.0f;
  int roi_x = 0, roi_y = 0, roi_w = 0, roi_h = 0;

  // 触发模式配置
  bool trigger_mode = false;
  bool hardware_isp = true;

  // 新增配置项
  // 自动曝光相关
  bool auto_exposure = false;      // 自动曝光开关
  bool auto_gain = false;          // 自动增益开关
  int ae_target_brightness = 128;  // 自动曝光目标亮度 (0-255)

  // 抗频闪设置
  int anti_flicker_mode = 0;  // 0: 关闭, 1: 50Hz, 2: 60Hz

  // 采集模式
  int acquisition_mode = 0;  // 0: 连续, 1: 单帧, 2: 多帧

  // 触发详细配置
  int trigger_source = 0;         // 0: 软触发, 1-8: 硬触发(Line1-Line8)
  double trigger_delay_us = 0.0;  // 触发延迟(微秒)

  // 曝光参数范围
  double exposure_min_us = 100.0;      // 最小曝光时间(微秒)
  double exposure_max_us = 1000000.0;  // 最大曝光时间(微秒)

  // 增益范围
  float gain_min = 1.0f;   // 最小增益
  float gain_max = 16.0f;  // 最大增益

  // 其他图像增强参数 (均为int类型，符合DVP SDK定义)
  int contrast = 0;               // 对比度 (-100 到 100)
  int gamma = 100;                // 伽马值 (默认100，表示1.0)
  int saturation = 100;           // 饱和度 (0-200)
  bool sharpness_enable = false;  // 锐度使能
  int sharpness = 0;              // 锐度值

  // 新增的图像处理参数
  bool inverse_state = false;          // 负片使能
  bool flip_horizontal_state = false;  // 横向翻转使能
  bool flip_vertical_state = false;    // 纵向翻转使能
  bool rotate_state = false;           // 旋转使能
  bool rotate_opposite = false;        // 旋转方向 (false=顺时针，true=逆时针)
  float black_level = 0.0f;            // 黑电平
  int color_temperature = 6500;        // 色温 (单位K，默认6500K)
  bool flat_field_state = false;       // 平场校正使能
  bool defect_fix_state = false;       // 缺陷像素校正使能

  // 新增参数
  bool mono_state = false;            // 去色使能
  int ae_roi_x = 0;                   // 自动曝光ROI x坐标
  int ae_roi_y = 0;                   // 自动曝光ROI y坐标
  int ae_roi_w = 0;                   // 自动曝光ROI宽度
  int ae_roi_h = 0;                   // 自动曝光ROI高度
  int awb_roi_x = 0;                  // 自动白平衡ROI x坐标
  int awb_roi_y = 0;                  // 自动白平衡ROI y坐标
  int awb_roi_w = 0;                  // 自动白平衡ROI宽度
  int awb_roi_h = 0;                  // 自动白平衡ROI高度
  bool cooler_state = false;          // 制冷器使能
  int buffer_queue_size = 0;          // 缓存队列大小 (0表示使用默认值)
  bool stream_flow_ctrl_sel = false;  // 流控制方案选择
  unsigned int link_timeout = 5000;   // 连接超时时间(毫秒)

  // 白平衡相关参数
  int awb_operation = 0;  // 自动白平衡操作方式 (0=手动, 1=自动)

  // 触发相关参数
  int trigger_activation = 0;      // 触发激活方式 (0=上升沿, 1=下降沿等)
  int trigger_count = 1;           // 触发计数
  double trigger_debouncer = 0.0;  // 触发消抖时间(微秒)
  int strobe_source = 0;           // Strobe信号源
  double strobe_delay = 0.0;       // Strobe信号延迟(微秒)
  double strobe_duration = 0.0;    // Strobe信号持续时间(微秒)

  // 线扫相机专用参数
  bool line_trig_enable = false;     // 线扫触发使能
  int line_trig_source = 0;          // 线扫触发源
  double line_trig_filter = 0.0;     // 线扫触发过滤
  int line_trig_edge_sel = 0;        // 线扫触发边沿选择
  double line_trig_delay = 0.0;      // 线扫触发延时(微秒)
  double line_trig_debouncer = 0.0;  // 线扫触发消抖(微秒)

  // 其他高级参数
  double acquisition_frame_rate = 0.0;         // 帧率控制 (0表示不限制)
  bool acquisition_frame_rate_enable = false;  // 帧率控制使能
  bool flat_field_enable = false;              // 平场使能
  DvpConfig clone() const { return *this; }
};
