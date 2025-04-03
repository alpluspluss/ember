project(sparkle LANGUAGES CXX)

add_library(sparkle
        lib/sprout/region.cpp
        lib/sprout/passes/aa.cpp
        lib/sprout/passes/dce.cpp
        lib/sprout/passes/ipa.cpp
        lib/sprout/passes/ipo.cpp
        lib/sprout/passes/pre.cpp
        lib/sprout/utils/dump.cpp

        lib/sprout/utils/irutils.cpp
        lib/sprout/utils/printer.cpp

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

### dce optimization
add_executable(SparkleDCEOptimization
        tests/optimization/dce.cpp
)

target_include_directories(SparkleDCEOptimization PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleDCEOptimization PRIVATE
        sparkle
)

### alias analysis
add_executable(SparkleAliasAnalysis
        tests/optimization/aa.cpp
)

target_include_directories(SparkleAliasAnalysis PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleAliasAnalysis PRIVATE
        sparkle
)

### inter-procedural analysis
add_executable(SparkleIPA
        tests/optimization/ipa.cpp
)

target_include_directories(SparkleIPA PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleIPA PRIVATE
        sparkle
)

### inter-procedural optimization
add_executable(SparkleIPO
        tests/optimization/ipo.cpp
)

target_include_directories(SparkleIPO PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleIPO PRIVATE
        sparkle
)

## pre optimization
add_executable(SparklePRE
        tests/optimization/pre.cpp
)

target_include_directories(SparklePRE PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparklePRE PRIVATE
        sparkle
)

## integration test optimization (all)
add_executable(SparkleIRTest
        tests/optimization/integration.cpp
)

target_include_directories(SparkleIRTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SparkleIRTest PRIVATE
        sparkle
)