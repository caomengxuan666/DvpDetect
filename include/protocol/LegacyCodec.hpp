// Copyright (c) 2025 caomengxuan666
#pragma once
#include "codec.hpp"

namespace protocol {

/// @brief 老协议编解码器（兼容所有现场）
class LegacyCodec : public ICodec {
 public:
  std::vector<uint8_t> encode_config(const ServerConfig& config) override;
  std::optional<ServerConfig> decode_config(
      std::span<const uint8_t> data) override;

  std::vector<uint8_t> encode_features(const FeatureReport& report) override;
  std::optional<FeatureReport> decode_features(
      std::span<const uint8_t> data) override;

  std::vector<uint8_t> encode_status(const FrontendStatus& status) override;
  std::optional<FrontendStatus> decode_status(
      std::span<const uint8_t> data) override;

 private:
  void trim_trailing_spaces(std::string& s);
  void parse_optional_fields(std::span<const uint8_t> data, size_t offset,
                             ServerConfig& config);
};

}  // namespace protocol
