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
 *  - File: DvpMultiCameraManager.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "DvpMultiCameraManager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// 添加缺失的包含文件
#include "DvpCapture.hpp"
#include "DvpConfig.hpp"
#include "FrameProcessor.hpp"

DvpMultiCameraManager::DvpMultiCameraManager(QObject* parent)
    : QObject(parent) {}

DvpMultiCameraManager::~DvpMultiCameraManager() {}

bool DvpMultiCameraManager::addCamera(const QString& cameraId,
                                      std::unique_ptr<DvpCapture> capture) {
  if (m_cameras.find(cameraId) != m_cameras.end()) {
    return false;  // 相机已存在
  }

  // 使用make_unique创建CameraData对象并插入到map中
  auto cameraData = std::make_unique<CameraData>(std::move(capture));
  m_cameras.emplace(cameraId, std::move(cameraData));
  return true;
}

void DvpMultiCameraManager::removeCamera(const QString& cameraId) {
  m_cameras.erase(cameraId);
}

bool DvpMultiCameraManager::hasCamera(const QString& cameraId) const {
  return m_cameras.find(cameraId) != m_cameras.end();
}

DvpCapture* DvpMultiCameraManager::getCamera(const QString& cameraId) const {
  auto it = m_cameras.find(cameraId);
  if (it != m_cameras.end()) {
    return it->second->capture.get();
  }
  return nullptr;
}

QStringList DvpMultiCameraManager::getAllCameraIds() const {
  QStringList ids;
  for (const auto& pair : m_cameras) {
    ids.append(pair.first);
  }
  return ids;
}

void DvpMultiCameraManager::setCameraConfig(const QString& cameraId,
                                            const DvpConfig& config) {
  auto it = m_cameras.find(cameraId);
  if (it != m_cameras.end()) {
    it->second->config = config;
    it->second->capture->set_config(config);
  }
}

DvpConfig DvpMultiCameraManager::getCameraConfig(
    const QString& cameraId) const {
  auto it = m_cameras.find(cameraId);
  if (it != m_cameras.end()) {
    return it->second->config;
  }
  return DvpConfig{};
}

bool DvpMultiCameraManager::saveCameraConfig(const QString& cameraId,
                                             const QString& filePath) const {
  auto it = m_cameras.find(cameraId);
  if (it == m_cameras.end()) {
    return false;
  }

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  const DvpConfig& config = it->second->config;
  QJsonObject json;

  // 基本图像参数
  json["exposure_us"] = config.exposure_us;
  json["gain"] = config.gain;
  json["roi_x"] = config.roi_x;
  json["roi_y"] = config.roi_y;
  json["roi_w"] = config.roi_w;
  json["roi_h"] = config.roi_h;
  json["trigger_mode"] = config.trigger_mode;
  json["hardware_isp"] = config.hardware_isp;

  // 自动曝光相关
  json["auto_exposure"] = config.auto_exposure;
  json["auto_gain"] = config.auto_gain;
  json["ae_target_brightness"] = config.ae_target_brightness;

  // 抗频闪设置
  json["anti_flicker_mode"] = config.anti_flicker_mode;

  // 采集模式
  json["acquisition_mode"] = config.acquisition_mode;

  // 触发详细配置
  json["trigger_source"] = config.trigger_source;
  json["trigger_delay_us"] = config.trigger_delay_us;

  // 曝光参数范围
  json["exposure_min_us"] = config.exposure_min_us;
  json["exposure_max_us"] = config.exposure_max_us;

  // 增益范围
  json["gain_min"] = config.gain_min;
  json["gain_max"] = config.gain_max;

  // 图像增强参数
  json["contrast"] = config.contrast;
  json["gamma"] = config.gamma;
  json["saturation"] = config.saturation;
  json["sharpness_enable"] = config.sharpness_enable;
  json["sharpness"] = config.sharpness;

  // 图像处理参数
  json["inverse_state"] = config.inverse_state;
  json["flip_horizontal_state"] = config.flip_horizontal_state;
  json["flip_vertical_state"] = config.flip_vertical_state;
  json["rotate_state"] = config.rotate_state;
  json["rotate_opposite"] = config.rotate_opposite;
  json["black_level"] = config.black_level;
  json["color_temperature"] = config.color_temperature;
  json["flat_field_state"] = config.flat_field_state;
  json["defect_fix_state"] = config.defect_fix_state;

  // 新增参数
  json["mono_state"] = config.mono_state;
  json["ae_roi_x"] = config.ae_roi_x;
  json["ae_roi_y"] = config.ae_roi_y;
  json["ae_roi_w"] = config.ae_roi_w;
  json["ae_roi_h"] = config.ae_roi_h;
  json["awb_roi_x"] = config.awb_roi_x;
  json["awb_roi_y"] = config.awb_roi_y;
  json["awb_roi_w"] = config.awb_roi_w;
  json["awb_roi_h"] = config.awb_roi_h;
  json["cooler_state"] = config.cooler_state;
  json["buffer_queue_size"] = config.buffer_queue_size;
  json["stream_flow_ctrl_sel"] = config.stream_flow_ctrl_sel;
  json["link_timeout"] = static_cast<qint64>(config.link_timeout);

  // 白平衡相关参数
  json["awb_operation"] = config.awb_operation;

  // 触发相关参数
  json["trigger_activation"] = config.trigger_activation;
  json["trigger_count"] = config.trigger_count;
  json["trigger_debouncer"] = config.trigger_debouncer;
  json["strobe_source"] = config.strobe_source;
  json["strobe_delay"] = config.strobe_delay;
  json["strobe_duration"] = config.strobe_duration;

  // 线扫相机专用参数
  json["line_trig_enable"] = config.line_trig_enable;
  json["line_trig_source"] = config.line_trig_source;
  json["line_trig_filter"] = config.line_trig_filter;
  json["line_trig_edge_sel"] = config.line_trig_edge_sel;
  json["line_trig_delay"] = config.line_trig_delay;
  json["line_trig_debouncer"] = config.line_trig_debouncer;

  // 其他高级参数
  json["acquisition_frame_rate"] = config.acquisition_frame_rate;
  json["acquisition_frame_rate_enable"] = config.acquisition_frame_rate_enable;
  json["flat_field_enable"] = config.flat_field_enable;

  QJsonDocument doc(json);
  file.write(doc.toJson());
  file.close();

  // 更新配置文件路径
  it->second->configPath = filePath;

  return true;
}

