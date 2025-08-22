# UI Server CMakeLists.txt - Web-based Tank Simulator Interface
cmake_minimum_required(VERSION 3.14)

# Define the tank_simulator_ui executable
add_executable(tank_simulator_ui
    ui_main.cpp
)

# Set C++20 standard
set_target_properties(tank_simulator_ui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Set output directory
set_target_properties(tank_simulator_ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)

# Include directories
target_include_directories(tank_simulator_ui PRIVATE
    "${CMAKE_SOURCE_DIR}/common"
    "${CMAKE_SOURCE_DIR}/"
    "${CMAKE_SOURCE_DIR}/third_party"
    "${CMAKE_SOURCE_DIR}/UserCommon"
)

# Link required libraries
target_link_libraries(tank_simulator_ui PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    UserCommon
)

# Add compiler flags
target_compile_options(tank_simulator_ui PRIVATE
    -Wall -Wextra -Werror -pedantic
)