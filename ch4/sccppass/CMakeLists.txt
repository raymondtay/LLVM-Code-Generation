cmake_minimum_required(VERSION 3.20)
project(LocalConstProp LANGUAGES CXX C)

find_package(LLVM 20.1.8 REQUIRED CONFIG)       # Pick the LLVM you have
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_library(LocalConstProp MODULE ConstProp.cpp)
target_compile_features(LocalConstProp PRIVATE cxx_std_17)
target_link_libraries(LocalConstProp PRIVATE LLVMCore)

include(LLVMTestingSupport)
add_subdirectory(tests)


