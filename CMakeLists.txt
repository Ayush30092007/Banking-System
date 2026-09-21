cmake_minimum_required(VERSION 3.12)

project(BankingSystemBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(banking_backend
    src/BankService.cpp
    src/FileStorage.cpp
    src/Models.cpp
)

target_include_directories(banking_backend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(banking_app src/main.cpp)
target_link_libraries(banking_app PRIVATE banking_backend)

enable_testing()

add_executable(backend_tests tests/backend_tests.cpp)
target_link_libraries(backend_tests PRIVATE banking_backend)

add_test(NAME backend_tests COMMAND backend_tests)
