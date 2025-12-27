// Copyright (c) 2025 caomengxuan666
#include "protocol/LegacyCodec.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

namespace protocol {

std::vector<uint8_t> LegacyCodec::encode_config(const ServerConfig& config) {
  std::vector<uint8_t> buffer;
  buffer.reserve(72 + 20 + 5 + 5 + 5 + 4 + 4 + 80 + 4 + 64 + 4 + 64 + 4);

  // 开始信号 'O'
  buffer.push_back('O');

  // 卷号 (72字节)
  std::string padded_roll = config.roll_id;
  padded_roll.resize(72, ' ');
  buffer.insert(buffer.end(), padded_roll.begin(), padded_roll.end());

  // 牌号 (20字节)
  std::string padded_brand = config.brand;
  padded_brand.resize(20, ' ');
  buffer.insert(buffer.end(), padded_brand.begin(), padded_brand.end());

  // 厚度 (5字节字符串)
  std::string thickness_str = config.thickness_str;
  thickness_str.resize(5, ' ');
  buffer.insert(buffer.end(), thickness_str.begin(), thickness_str.end());

  // 最小缺陷长度 (5字节)
  std::string min_len_str = config.min_defect_length_str;
  min_len_str.resize(5, ' ');
  buffer.insert(buffer.end(), min_len_str.begin(), min_len_str.end());

  // 最小缺陷面积 (5字节)
  std::string min_area_str = config.min_defect_area_str;
  min_area_str.resize(5, ' ');
  buffer.insert(buffer.end(), min_area_str.begin(), min_area_str.end());

  // 料头长度 (4字节 float)
  float head_length = config.head_length;
  buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&head_length),
                reinterpret_cast<uint8_t*>(&head_length) + 4);

  // 可选字段（按协议顺序）
  if (config.material_type) {
    int32_t mat_type = *config.material_type;
    buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&mat_type),
                  reinterpret_cast<uint8_t*>(&mat_type) + 4);

    if (config.segmentation_params) {
      buffer.insert(
          buffer.end(),
          reinterpret_cast<const uint8_t*>(config.segmentation_params->data()),
          reinterpret_cast<const uint8_t*>(config.segmentation_params->data()) +
              80);

      if (config.upper_surface_id) {
        int32_t id = *config.upper_surface_id;
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&id),
                      reinterpret_cast<uint8_t*>(&id) + 4);

        if (config.upper_large_params) {
          buffer.insert(buffer.end(),
                        reinterpret_cast<const uint8_t*>(
                            config.upper_large_params->data()),
                        reinterpret_cast<const uint8_t*>(
                            config.upper_large_params->data()) +
                            64);

          if (config.lower_surface_id) {
            int32_t id2 = *config.lower_surface_id;
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&id2),
                          reinterpret_cast<uint8_t*>(&id2) + 4);

            if (config.lower_large_params) {
              buffer.insert(buffer.end(),
                            reinterpret_cast<const uint8_t*>(
                                config.lower_large_params->data()),
                            reinterpret_cast<const uint8_t*>(
                                config.lower_large_params->data()) +
                                64);

              if (config.cutting_count) {
                int32_t cnt = *config.cutting_count;
                buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&cnt),
                              reinterpret_cast<uint8_t*>(&cnt) + 4);
              }
            }
          }
        }
      }
    }
  }

  return buffer;
}

std::optional<ServerConfig> LegacyCodec::decode_config(
    std::span<const uint8_t> data) {
  if (data.empty() || data[0] != 'O') {
    return std::nullopt;
  }

  ServerConfig config;
  size_t offset = 1;

  // 卷号 (72字节)
  if (offset + 72 > data.size()) return std::nullopt;
  config.roll_id =
      std::string(reinterpret_cast<const char*>(data.data() + offset), 72);
  trim_trailing_spaces(config.roll_id);
  offset += 72;

  // 牌号 (20字节)
  if (offset + 20 > data.size()) return std::nullopt;
  config.brand =
      std::string(reinterpret_cast<const char*>(data.data() + offset), 20);
  trim_trailing_spaces(config.brand);
  offset += 20;

  // 厚度 (5字节)
  if (offset + 5 > data.size()) return std::nullopt;
  config.thickness_str =
      std::string(reinterpret_cast<const char*>(data.data() + offset), 5);
  offset += 5;

  // 最小缺陷长度 (5字节)
  if (offset + 5 > data.size()) return std::nullopt;
  config.min_defect_length_str =
      std::string(reinterpret_cast<const char*>(data.data() + offset), 5);
  offset += 5;

  // 最小缺陷面积 (5字节)
  if (offset + 5 > data.size()) return std::nullopt;
  config.min_defect_area_str =
      std::string(reinterpret_cast<const char*>(data.data() + offset), 5);
  offset += 5;

  // 料头长度 (4字节)
  if (offset + 4 > data.size()) return std::nullopt;
  std::memcpy(&config.head_length, data.data() + offset, 4);
  offset += 4;

  // 尝试解析可选字段（不强制）
  parse_optional_fields(data, offset, config);

  return config;
}

void LegacyCodec::trim_trailing_spaces(std::string& s) {
  s.erase(s.find_last_not_of(' ') + 1);
}

