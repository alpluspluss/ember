#pragma once

#include <cstdint>

namespace sprk::riscv 
{
    /* Floating-point Add, Single-precision */
    uint32_t encode_fadd_s(int rd, int rs1, int rs2, int rm);

    /* Floating-point Subtract, Single-precision */
    uint32_t encode_fsub_s(int rd, int rs1, int rs2, int rm);

    /* Floating-point Multiply, Single-precision */
    uint32_t encode_fmul_s(int rd, int rs1, int rs2, int rm);

    /* Floating-point Divide, Single-precision */
    uint32_t encode_fdiv_s(int rd, int rs1, int rs2, int rm);

    /* Floating-point Square Root, Single-precision */
    uint32_t encode_fsqrt_s(int rd, int rs1, int rm);

    /* Floating-point Minimum, Single-precision */
    uint32_t encode_fmin_s(int rd, int rs1, int rs2);

    /* Floating-point Maximum, Single-precision */
    uint32_t encode_fmax_s(int rd, int rs1, int rs2);

    /* Floating-point Load Word */
    uint32_t encode_flw(int rd, int rs1, int offset);

    /* Floating-point Store Word */
    uint32_t encode_fsw(int rs2, int rs1, int offset);

    /* Floating-point Load Doubleword */
    uint32_t encode_fld(int rd, int rs1, int offset);

    /* Floating-point Store Doubleword */
    uint32_t encode_fsd(int rs2, int rs1, int offset);

    /* Floating-point Multiply-Add, Single-precision */
    uint32_t encode_fmadd_s(int rd, int rs1, int rs2, int rs3, int rm);

    /* Floating-point Multiply-Subtract, Single-precision */
    uint32_t encode_fmsub_s(int rd, int rs1, int rs2, int rs3, int rm);

    /* Floating-point Negated Multiply-Subtract, Single-precision */
    uint32_t encode_fnmsub_s(int rd, int rs1, int rs2, int rs3, int rm);

    /* Floating-point Negated Multiply-Add, Single-precision */
    uint32_t encode_fnmadd_s(int rd, int rs1, int rs2, int rs3, int rm);

    /* Floating-point Sign Injection */
    uint32_t encode_fsgnj_s(int rd, int rs1, int rs2);

    /* Floating-point Negated Sign Injection */
    uint32_t encode_fsgnjn_s(int rd, int rs1, int rs2);

    /* Floating-point Sign Injection XOR */
    uint32_t encode_fsgnjx_s(int rd, int rs1, int rs2);

    /* Floating-point Equal Comparison */
    uint32_t encode_feq_s(int rd, int rs1, int rs2);

    /* Floating-point Less Than Comparison */
    uint32_t encode_flt_s(int rd, int rs1, int rs2);

    /* Floating-point Less Than or Equal Comparison */
    uint32_t encode_fle_s(int rd, int rs1, int rs2);

    /* Floating-point Classification */
    uint32_t encode_fclass_s(int rd, int rs1);

    /* Float to Signed Integer (32-bit) Conversion */
    uint32_t encode_fcvt_w_s(int rd, int rs1, int rm);

    /* Float to Unsigned Integer (32-bit) Conversion */
    uint32_t encode_fcvt_wu_s(int rd, int rs1, int rm);

    /* Signed Integer (32-bit) to Float Conversion */
    uint32_t encode_fcvt_s_w(int rd, int rs1, int rm);

    /* Unsigned Integer (32-bit) to Float Conversion */
    uint32_t encode_fcvt_s_wu(int rd, int rs1, int rm);

    /* RV64 ONLY */

    /* Float to Signed Integer (64-bit) Conversion */
    uint32_t encode_fcvt_l_s(int rd, int rs1, int rm);

    /* Float to Unsigned Integer (64-bit) Conversion */
    uint32_t encode_fcvt_lu_s(int rd, int rs1, int rm);

    /* Signed Integer (64-bit) to Float Conversion */
    uint32_t encode_fcvt_s_l(int rd, int rs1, int rm);

    /* Unsigned Integer (64-bit) to Float Conversion */
    uint32_t encode_fcvt_s_lu(int rd, int rs1, int rm);

    /* Move 32-bit Float to Integer Register */
    uint32_t encode_fmv_x_s(int rd, int rs1);

    /* Move Integer Register to 32-bit Float */
    uint32_t encode_fmv_s_x(int rd, int rs1);
}