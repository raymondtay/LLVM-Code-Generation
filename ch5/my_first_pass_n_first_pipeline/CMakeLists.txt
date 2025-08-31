cmake_minimum_required(VERSION 3.13)
project(MyFirstPass LANGUAGES C CXX)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Build shared object for opt to load
add_library(MyFirstPass SHARED MyFirstPass.cpp)

# Link against LLVM
llvm_map_components_to_libnames(llvm_libs support core irreader passes)
target_link_libraries(MyFirstPass PRIVATE ${llvm_libs})

set_target_properties(MyFirstPass PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)

add_executable(run_pipeline RunPipeline.cpp MyFirstPass.cpp)

# Link against LLVM libs needed for reading IR and running passes
llvm_map_components_to_libnames(llvm_libs core irreader passes analysis support)

target_link_libraries(run_pipeline PRIVATE ${llvm_libs})
target_include_directories(run_pipeline PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_features(run_pipeline PRIVATE cxx_std_17)

