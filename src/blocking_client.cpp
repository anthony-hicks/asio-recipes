#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <spdlog/spdlog.h>

#include <iostream>

using asio::ip::tcp;

int main(int argc, char* argv[]) {

  if (argc != 2) {
    std::cerr << std::format("Usage: {} <port>", argv[0]);
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

  // Connect to the server
  asio::connect(socket, endpoints, ec);

  if (ec) {
    spdlog::error("connect: {}", ec.message());
    return 1;
  }

  // Get input from the user
  std::array<char, 1024> buf{};

  std::cout << "Enter message: ";
  std::cin.getline(buf.data(), buf.size());

  std::string_view message(buf);

  // Send message to the server
  asio::write(socket, asio::buffer(message), ec);

  if (ec) {
    spdlog::error("write: {}", ec.message());
    return 1;
  }

  // Read the server's echo
  auto bytes_read = socket.read_some(asio::buffer(buf), ec);
  if (ec) {
    spdlog::error("read: {}", ec.message());
    return 1;
  }

  std::cout << "Reply is: ";
  std::cout.write(buf.data(), bytes_read);
  std::cout << '\n';

  return 0;
}

