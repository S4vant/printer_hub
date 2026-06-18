cmake_minimum_required(VERSION 3.20)

project(print-agent)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)

pkg_check_modules(
    SYSTEMD REQUIRED
    libsystemd
)

find_package(nlohmann_json REQUIRED)

add_executable(
    print-agent

    src/main.cpp
    src/JournalReader.cpp
    src/JobParser.cpp
    src/JsonWriter.cpp
)

target_include_directories(
    print-agent
    PRIVATE
    src
    ${SYSTEMD_INCLUDE_DIRS}
)

target_link_libraries(
    print-agent
    PRIVATE
    ${SYSTEMD_LIBRARIES}
    nlohmann_json::nlohmann_json
)