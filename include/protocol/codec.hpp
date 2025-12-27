// Copyright (c) 2025 caomengxuan666
#pragma once
#include <optional>
#include <span>

#include "messages.hpp"

namespace protocol {

/// @brief 编解码器接口（未来 gRPC 可直接替换实现）
class ICodec {
 public:
  virtual ~ICodec() = default;

  virtual std::vector<uint8_t> encode_config(const ServerConfig& config) = 0;
  virtual std::optional<ServerConfig> decode_config(
      std::span<const uint8_t> data) = 0;

  virtual std::vector<uint8_t> encode_features(const FeatureReport& report) = 0;
  virtual std::optional<FeatureReport> decode_features(
      std::span<const uint8_t> data) = 0;

  virtual std::vector<uint8_t> encode_status(const FrontendStatus& status) = 0;
  virtual std::optional<FrontendStatus> decode_status(
      std::span<const uint8_t> data) = 0;
};

}  // namespace protocol
