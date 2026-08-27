add_executable(monetix_gateway
    src/main.cpp
    src/http_gateway.cpp
    src/auth_interceptor.cpp
    src/rate_limiter.cpp
    src/nats_publisher.cpp
)
target_include_directories(monetix_gateway PRIVATE include)
target_link_libraries(monetix_gateway PRIVATE monetix_common Boost::system OpenSSL::Crypto Threads::Threads)
