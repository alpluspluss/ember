#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_msr_imm(uint32_t op1, uint32_t CRn, uint32_t CRm, uint32_t op2);
    uint32_t encode_msr_reg(int rt, uint32_t op1, uint32_t CRn, uint32_t CRm, uint32_t op2);
    uint32_t encode_mrs(int rt, uint32_t op1, uint32_t CRn, uint32_t CRm, uint32_t op2);
    uint32_t encode_dsb(uint32_t option);
    uint32_t encode_dmb(uint32_t option);
    uint32_t encode_isb(uint32_t option);
    uint32_t encode_svc(uint32_t imm16);
    uint32_t encode_hvc(uint32_t imm16);
    uint32_t encode_smc(uint32_t imm16);
    uint32_t encode_hint(uint32_t imm);
    uint32_t encode_wfe(void);
    uint32_t encode_wfi(void);
    uint32_t encode_sev(void);
    uint32_t encode_nop(void);
    uint32_t encode_brk(uint32_t imm16);
    uint32_t encode_dc(int rt, uint32_t op1, uint32_t CRm, uint32_t op2);
}