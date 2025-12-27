// Copyright (c) 2025 caomengxuan666
#pragma once
#include <functional>
#include <memory>

#include "TransportAdapter.hpp"
#include "codec.hpp"
#include "messages.hpp"

namespace protocol {

class ProtocolSession {
 public:
  using ConfigCallback = std::function<void(std::shared_ptr<ServerConfig>)>;
  using FeaturesCallback = std::function<void(std::shared_ptr<FeatureReport>)>;
  using StatusCallback = std::function<void(std::shared_ptr<FrontendStatus>)>;
  using SendCallback = std::function<void(std::error_code)>;

  ProtocolSession(std::unique_ptr<ICodec> codec,
                  std::unique_ptr<ITransportAdapter> transport);
  ~ProtocolSession();

  void async_connect(const std::string& ip, uint16_t port,
                     std::function<void(std::error_code)> callback);
  void async_receive_config(ConfigCallback callback);
  void async_receive_features(FeaturesCallback callback);
  void async_receive_status(StatusCallback callback);
  void async_send_features(const FeatureReport& report, SendCallback callback);
  void async_send_status(const FrontendStatus& status, SendCallback callback);
  void start_reconnect_timer(uint16_t interval_sec = 5);

 private:
  std::unique_ptr<ICodec> codec_;
  std::unique_ptr<ITransportAdapter> transport_;
  std::unique_ptr<asio::steady_timer> reconnect_timer_ = nullptr;
};

}  // namespace protocol
