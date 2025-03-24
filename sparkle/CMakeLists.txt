project(sparkle LANGUAGES CXX)

add_library(sparkle
        # aarch64 codegen backend
        lib/target/aarch64/common.cpp
        lib/target/aarch64/encoder/arith.cpp
        lib/target/aarch64/encoder/atomic.cpp
        lib/target/aarch64/encoder/bitfield.cpp
        lib/target/aarch64/encoder/bitmanip.cpp
        lib/target/aarch64/encoder/branch.cpp
        lib/target/aarch64/encoder/cmp.cpp
        lib/target/aarch64/encoder/conditional.cpp
        lib/target/aarch64/encoder/data.cpp
        lib/target/aarch64/encoder/fp.cpp
        lib/target/aarch64/encoder/logical.cpp
        lib/target/aarch64/encoder/memory.cpp
        lib/target/aarch64/encoder/mul.cpp
        lib/target/aarch64/encoder/shift.cpp
        lib/target/aarch64/encoder/simd.cpp
        lib/target/aarch64/encoder/system.cpp
)

target_include_directories(sparkle PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(SparkleCGenAarch64
        tests/mcodegen/aarch64.cpp
)

target_include_directories(SparkleCGenAarch64 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleCGenAarch64 PRIVATE
        sparkle
)