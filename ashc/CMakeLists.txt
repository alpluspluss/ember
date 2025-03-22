project(ash LANGUAGES CXX)

# frontend lib
add_library(ash
        lib/lex/lexer.cpp
)

target_include_directories(ash PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# the compiler driver
add_executable(ashc
        tools/ashc/main.cpp
)

target_include_directories(ashc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ashc PRIVATE
        ash
)