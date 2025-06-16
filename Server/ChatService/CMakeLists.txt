cmake_minimum_required(VERSION 3.22)

project(chat_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




find_package(userver REQUIRED core postgresql)

add_subdirectory(../Common/JwtMiddleware ${CMAKE_BINARY_DIR}/Common/JwtMiddleware)

#file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)

file (GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp)

include_directories(
    
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# userver_target_generate_chaotic(${PROJECT_NAME}-chgen
#     # Generate serializers for responses
#     GENERATE_SERIALIZERS
#     # Map '/components/schemas/*' JSONSchema types to C++ types in 'samples::hello' namespace
#     LAYOUT
#         "/components/schemas/([^/]*)/=chat_service::dto::{0}"
#     OUTPUT_DIR
#         ${CMAKE_CURRENT_BINARY_DIR}/src
#     SCHEMAS
#         ${SCHEMAS}
#     RELATIVE_TO
#         ${CMAKE_CURRENT_SOURCE_DIR}
# )


add_executable(${PROJECT_NAME} main.cpp ${CPP_FILES})


#target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}-chgen)


target_link_libraries(${PROJECT_NAME} PUBLIC userver::core userver::postgresql JwtMiddleware)

find_package(ICU REQUIRED COMPONENTS uc)
target_link_libraries(${PROJECT_NAME} PRIVATE ICU::uc)