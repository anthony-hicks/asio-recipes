add_executable(echo-server echo_server_main.cpp)
add_executable(echo-client echo_client_main.cpp)

target_link_libraries(echo-server PRIVATE spdlog asio)
target_link_libraries(echo-client PRIVATE spdlog asio)

target_enable_sanitizers(echo-server "Address")
target_enable_sanitizers(echo-server "Undefined")
target_enable_sanitizers(echo-client "Address")
target_enable_sanitizers(echo-client "Undefined")
