project(sparkle LANGUAGES CXX)

add_library(sparkle
        lib/ir/obj/region.cpp
        lib/ir/pass/dce.cpp
        lib/ir/pass/manager.cpp
        lib/ir/utils/irdump.cpp

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

        # risc-v
        lib/target/risc-v/encoder/arith.cpp
        lib/target/risc-v/encoder/atomic.cpp
        lib/target/risc-v/encoder/bitmanip.cpp
        lib/target/risc-v/encoder/branch.cpp
        lib/target/risc-v/encoder/cmp.cpp
        lib/target/risc-v/encoder/data.cpp
        lib/target/risc-v/encoder/fp.cpp
        lib/target/risc-v/encoder/logical.cpp
        lib/target/risc-v/encoder/memory.cpp
        lib/target/risc-v/encoder/mul.cpp
        lib/target/risc-v/encoder/shift.cpp
        lib/target/risc-v/encoder/simd.cpp
        lib/target/risc-v/encoder/system.cpp
)

target_include_directories(sparkle PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# tests
### mc codegen aarch64
add_executable(SparkleCGenAarch64
        tests/mcodegen/aarch64.cpp
)

target_include_directories(SparkleCGenAarch64 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleCGenAarch64 PRIVATE
        sparkle
)

# ir
add_executable(SparkleDCE
        tests/ir/dce.cpp
)

target_include_directories(SparkleDCE PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleDCE PRIVATE
        sparkle
)