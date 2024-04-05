add_executable(blocking-client blocking_client.cpp)
target_link_libraries(blocking-client asio spdlog)

add_executable(blocking-client-mt blocking_client_mt.cpp)
target_link_libraries(blocking-client-mt asio spdlog)

add_executable(blocking-server blocking_server.cpp)
target_link_libraries(blocking-server asio spdlog)

add_executable(blocking-server-mt blocking_server_mt.cpp)
target_link_libraries(blocking-server-mt asio spdlog)

add_executable(async-client async_client.cpp)
target_link_libraries(async-client asio spdlog)

add_executable(async-client-mt async_client_mt.cpp)
target_link_libraries(async-client-mt asio spdlog)

add_executable(async-server async_server.cpp)
target_link_libraries(async-server asio spdlog)

add_executable(async-server-mt async_server_mt.cpp)
target_link_libraries(async-server-mt asio spdlog)

# Coroutines
add_executable(async-client-coro async_client_coro.cpp)
target_link_libraries(async-client-coro asio spdlog)

add_executable(async-client-mt-coro async_client_mt_coro.cpp)
target_link_libraries(async-client-mt-coro asio spdlog)

add_executable(async-server-coro async_server_coro.cpp)
target_link_libraries(async-server-coro asio spdlog)

add_executable(async-server-mt-coro async_server_mt_coro.cpp)
target_link_libraries(async-server-mt-coro asio spdlog)
