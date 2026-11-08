cmake_minimum_required(VERSION 3.20)
project(VeriCodeAST VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler optimization and warning flags
if(MSVC)
    add_compile_options(/W4 /permissive- /O2 /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -O3 -fPIC)
endif()

# Header include paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core library source files
set(CORE_SOURCES
    src/ast_visitor.cpp
    src/cfg.cpp
    src/diagnostic.cpp
    src/verifier_engine.cpp
    src/rules/cwe_401_memory_leak.cpp
    src/rules/cwe_362_race_condition.cpp
    src/rules/cwe_476_null_deref.cpp
)

# Static library for linking and benchmarking
add_library(vericode_core STATIC ${CORE_SOURCES})

# Command-line interface executable
add_executable(vericode_cli src/main.cpp)
target_link_libraries(vericode_cli PRIVATE vericode_core)

# Install targets
install(TARGETS vericode_cli vericode_core
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)
