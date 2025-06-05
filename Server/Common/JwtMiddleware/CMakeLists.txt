cmake_minimum_required(VERSION 3.22)
project(jwt_middleware)

find_package(userver REQUIRED core)
find_package(jwt-cpp CONFIG REQUIRED)


add_library(JwtMiddleware STATIC 
    src/JwtMiddleware.cpp
    include/JwtMiddleware.hpp
)


target_link_libraries(JwtMiddleware PUBLIC 
    userver::core 
    jwt-cpp::jwt-cpp
)


target_include_directories(JwtMiddleware PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)