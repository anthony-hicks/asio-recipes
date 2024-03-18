#include <asio.hpp>
#include <spdlog/spdlog.h>

#include <format>
#include <iostream>

using asio::ip::tcp;

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << std::format("Usage: {} <host> <port>", argv[0]);
  }

  spdlog::set_level(spdlog::level::debug);

  // Execution context
  asio::io_context io_context;

  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);

  // Connect to the server
  asio::error_code ec;
  asio::connect(socket, resolver.resolve(argv[1], argv[2]), ec);

  // TODO: Should retry? Or no because this should be the "simplest" recipe? 
  // Separate retry recipe? Should write up each specifically and what they should show
  if (ec) {
    spdlog::error("connect: {}", ec.message());
    return 1;
  }

  while (true) {

    // Get input from the user
    std::cout << "Enter message: ";

    std::array<char, 1024> buf;
    std::cin.getline(buf.data(), buf.size());

    std::span<char> message(buf.data(), std::strlen(buf.data()));

    // Send message to the server
    asio::write(socket, asio::buffer(message), ec);

    if (ec) {
      spdlog::error("write: {}", ec.message());
      return 1;
    }

    // Read echo from the server
    // NOTE: The use of `read_some` instead of `asio::read`. The server _should_ 
    // send us back a message of exactly the same length, but what if it doesn't?
    // We will sit here and block forever. It's probably more correct to stick 
    auto length = asio::read(socket, asio::buffer(message), ec);
    if (ec) {
      spdlog::error("read: {}", ec.message());
      return 1;
    }

    std::cout << "Reply is: ";
    std::cout.write(buf.data(), length);
    std::cout << '\n';
  }

  return 0;
}
