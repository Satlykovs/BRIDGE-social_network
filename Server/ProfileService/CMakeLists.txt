cmake_minimum_required(VERSION 3.22)

project(profile_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(userver REQUIRED core postgresql chaotic kafka s3api)


file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)

file (GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp)

add_subdirectory(../Common/JwtMiddleware ${CMAKE_BINARY_DIR}/Common/JwtMiddleware)


include_directories(
    
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

userver_target_generate_chaotic(${PROJECT_NAME}-chgen
    ARGS
        -n "/components/schemas/([^/]*)/=profile_service::models::{0}"
        --generate-serializers
    OUTPUT_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    SCHEMAS
        ${SCHEMAS}
    RELATIVE_TO
        ${CMAKE_CURRENT_SOURCE_DIR}
)

userver_add_sql_library(
    ${PROJECT_NAME}_sql
    NAMESPACE sql_queries
    OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/sql
    SQL_FILES sql/*.sql
)

add_executable(${PROJECT_NAME} main.cpp ${CPP_FILES})


target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_sql ${PROJECT_NAME}-chgen)


target_link_libraries(${PROJECT_NAME} PUBLIC userver::core userver::postgresql userver::chaotic userver::s3api userver::kafka userver::s3api JwtMiddleware)

find_package(ICU REQUIRED COMPONENTS uc)
target_link_libraries(${PROJECT_NAME} PRIVATE ICU::uc)
