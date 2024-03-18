#include <asio.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <format>
#include <iostream>
#include <vector>

// TODO: README.md per subdir
// describing each recipe.
//  - sync echo server/client
//  - async echo server (async client, too?)
//  - gui app that is also a client
//  - gui app that is also a server
//  - server that is also a client to 1+ servers
//  - coroutines

using asio::ip::tcp;

// TODO: Thread ids in logs
void session(std::stop_token stop_token, tcp::socket socket)
{
  auto port = socket.remote_endpoint().port();

  spdlog::info("{}: accepted client", port);

  while (!stop_token.stop_requested()) {
    std::array<char, 1024> buf{};

    // Read message from the client
    asio::error_code ec;
    auto length = socket.read_some(asio::buffer(buf), ec);

    if (ec == asio::error::eof) {
      spdlog::info("{}: connection closed", port);
      break;
    }

    if (ec) {
      spdlog::error("{}: read_some: {}", port, ec.message());
      break;
    }

    std::string_view message(buf.data(), length);

    spdlog::info("{}: read_some: {}", port, message);

    // 'Echo' the message back to the client
    asio::write(socket, asio::buffer(buf, length), ec);

    if (ec) {
      spdlog::error("{}: write: {}", port, ec.message());
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  std::span<char*> args(argv, argc);

  spdlog::set_level(spdlog::level::debug);

  if (argc != 2) {
    std::cerr << std::format("usage: {} <port>", args[0]);
    return 1;
  }

  unsigned short port = std::atoi(args[1]);

  // Server execution context
  asio::io_context io_context;

  // TCP acceptor
  // NOTE: This constructor will throw an exception if there was an error binding
  // or listening on the port. For example, if you use a reserved port number. There
  // is no overload to use an error code.
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

  spdlog::info("Listening on port {}", port);

  // Server loop
  std::vector<std::jthread> threads;
  while (true) {

    // Accept a new connection
    asio::error_code ec;
    tcp::socket socket = acceptor.accept(ec);

    if (ec) {
      spdlog::error("accept: {}", ec.message());
      continue;
    }

    // Handle new connection on its own thread
    threads.emplace_back(session, std::move(socket));
  }

  return 0;
}
