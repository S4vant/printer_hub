cmake_minimum_required(VERSION 3.20)

project(print-agent)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)

pkg_check_modules(
    SYSTEMD REQUIRED
    libsystemd
)

find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)

add_executable(
    print-agent

    src/main.cpp

    src/cli/CommandLine.cpp

    src/agent/Agent.cpp

    src/parser/JournalReader.cpp
    src/parser/JobParser.cpp

    src/output/JsonWriter.cpp

    src/config/Config.cpp

    src/transport/HttpSender.cpp
    src/transport/ZabbixSender.cpp
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
    CURL::libcurl
)