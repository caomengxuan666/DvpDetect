// include/DvpCameraBuilder.hpp
#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "DvpCapture.hpp"
#include "DvpConfig.hpp"

class DvpCameraBuilder {
 public:
  // 构造：通过 UserID 或 FriendlyName
  // 存在一个fallback机制：如果指定的ID或名称无法打开相机，则尝试使用枚举索引打开第一个可用相机
  static DvpCameraBuilder fromUserId(const std::string& id);
  static DvpCameraBuilder fromFriendlyName(const std::string& name);

  // === 链式配置 ===
  DvpCameraBuilder& roi(int x, int y, int w, int h);
  DvpCameraBuilder& exposure(double us);
  DvpCameraBuilder& gain(float gain);
  DvpCameraBuilder& hardwareIsp(bool enable = true);
  DvpCameraBuilder& targetFormat(dvpStreamFormat fmt);

  // === 自动曝光和增益配置 ===
  DvpCameraBuilder& autoExposure(bool enable);
  DvpCameraBuilder& autoGain(bool enable);
  DvpCameraBuilder& aeTargetBrightness(int brightness);

  // === 抗频闪配置 ===
  DvpCameraBuilder& antiFlickerMode(int mode);  // 0: 关闭, 1: 50Hz, 2: 60Hz

  // === 采集模式配置 ===
  DvpCameraBuilder& acquisitionMode(int mode);  // 0: 连续, 1: 单帧, 2: 多帧

  // === 曝光时间范围配置 ===
  DvpCameraBuilder& exposureRange(double min_us, double max_us);

  // === 增益范围配置 ===
  DvpCameraBuilder& gainRange(float min_gain, float max_gain);

  // === 图像增强参数配置 ===
  DvpCameraBuilder& contrast(int value);           // 对比度 (-100 到 100)
  DvpCameraBuilder& gamma(int value);              // 伽马值 (默认100，表示1.0)
  DvpCameraBuilder& saturation(int value);         // 饱和度 (0-200)
  DvpCameraBuilder& sharpness(int value);          // 锐度值
  DvpCameraBuilder& sharpnessEnable(bool enable);  // 锐度使能

  // === 图像处理参数配置 ===
  DvpCameraBuilder& inverseState(bool enable);          // 负片使能
  DvpCameraBuilder& flipHorizontalState(bool enable);   // 横向翻转使能
  DvpCameraBuilder& flipVerticalState(bool enable);     // 纵向翻转使能
  DvpCameraBuilder& rotateState(bool enable);           // 旋转使能
  DvpCameraBuilder& rotateOpposite(bool opposite);      // 旋转方向
  DvpCameraBuilder& blackLevel(float level);            // 黑电平
  DvpCameraBuilder& colorTemperature(int temperature);  // 色温
  DvpCameraBuilder& flatFieldState(bool enable);        // 平场校正使能
  DvpCameraBuilder& defectFixState(bool enable);        // 缺陷像素校正使能

  // === 新增的参数配置方法 ===
  DvpCameraBuilder& monoState(bool enable);              // 去色使能
  DvpCameraBuilder& aeRoi(int x, int y, int w, int h);   // 自动曝光ROI
  DvpCameraBuilder& awbRoi(int x, int y, int w, int h);  // 自动白平衡ROI
  DvpCameraBuilder& coolerState(bool enable);            // 制冷器使能
  DvpCameraBuilder& bufferQueueSize(int size);           // 缓存队列大小
  DvpCameraBuilder& streamFlowCtrlSel(bool sel);         // 流控制方案选择
  DvpCameraBuilder& linkTimeout(unsigned int timeout);   // 连接超时时间

  // === 白平衡相关参数配置 ===
  DvpCameraBuilder& awbOperation(int operation);  // 自动白平衡操作方式

  // === 触发相关参数配置 ===
  DvpCameraBuilder& triggerActivation(int activation);   // 触发激活方式
  DvpCameraBuilder& triggerCount(int count);             // 触发计数
  DvpCameraBuilder& triggerDebouncer(double debouncer);  // 触发消抖时间
  DvpCameraBuilder& strobeSource(int source);            // Strobe信号源
  DvpCameraBuilder& strobeDelay(double delay);           // Strobe信号延迟
  DvpCameraBuilder& strobeDuration(double duration);     // Strobe信号持续时间

