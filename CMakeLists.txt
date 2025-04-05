cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party/googletest)
enable_testing()

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "include/*.h")

add_library(mylibrary ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_include_directories(mylibrary
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(mytests ${TEST_SOURCES})
target_include_directories(mytests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(mytests mylibrary gtest gtest_main)
add_test(NAME UnitTests COMMAND mytests)

install(TARGETS mylibrary
    PUBLIC_HEADER DESTINATION include
)