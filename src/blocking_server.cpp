#include <asio/write.hpp>
#include <cstddef>
#include <format>
#include <iostream>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <spdlog/spdlog.h>
#include <system_error>

using asio::ip::tcp;

void session(tcp::socket socket)
{
  std::error_code ec;
  auto remote_endpoint = socket.remote_endpoint(ec);

  if (ec) {
    spdlog::error("remote_endpoint: {}", ec.message());
    return;
  }

  auto port = remote_endpoint.port();

  spdlog::info("{}: accepted", port);

  std::array<char, 1024> buf{};

  while (true) {
    // Read message from the client
    std::size_t bytes_read = socket.read_some(asio::buffer(buf), ec);

    if (ec == asio::error::eof) {
      spdlog::info("{}: connection closed by peer", port);
      return;
    }

    if (ec) {
      spdlog::error("{}: read_some: {}", port, ec.message());
      return;
    }

    spdlog::info("{}: {}", port, std::string_view(buf.data(), bytes_read));

    // Echo the message back to the client
    asio::write(socket, asio::buffer(buf, bytes_read), ec);

    if (ec) {
      spdlog::error("{}: write: {}", port, ec.message());
      return;
    }
  }
}

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

  // NOTE: This constructor will throw an exception if there was an error binding
  // or listening on the port. For example, if you use a reserved port number. There
  // is no overload to use an error code.
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

  spdlog::info("Server started on port {}", port);

  while (true) {
    // [BLOCKING] Accept a new connection
    std::error_code ec;
    tcp::socket socket = acceptor.accept(ec);

    if (ec) {
      spdlog::error("accept: {}", ec.message());
      continue;
    }

    // Handle the client session on the main thread
    session(std::move(socket));
  }

  return 0;
}
