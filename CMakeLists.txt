cmake_minimum_required(VERSION 3.16)
project(Quantis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(quantis
    src/main.cpp
    src/ScreenerEngine.cpp
    src/Storage.cpp
    src/MarketDataProvider.cpp
    src/TableRenderer.cpp
)

target_include_directories(quantis PRIVATE include ${SQLite3_INCLUDE_DIRS})

target_link_libraries(quantis PRIVATE ${SQLite3_LIBRARIES})

install(TARGETS quantis RUNTIME DESTINATION bin)

