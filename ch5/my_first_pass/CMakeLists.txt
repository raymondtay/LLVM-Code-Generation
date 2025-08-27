cmake_minimum_required(VERSION 3.13)
project(HelloPass LANGUAGES C CXX)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Build shared object for opt to load
add_library(HelloPass SHARED HelloPass.cpp)

# Link against LLVM
llvm_map_components_to_libnames(llvm_libs support core irreader passes)
target_link_libraries(HelloPass PRIVATE ${llvm_libs})

set_target_properties(HelloPass PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)


