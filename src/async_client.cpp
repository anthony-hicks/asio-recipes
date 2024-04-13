#include "spdlog/spdlog.h"
#include <asio/connect.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

#include <format>
#include <iostream>
#include <queue>
#include <thread>

using asio::ip::tcp;

using namespace std::chrono_literals;

class Client {
  asio::io_context& _io_context;
  tcp::socket _socket;

  std::array<char, 1024> read_buf_;

  // NOTE: You MUST use a message queue to manage lifetimes
  // properly. If you do not, the asio::post handler owns the
  // message's lifetime and will destruct the message before
  // the async_write actually fires.
  std::queue<std::string> message_queue_;

public:
  Client(
    asio::io_context& io_context, const tcp::resolver::results_type& endpoints) :
    _io_context(io_context),
    _socket(io_context)
  {
    async_connect(endpoints);
  }

  void async_write(const std::string& message)
  {
    asio::post(_io_context, [this, message]() {
      message_queue_.push(message);

      asio::async_write(
        _socket,
        asio::buffer(message_queue_.front()),
        [this](std::error_code ec, std::size_t bytes_written) {
          if (ec) {
            spdlog::error("write: {}", ec.message());
            _socket.close();
            return;
          }

          message_queue_.pop();
          async_read(bytes_written);
        });
    });
  }

private:
  void async_connect(const tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(
      _socket, endpoints, [this](std::error_code ec, const tcp::endpoint& endpoint) {
        if (ec) {
          spdlog::error("async_connect: {}", ec.message());
          return;
        }

        spdlog::info(
          "Connected to server {}:{}",
          endpoint.address().to_string(),
          endpoint.port());
      });
  }

  void async_read(std::size_t size)
  {
    asio::async_read(
      _socket,
      asio::buffer(read_buf_, size),
      [this](std::error_code ec, std::size_t bytes_read) {
        if (ec) {
          spdlog::error("read: {}", ec.message());
          _socket.close();
          return;
        }

        spdlog::info("Reply is: {}", std::string_view(read_buf_.data(), bytes_read));
      });
  }
};

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << std::format("Usage: {} <port>\n", argv[0]);
    return 1;
  }

  spdlog::set_level(spdlog::level::debug);

  // Execution context
  asio::io_context io_context;

  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);

  std::error_code ec;
  auto endpoints = resolver.resolve("localhost", argv[1], ec);

  if (ec) {
    spdlog::error("resolve: {}", ec.message());
    return 1;
  }

  Client client(io_context, endpoints);

  std::jthread io_thread([&io_context]() {
    auto work = asio::make_work_guard(io_context);
    io_context.run();
  });

  // Get input from the user
  std::array<char, 1024> buf{};

  std::cout << "Enter message: ";
  std::string message;
  std::getline(std::cin, message);

  client.async_write(message);

  // If we immediately stop the io_context, the write never
  // occurs.
  std::this_thread::sleep_for(1ms);

  io_context.stop();

  return 0;
}