bool DvpMultiCameraManager::loadCameraConfig(const QString& cameraId,
                                             const QString& filePath) {
  auto it = m_cameras.find(cameraId);
  if (it == m_cameras.end()) {
    return false;
  }

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QByteArray data = file.readAll();
  file.close();

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);
  if (error.error != QJsonParseError::NoError) {
    return false;
  }

  DvpConfig& config = it->second->config;
  QJsonObject json = doc.object();

  // 基本图像参数
  if (json.contains("exposure_us"))
    config.exposure_us = json["exposure_us"].toDouble();
  if (json.contains("gain"))
    config.gain = static_cast<float>(json["gain"].toDouble());
  if (json.contains("roi_x")) config.roi_x = json["roi_x"].toInt();
  if (json.contains("roi_y")) config.roi_y = json["roi_y"].toInt();
  if (json.contains("roi_w")) config.roi_w = json["roi_w"].toInt();
  if (json.contains("roi_h")) config.roi_h = json["roi_h"].toInt();
  if (json.contains("trigger_mode"))
    config.trigger_mode = json["trigger_mode"].toBool();
  if (json.contains("hardware_isp"))
    config.hardware_isp = json["hardware_isp"].toBool();

  // 自动曝光相关
  if (json.contains("auto_exposure"))
    config.auto_exposure = json["auto_exposure"].toBool();
  if (json.contains("auto_gain")) config.auto_gain = json["auto_gain"].toBool();
  if (json.contains("ae_target_brightness"))
    config.ae_target_brightness = json["ae_target_brightness"].toInt();

  // 抗频闪设置
  if (json.contains("anti_flicker_mode"))
    config.anti_flicker_mode = json["anti_flicker_mode"].toInt();

  // 采集模式
  if (json.contains("acquisition_mode"))
    config.acquisition_mode = json["acquisition_mode"].toInt();

  // 触发详细配置
  if (json.contains("trigger_source"))
    config.trigger_source = json["trigger_source"].toInt();
  if (json.contains("trigger_delay_us"))
    config.trigger_delay_us = json["trigger_delay_us"].toDouble();

  // 曝光参数范围
  if (json.contains("exposure_min_us"))
    config.exposure_min_us = json["exposure_min_us"].toDouble();
  if (json.contains("exposure_max_us"))
    config.exposure_max_us = json["exposure_max_us"].toDouble();

  // 增益范围
  if (json.contains("gain_min"))
    config.gain_min = static_cast<float>(json["gain_min"].toDouble());
  if (json.contains("gain_max"))
    config.gain_max = static_cast<float>(json["gain_max"].toDouble());

  // 图像增强参数
  if (json.contains("contrast")) config.contrast = json["contrast"].toInt();
  if (json.contains("gamma")) config.gamma = json["gamma"].toInt();
  if (json.contains("saturation"))
    config.saturation = json["saturation"].toInt();
  if (json.contains("sharpness_enable"))
    config.sharpness_enable = json["sharpness_enable"].toBool();
  if (json.contains("sharpness")) config.sharpness = json["sharpness"].toInt();

  // 图像处理参数
  if (json.contains("inverse_state"))
    config.inverse_state = json["inverse_state"].toBool();
  if (json.contains("flip_horizontal_state"))
    config.flip_horizontal_state = json["flip_horizontal_state"].toBool();
  if (json.contains("flip_vertical_state"))
    config.flip_vertical_state = json["flip_vertical_state"].toBool();
  if (json.contains("rotate_state"))
    config.rotate_state = json["rotate_state"].toBool();
  if (json.contains("rotate_opposite"))
    config.rotate_opposite = json["rotate_opposite"].toBool();
  if (json.contains("black_level"))
    config.black_level = static_cast<float>(json["black_level"].toDouble());
  if (json.contains("color_temperature"))
    config.color_temperature = json["color_temperature"].toInt();
  if (json.contains("flat_field_state"))
    config.flat_field_state = json["flat_field_state"].toBool();
  if (json.contains("defect_fix_state"))
    config.defect_fix_state = json["defect_fix_state"].toBool();

  // 新增参数
  if (json.contains("mono_state"))
    config.mono_state = json["mono_state"].toBool();
  if (json.contains("ae_roi_x")) config.ae_roi_x = json["ae_roi_x"].toInt();
  if (json.contains("ae_roi_y")) config.ae_roi_y = json["ae_roi_y"].toInt();
  if (json.contains("ae_roi_w")) config.ae_roi_w = json["ae_roi_w"].toInt();
  if (json.contains("ae_roi_h")) config.ae_roi_h = json["ae_roi_h"].toInt();
  if (json.contains("awb_roi_x")) config.awb_roi_x = json["awb_roi_x"].toInt();
  if (json.contains("awb_roi_y")) config.awb_roi_y = json["awb_roi_y"].toInt();
  if (json.contains("awb_roi_w")) config.awb_roi_w = json["awb_roi_w"].toInt();
  if (json.contains("awb_roi_h")) config.awb_roi_h = json["awb_roi_h"].toInt();
  if (json.contains("cooler_state"))
    config.cooler_state = json["cooler_state"].toBool();
  if (json.contains("buffer_queue_size"))
    config.buffer_queue_size = json["buffer_queue_size"].toInt();
  if (json.contains("stream_flow_ctrl_sel"))
    config.stream_flow_ctrl_sel = json["stream_flow_ctrl_sel"].toBool();
  if (json.contains("link_timeout"))
    config.link_timeout =
        static_cast<unsigned int>(json["link_timeout"].toInt());

  // 白平衡相关参数
  if (json.contains("awb_operation"))
    config.awb_operation = json["awb_operation"].toInt();

  // 触发相关参数
  if (json.contains("trigger_activation"))
    config.trigger_activation = json["trigger_activation"].toInt();
  if (json.contains("trigger_count"))
    config.trigger_count = json["trigger_count"].toInt();
  if (json.contains("trigger_debouncer"))
    config.trigger_debouncer = json["trigger_debouncer"].toDouble();
  if (json.contains("strobe_source"))
    config.strobe_source = json["strobe_source"].toInt();
  if (json.contains("strobe_delay"))
    config.strobe_delay = json["strobe_delay"].toDouble();
  if (json.contains("strobe_duration"))
    config.strobe_duration = json["strobe_duration"].toDouble();

  // 线扫相机专用参数
  if (json.contains("line_trig_enable"))
    config.line_trig_enable = json["line_trig_enable"].toBool();
  if (json.contains("line_trig_source"))
    config.line_trig_source = json["line_trig_source"].toInt();
  if (json.contains("line_trig_filter"))
    config.line_trig_filter = json["line_trig_filter"].toDouble();
  if (json.contains("line_trig_edge_sel"))
    config.line_trig_edge_sel = json["line_trig_edge_sel"].toInt();
  if (json.contains("line_trig_delay"))
    config.line_trig_delay = json["line_trig_delay"].toDouble();
  if (json.contains("line_trig_debouncer"))
    config.line_trig_debouncer = json["line_trig_debouncer"].toDouble();

  // 其他高级参数
  if (json.contains("acquisition_frame_rate"))
    config.acquisition_frame_rate = json["acquisition_frame_rate"].toDouble();
  if (json.contains("acquisition_frame_rate_enable"))
    config.acquisition_frame_rate_enable =
        json["acquisition_frame_rate_enable"].toBool();
  if (json.contains("flat_field_enable"))
    config.flat_field_enable = json["flat_field_enable"].toBool();

  // 应用配置到相机
  it->second->capture->set_config(config);

  // 更新配置文件路径
  it->second->configPath = filePath;

  return true;
}

bool DvpMultiCameraManager::startCamera(const QString& cameraId,
                                        const FrameProcessor& processor) {
  auto it = m_cameras.find(cameraId);
  if (it != m_cameras.end()) {
    return it->second->capture->start(processor);
  }
  return false;
}

void DvpMultiCameraManager::stopCamera(const QString& cameraId) {
  auto it = m_cameras.find(cameraId);
  if (it != m_cameras.end()) {
    it->second->capture->stop();
  }
}