  // === 线扫相机专用参数配置 ===
  DvpCameraBuilder& lineTrigEnable(bool enable);          // 线扫触发使能
  DvpCameraBuilder& lineTrigSource(int source);           // 线扫触发源
  DvpCameraBuilder& lineTrigFilter(double filter);        // 线扫触发过滤
  DvpCameraBuilder& lineTrigEdgeSel(int edge);            // 线扫触发边沿选择
  DvpCameraBuilder& lineTrigDelay(double delay);          // 线扫触发延时
  DvpCameraBuilder& lineTrigDebouncer(double debouncer);  // 线扫触发消抖

  // === 其他高级参数配置 ===
  DvpCameraBuilder& acquisitionFrameRate(double rate);        // 帧率控制
  DvpCameraBuilder& acquisitionFrameRateEnable(bool enable);  // 帧率控制使能
  DvpCameraBuilder& flatFieldEnable(bool enable);             // 平场使能

  // === 触发模式配置 ===
  DvpCameraBuilder& triggerMode(bool enable);
  DvpCameraBuilder& triggerSource(dvpTriggerSource src);
  DvpCameraBuilder& triggerDelay(double us);

  // === 回调注册（支持链式注册多个）===
  DvpCameraBuilder& onFrame(const FrameProcessor& proc);
  DvpCameraBuilder& onEvent(DvpEventType event, const DvpEventHandler& handler);

  // === 构建并启动 ===
  std::unique_ptr<DvpCapture> build();

 private:
  struct Config {
    std::string user_id;
    std::string friendly_name;
    bool use_user_id = true;

    // 相机参数
    std::optional<dvpRegion> roi;
    std::optional<double> exposure;
    std::optional<float> gain;
    std::optional<bool> hardware_isp;
    std::optional<dvpStreamFormat> target_format;

    // 自动曝光和增益
    std::optional<bool> auto_exposure;
    std::optional<bool> auto_gain;
    std::optional<int> ae_target_brightness;

    // 抗频闪
    std::optional<int> anti_flicker_mode;

    // 采集模式
    std::optional<int> acquisition_mode;

    // 曝光时间范围
    std::optional<double> exposure_min;
    std::optional<double> exposure_max;

    // 增益范围
    std::optional<float> gain_min;
    std::optional<float> gain_max;

    // 图像增强参数
    std::optional<int> contrast;
    std::optional<int> gamma;
    std::optional<int> saturation;
    std::optional<bool> sharpness_enable;
    std::optional<int> sharpness;

    // 图像处理参数
    std::optional<bool> inverse_state;
    std::optional<bool> flip_horizontal_state;
    std::optional<bool> flip_vertical_state;
    std::optional<bool> rotate_state;
    std::optional<bool> rotate_opposite;
    std::optional<float> black_level;
    std::optional<int> color_temperature;
    std::optional<bool> flat_field_state;
    std::optional<bool> defect_fix_state;

    // 新增参数
    std::optional<bool> mono_state;
    std::optional<dvpRegion> ae_roi;
    std::optional<dvpRegion> awb_roi;
    std::optional<bool> cooler_state;
    std::optional<int> buffer_queue_size;
    std::optional<bool> stream_flow_ctrl_sel;
    std::optional<unsigned int> link_timeout;

    // 白平衡相关参数
    std::optional<int> awb_operation;

    // 触发相关参数
    std::optional<int> trigger_activation;
    std::optional<int> trigger_count;
    std::optional<double> trigger_debouncer;
    std::optional<int> strobe_source;
    std::optional<double> strobe_delay;
    std::optional<double> strobe_duration;

    // 线扫相机专用参数
    std::optional<bool> line_trig_enable;
    std::optional<int> line_trig_source;
    std::optional<double> line_trig_filter;
    std::optional<int> line_trig_edge_sel;
    std::optional<double> line_trig_delay;
    std::optional<double> line_trig_debouncer;

    // 其他高级参数
    std::optional<double> acquisition_frame_rate;
    std::optional<bool> acquisition_frame_rate_enable;
    std::optional<bool> flat_field_enable;

    // 触发
    std::optional<bool> trigger_mode;
    std::optional<dvpTriggerSource> trigger_source;
    std::optional<double> trigger_delay;

    // 回调
    std::vector<FrameProcessor> frame_processors;
    std::unordered_map<DvpEventType, DvpEventHandler> event_handlers;
  };

  Config config_;
};
