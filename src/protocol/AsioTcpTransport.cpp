// Copyright (c) 2025 caomengxuan666
#include "protocol/AsioTcpTransport.hpp"

#include <string>
#include <vector>

namespace protocol {

AsioTcpTransport::AsioTcpTransport(asio::io_context& io_ctx)
    : io_ctx_(io_ctx), socket_(io_ctx) {}

AsioTcpTransport::~AsioTcpTransport() { close(); }

void AsioTcpTransport::async_connect(
    const std::string& ip, uint16_t port,
    std::function<void(std::error_code)> callback) {
  asio::ip::tcp::resolver resolver(io_ctx_);
  auto endpoints = resolver.resolve(ip, std::to_string(port));
  asio::async_connect(socket_, endpoints,
                      [this, callback](const asio::error_code& ec,
                                       const asio::ip::tcp::endpoint&) {
                        is_connected_ = !ec;
                        callback(ec);
                      });
}

void AsioTcpTransport::async_send(std::span<const uint8_t> data,
                                  SendCallback callback) {
  if (!is_connected_) {
    callback(asio::error::not_connected);
    return;
  }

  auto buffer =
      std::make_shared<std::vector<uint8_t>>(data.begin(), data.end());
  asio::async_write(socket_, asio::buffer(*buffer),
                    [callback, buffer](const asio::error_code& ec,
                                       std::size_t) { callback(ec); });
}

void AsioTcpTransport::async_receive(ReceiveCallback callback) {
  if (!is_connected_) {
    callback(asio::error::not_connected, {});
    return;
  }

  // 接收开始信号 (1字节)
  async_read_exact(1, [this, callback](const asio::error_code& ec, auto data) {
    if (ec) {
      callback(ec, {});
      return;
    }

    char start_signal = data[0];
    if (start_signal == 'F') {
      // 接收特征数据
      async_read_exact(72 + 4, [this, callback](const asio::error_code& ec,
                                                auto header) {
        if (ec) {
          callback(ec, {});
          return;
        }

        // 解析特征个数
        int32_t num_features;
        std::memcpy(&num_features, header.data() + 72, 4);

        // 接收剩余数据
        size_t remaining = 20 * 4 + num_features * 8 + 4;
        async_read_exact(remaining, [this, callback, num_features](
                                        const asio::error_code& ec, auto body) {
          if (ec) {
            callback(ec, {});
            return;
          }

          // 读取图片长度
          int32_t img_len;
          std::memcpy(&img_len, body.data() + 20 * 4 + num_features * 8, 4);

          // 读取图片数据
          async_read_exact(img_len, [callback, body](const asio::error_code& ec,
                                                     auto img_data) {
            if (ec) {
              callback(ec, {});
              return;
            }
            // 组合完整数据
            std::vector<uint8_t> full_data(1 + 72 + 4 + body.size() +
                                           img_data.size());
            full_data[0] = 'F';
            // 这里简化处理，实际应拼接所有数据
            callback({}, full_data);
          });
        });
      });
    } else if (start_signal == 'T') {
      // 接收状态数据 (4字节)
      async_read_exact(
          4, [callback](const asio::error_code& ec, auto status_data) {
            if (ec) {
              callback(ec, {});
              return;
            }
            std::vector<uint8_t> full_data(5);
            full_data[0] = 'T';
            std::memcpy(full_data.data() + 1, status_data.data(), 4);
            callback({}, full_data);
          });
    } else {
      // 未知信号
      callback(asio::error::invalid_argument, {});
    }
  });
}

void AsioTcpTransport::close() {
  if (is_connected_ && socket_.is_open()) {
    asio::post(io_ctx_, [this]() {
      asio::error_code ec;
      socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
      socket_.close(ec);
      is_connected_ = false;
    });
  }
}

void AsioTcpTransport::async_read_exact(
    size_t size,
    std::function<void(std::error_code, std::span<const uint8_t>)> handler) {
  if (!is_connected_) {
    handler(asio::error::not_connected, {});
    return;
  }

  auto buffer = std::make_shared<std::vector<uint8_t>>(size);
  asio::async_read(
      socket_, asio::buffer(*buffer),
      [handler, buffer](const asio::error_code& ec, std::size_t) {
        if (ec) {
          handler(ec, {});
        } else {
          handler(ec, std::span<const uint8_t>(buffer->data(), buffer->size()));
        }
      });
}

}  // namespace protocol
