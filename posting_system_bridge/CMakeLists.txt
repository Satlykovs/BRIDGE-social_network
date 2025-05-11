cmake_minimum_required(VERSION 3.22)

project(post_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(ICU REQUIRED COMPONENTS data i18n uc io)

find_package(Boost REQUIRED COMPONENTS context)

if(NOT TARGET Boost::context)
    find_library(Boost_CONTEXT_LIBRARY NAMES boost_context PATHS ${Boost_ROOT}/lib)
    if(Boost_CONTEXT_LIBRARY)
        add_library(Boost::context UNKNOWN IMPORTED)
        set_target_properties(Boost::context PROPERTIES IMPORTED_LOCATION ${Boost_CONTEXT_LIBRARY})
    else()
        message(FATAL_ERROR "Could not find boost_context library!")
    endif()
endif()

find_package(userver REQUIRED core postgresql chaotic)

file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

userver_target_generate_chaotic(${PROJECT_NAME}-chgen
    ARGS
        -n "/components/schemas/([^/]*)/=post_service::models::{0}"
        --generate-serializers
    OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
    SCHEMAS ${SCHEMAS}
    RELATIVE_TO ${CMAKE_CURRENT_SOURCE_DIR}
)

userver_add_sql_library(
    ${PROJECT_NAME}_sql
    NAMESPACE sql_queries
    OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/sql
    SQL_FILES sql/*.sql
)

add_executable(${PROJECT_NAME} main.cpp ${CPP_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_sql ${PROJECT_NAME}-chgen)
target_link_libraries(${PROJECT_NAME} PUBLIC userver::core userver::postgresql userver::chaotic)

find_package(ICU REQUIRED COMPONENTS uc)
target_link_libraries(${PROJECT_NAME} PRIVATE ICU::uc ICU::data ICU::i18n ICU::io)
