#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Set Vector Length Immediate */
    uint32_t encode_vsetvli(int rd, int rs1, int vtypei);

    /* Vector Load Element, 32-bit */
    uint32_t encode_vle32_v(int vd, int rs1, bool mask_used);

    /* Vector Store Element, 32-bit */
    uint32_t encode_vse32_v(int vs3, int rs1, bool mask_used);

    /* Vector Add, Vector-Vector */
    uint32_t encode_vadd_vv(int vd, int vs1, int vs2, bool mask_used);

    /* Vector Multiply, Vector-Vector */
    uint32_t encode_vmul_vv(int vd, int vs1, int vs2, bool mask_used);

    /* Vector Floating-point Add, Vector-Vector */
    uint32_t encode_vfadd_vv(int vd, int vs1, int vs2, bool mask_used);
}
