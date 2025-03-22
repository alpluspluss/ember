project(ash LANGUAGES CXX)

# the compiler driver
add_executable(ashc
        tools/ashc/main.cpp
)