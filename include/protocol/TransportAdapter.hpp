// Copyright (c) 2025 caomengxuan666
#pragma once
#include <functional>
#include <memory>
#include <span>
#include <system_error>

#include "asio.hpp"

namespace protocol {

class ITransportAdapter {
 public:
  virtual ~ITransportAdapter() = default;

  using SendCallback = std::function<void(std::error_code)>;
  using ReceiveCallback =
      std::function<void(std::error_code, std::vector<uint8_t>)>;

  virtual void async_connect(const std::string& ip, uint16_t port,
                             std::function<void(std::error_code)> callback) = 0;
  virtual void async_send(std::span<const uint8_t> data,
                          SendCallback callback) = 0;
  virtual void async_receive(ReceiveCallback callback) = 0;
  virtual void close() = 0;
};

}  // namespace protocol
