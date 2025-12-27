// Copyright (c) 2025 caomengxuan666
#pragma once
#include "TransportAdapter.hpp"

namespace protocol {

class AsioTcpTransport : public ITransportAdapter {
 public:
  explicit AsioTcpTransport(asio::io_context& io_ctx);
  ~AsioTcpTransport() override;

  void async_connect(const std::string& ip, uint16_t port,
                     std::function<void(std::error_code)> callback) override;
  void async_send(std::span<const uint8_t> data,
                  SendCallback callback) override;
  void async_receive(ReceiveCallback callback) override;
  void close() override;
  asio::io_context& get_io_context() { return io_ctx_; }

 private:
  asio::io_context& io_ctx_;
  asio::ip::tcp::socket socket_;
  asio::streambuf read_buffer_;
  bool is_connected_ = false;

  void async_read_exact(
      size_t size,
      std::function<void(std::error_code, std::span<const uint8_t>)> handler);
};

}  // namespace protocol
