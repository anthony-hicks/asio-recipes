#include "spdlog/spdlog.h"
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>

#include <iostream>
#include <memory>
#include <queue>
#include <system_error>

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
  tcp::socket socket_;
  int32_t port_;
  std::array<char, 1024> read_buffer_{};
  std::queue<std::string> message_queue_;

public:
  explicit Session(tcp::socket socket, int32_t port) :
    socket_(std::move(socket)),
    port_(port)
  {}

  void start() { do_read(); }

private:
  void do_read()
  {
    auto self(shared_from_this());

    socket_.async_read_some(
      asio::buffer(read_buffer_),
      [this, self](std::error_code ec, std::size_t bytes_read) {
        if (ec == asio::error::eof) {
          spdlog::info("{}: connection closed by peer", port_);
          return;
        }

        if (ec) {
          spdlog::error("{}: async_read_some: {}", port_, ec.message());
          return;
        }

        spdlog::info(
          "{}: {}", port_, std::string_view(read_buffer_.data(), bytes_read));

        std::string message(read_buffer_.data(), bytes_read);
        message_queue_.push(message);

        do_write();
      });
  }

  void do_write()
  {
    auto self(shared_from_this());

    asio::async_write(
      socket_,
      asio::buffer(message_queue_.front()),
      [this, self](std::error_code ec, std::size_t bytes_written) {
        message_queue_.pop();

        if (ec) {
          spdlog::error("{}: write: {}", port_, ec.message());
          return;
        }

        do_read();
      });
  }
};

class Server {
  tcp::acceptor acceptor_;

public:
  Server(asio::io_context& io_context, const tcp::endpoint& endpoint) :
    acceptor_(io_context, endpoint)
  {
    spdlog::info("Server started on port {}", endpoint.port());
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
      do_accept();

      if (ec) {
        spdlog::error("accept: {}", ec.message());
        return;
      }

      auto remote_endpoint = socket.remote_endpoint(ec);

      if (ec) {
        spdlog::error("remote_endpoint: {}", ec.message());
        return;
      }

      auto port = remote_endpoint.port();

      spdlog::info("{}: accepted", port);

      std::make_shared<Session>(std::move(socket), port)->start();
    });
  }
};

int main(int argc, char* argv[])
{
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::trace);

  if (argc != 2) {
    std::cerr << std::format("usage: {} <port>", argv[0]);
    return 1;
  }

  auto port = std::atoi(argv[1]);

  // Server execution context
  asio::io_context io_context;

  tcp::endpoint endpoint(tcp::v4(), port);

  Server server(io_context, endpoint);

  // Run the event loop
  io_context.run();

  return 0;
}