void LegacyCodec::parse_optional_fields(std::span<const uint8_t> data,
                                        size_t offset, ServerConfig& config) {
  // 来料类型 (4字节)
  if (offset + 4 <= data.size()) {
    int32_t material;
    std::memcpy(&material, data.data() + offset, 4);
    config.material_type = material;
    offset += 4;

    // 分割定位参数 (80字节)
    if (offset + 80 <= data.size()) {
      std::array<float, 20> params;
      std::memcpy(params.data(), data.data() + offset, 80);
      config.segmentation_params = params;
      offset += 80;

      // 上表面编号 (4字节)
      if (offset + 4 <= data.size()) {
        int32_t id;
        std::memcpy(&id, data.data() + offset, 4);
        config.upper_surface_id = id;
        offset += 4;

        // 上表面大图参数 (64字节 = 16 floats)
        if (offset + 64 <= data.size()) {
          std::array<float, 16> large_params;
          std::memcpy(large_params.data(), data.data() + offset, 64);
          config.upper_large_params = large_params;
          offset += 64;

          // 下表面编号 (4字节)
          if (offset + 4 <= data.size()) {
            int32_t id2;
            std::memcpy(&id2, data.data() + offset, 4);
            config.lower_surface_id = id2;
            offset += 4;

            // 下表面大图参数 (64字节)
            if (offset + 64 <= data.size()) {
              std::array<float, 16> large_params2;
              std::memcpy(large_params2.data(), data.data() + offset, 64);
              config.lower_large_params = large_params2;
              offset += 64;

              // 分切方案条数 (4字节)
              if (offset + 4 <= data.size()) {
                int32_t cnt;
                std::memcpy(&cnt, data.data() + offset, 4);
                config.cutting_count = cnt;
                // 分切方案内容可按需解析
              }
            }
          }
        }
      }
    }
  }
}

std::vector<uint8_t> LegacyCodec::encode_features(const FeatureReport& report) {
  std::vector<uint8_t> buffer;
  buffer.reserve(1 + 72 + 4 + 20 * 4 + report.features.size() * 8 + 4 +
                 report.image_data.size());

  // 'F' 开始信号
  buffer.push_back('F');

  // 卷号 (72字节)
  std::string padded_roll = report.roll_id;
  padded_roll.resize(72, ' ');
  buffer.insert(buffer.end(), padded_roll.begin(), padded_roll.end());

  // 特征个数 (4字节)
  int32_t num_features = static_cast<int32_t>(report.features.size());
  buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&num_features),
                reinterpret_cast<uint8_t*>(&num_features) + 4);

  // 20个特殊图片 (20 * 4字节)
  for (float f : report.special_images) {
    buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&f),
                  reinterpret_cast<uint8_t*>(&f) + 4);
  }

  // 特征列表
  for (const auto& feat : report.features) {
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&feat.first),
                  reinterpret_cast<const uint8_t*>(&feat.first) + 4);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&feat.second),
                  reinterpret_cast<const uint8_t*>(&feat.second) + 4);
  }

  // 图片长度 + 数据
  int32_t img_len = static_cast<int32_t>(report.image_data.size());
  buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&img_len),
                reinterpret_cast<uint8_t*>(&img_len) + 4);
  buffer.insert(buffer.end(), report.image_data.begin(),
                report.image_data.end());

  return buffer;
}

std::optional<FeatureReport> LegacyCodec::decode_features(
    std::span<const uint8_t> data) {
  if (data.empty() || data[0] != 'F') {
    return std::nullopt;
  }

  FeatureReport report;
  size_t offset = 1;

  // 卷号 (72字节)
  if (offset + 72 > data.size()) return std::nullopt;
  report.roll_id =
      std::string(reinterpret_cast<const char*>(data.data() + offset), 72);
  report.roll_id.erase(report.roll_id.find_last_not_of(' ') + 1);
  offset += 72;

  // 特征个数 (4字节)
  if (offset + 4 > data.size()) return std::nullopt;
  int32_t num_features;
  std::memcpy(&num_features, data.data() + offset, 4);
  offset += 4;

  // 20个特殊图片 (80字节)
  if (offset + 80 > data.size()) return std::nullopt;
  std::memcpy(report.special_images.data(), data.data() + offset, 80);
  offset += 80;

  // 特征列表
  for (int i = 0; i < num_features; ++i) {
    if (offset + 8 > data.size()) return std::nullopt;
    int32_t index;
    float value;
    std::memcpy(&index, data.data() + offset, 4);
    std::memcpy(&value, data.data() + offset + 4, 4);
    report.features.emplace_back(index, value);
    offset += 8;
  }

  // 图片长度
  if (offset + 4 > data.size()) return std::nullopt;
  int32_t img_len;
  std::memcpy(&img_len, data.data() + offset, 4);
  offset += 4;

  // 图片数据
  if (offset + img_len > data.size()) return std::nullopt;
  report.image_data.assign(data.data() + offset,
                           data.data() + offset + img_len);

  return report;
}

std::vector<uint8_t> LegacyCodec::encode_status(const FrontendStatus& status) {
  std::vector<uint8_t> buffer;
  buffer.push_back('T');

  uint32_t status_int = status.to_uint32();
  buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&status_int),
                reinterpret_cast<uint8_t*>(&status_int) + 4);
  return buffer;
}

std::optional<FrontendStatus> LegacyCodec::decode_status(
    std::span<const uint8_t> data) {
  if (data.empty() || data[0] != 'T' || data.size() < 5) {
    return std::nullopt;
  }

  uint32_t status_int;
  std::memcpy(&status_int, data.data() + 1, 4);

  FrontendStatus status;
  status.self_check = (status_int & (1U << 1)) != 0;
  status.capture = (status_int & (1U << 2)) != 0;
  status.file_io = (status_int & (1U << 3)) != 0;
  status.image_anomaly = (status_int & (1U << 6)) != 0;

  return status;
}

}  // namespace protocol
