project(proxy)
add_executable(proxy main.cpp src/tcp_proxy.cpp)
target_compile_features(proxy PRIVATE cxx_std_17)
