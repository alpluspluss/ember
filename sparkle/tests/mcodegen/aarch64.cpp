#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <sparkle/target/aarch64/encoding/arith.hpp>
#include <sparkle/target/aarch64/encoding/atomic.hpp>
#include <sparkle/target/aarch64/encoding/bitfield.hpp>
#include <sparkle/target/aarch64/encoding/branch.hpp>
#include <sparkle/target/aarch64/encoding/cmp.hpp>
#include <sparkle/target/aarch64/encoding/data.hpp>
#include <sparkle/target/aarch64/encoding/logical.hpp>
#include <sparkle/target/aarch64/encoding/memory.hpp>
#include <sparkle/target/aarch64/encoding/shift.hpp>
#include <sparkle/target/aarch64/encoding/system.hpp>
#include <sparkle/target/aarch64/common.hpp>
#include <sparkle/target/aarch64/encoding/fp.hpp>
#include <sparkle/target/aarch64/encoding/simd.hpp>
#include <sparkle/target/aarch64/encoding/conditional.hpp>
#include <sparkle/target/aarch64/encoding/mul.hpp>
#include <sparkle/target/aarch64/encoding/bitmanip.hpp>

using namespace sprk::aarch64;

void write_binary_file(const std::string& filename, const std::vector<uint32_t>& instructions)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file)
	{
		std::cerr << "error: Unable to open file " << filename << " for writing\n";
		return;
	}

	file.write(reinterpret_cast<const char*>(instructions.data()), instructions.size() * sizeof(uint32_t));
	std::cout << "successfully wrote " << instructions.size() << " instructions to " << filename << "\n";
}

void test_arithmetic()
{
    std::vector<uint32_t> instructions;

    instructions.push_back(encode_add_imm(0, 1, 42, false, false));
    instructions.push_back(encode_add_imm(2, 3, 1000, true, false));
    instructions.push_back(encode_add_imm(4, 5, 1, false, true));
    instructions.push_back(encode_add_imm(6, 31, 4095, true, true));

    instructions.push_back(encode_add_reg(0, 1, 2, 0, 0, false, false));
    instructions.push_back(encode_add_reg(3, 4, 5, 0, 0, true, false));
    instructions.push_back(encode_add_reg(6, 7, 8, 0, 2, false, false));
    instructions.push_back(encode_add_reg(9, 10, 11, 1, 4, true, true));

    instructions.push_back(encode_sub_imm(0, 1, 42, false, false));
    instructions.push_back(encode_sub_imm(2, 3, 1000, true, false));
    instructions.push_back(encode_sub_imm(4, 5, 1, false, true));
    instructions.push_back(encode_sub_imm(6, 31, 4095, true, true));

    instructions.push_back(encode_sub_reg(0, 1, 2, 0, 0, false, false));
    instructions.push_back(encode_sub_reg(3, 4, 5, 0, 0, true, false));
    instructions.push_back(encode_sub_reg(6, 7, 8, 0, 2, false, false));
    instructions.push_back(encode_sub_reg(9, 10, 11, 1, 4, true, true));

    instructions.push_back(encode_add_ext(0, 1, 2, 2, 2, false, false));
    instructions.push_back(encode_sub_ext(3, 4, 5, 6, 3, true, true));

    write_binary_file("arithmetic_test.bin", instructions);
}

void test_atomic()
{
    std::vector<uint32_t> instructions;

    /* load store exclu */
    instructions.push_back(encode_ldxr(0, 1, 0));
    instructions.push_back(encode_ldxr(2, 3, 1));
    instructions.push_back(encode_ldxr(4, 5, 2));
    instructions.push_back(encode_ldxr(6, 7, 3));
    
    instructions.push_back(encode_stxr(0, 1, 2, 0));
    instructions.push_back(encode_stxr(3, 4, 5, 1));
    instructions.push_back(encode_stxr(6, 7, 8, 2));
    instructions.push_back(encode_stxr(9, 10, 11, 3));

    /* exclusive lasr */
    instructions.push_back(encode_ldaxr(0, 1, 0));
    instructions.push_back(encode_ldaxr(2, 3, 1));
    instructions.push_back(encode_ldaxr(4, 5, 2));
    instructions.push_back(encode_ldaxr(6, 7, 3));
    
    instructions.push_back(encode_stlxr(0, 1, 2, 0));
    instructions.push_back(encode_stlxr(3, 4, 5, 1));
    instructions.push_back(encode_stlxr(6, 7, 8, 2));
    instructions.push_back(encode_stlxr(9, 10, 11, 3));

    /* CAS - only supports word (32-bit) & doubleword (64-bit) */
    instructions.push_back(encode_cas(6, 7, 8, 2, false, false)); /* CAS */
    instructions.push_back(encode_cas(6, 7, 8, 2, true, false)); /* CASA */
    instructions.push_back(encode_cas(6, 7, 8, 2, false, true)); /* CASL */
    instructions.push_back(encode_cas(6, 7, 8, 2, true, true)); /* CASAL */

    instructions.push_back(encode_cas(9, 10, 11, 3, false, false)); /* CAS */
    instructions.push_back(encode_cas(9, 10, 11, 3, true, false)); /* CASA */
    instructions.push_back(encode_cas(9, 10, 11, 3, false, true)); /* CASL */
    instructions.push_back(encode_cas(9, 10, 11, 3, true, true)); /* CASAL */

    /* atomic add */
    instructions.push_back(encode_ldadd(0, 1, 2, 0, false, false));
    instructions.push_back(encode_ldadd(3, 4, 5, 1, true, false));
    instructions.push_back(encode_ldadd(6, 7, 8, 2, false, true));
    instructions.push_back(encode_ldadd(9, 10, 11, 3, true, true));

    /* swap */
    instructions.push_back(encode_swp(0, 1, 2, 0, false, false));
    instructions.push_back(encode_swp(3, 4, 5, 1, true, false));
    instructions.push_back(encode_swp(6, 7, 8, 2, false, true));
    instructions.push_back(encode_swp(9, 10, 11, 3, true, true));

    write_binary_file("atomic_test.bin", instructions);
}

void test_bitfield()
{
    std::vector<uint32_t> instructions;

    /* bitfield move */
    instructions.push_back(encode_bfm(0, 1, 8, 15, false, 0));
    instructions.push_back(encode_bfm(2, 3, 16, 31, false, 1));
    instructions.push_back(encode_bfm(4, 5, 32, 47, true, 2));
    
    /* bitfield insert */
    instructions.push_back(encode_bfi(0, 1, 0, 8, false)); 
    instructions.push_back(encode_bfi(2, 3, 16, 8, false)); 
    instructions.push_back(encode_bfi(4, 5, 32, 16, true));
    
    /* bitfield extract & insert low */
    instructions.push_back(encode_bfxil(0, 1, 0, 8, false));
    instructions.push_back(encode_bfxil(2, 3, 16, 8, false));
    instructions.push_back(encode_bfxil(4, 5, 32, 16, true));
    
    /* signed bitfield extract */
    instructions.push_back(encode_sbfx(0, 1, 0, 8, false));
    instructions.push_back(encode_sbfx(2, 3, 16, 8, false));
    instructions.push_back(encode_sbfx(4, 5, 32, 16, true));
    
    /* unsigned bitfield extract */
    instructions.push_back(encode_ubfx(0, 1, 0, 8, false));
    instructions.push_back(encode_ubfx(2, 3, 16, 8, false));
    instructions.push_back(encode_ubfx(4, 5, 32, 16, true));
    
    /* arithmetic shift right via alias */
    instructions.push_back(encode_asr_imm_alias(0, 1, 8, false));
    instructions.push_back(encode_asr_imm_alias(2, 3, 16, false));
    instructions.push_back(encode_asr_imm_alias(4, 5, 32, true));
    
    /* extract reg */
    instructions.push_back(encode_extr(0, 1, 2, 8, false));
    instructions.push_back(encode_extr(3, 4, 5, 16, false));
    instructions.push_back(encode_extr(6, 7, 8, 32, true));

    write_binary_file("bitfield_test.bin", instructions);
}

void test_branch() /* NOTE TO AL: use fixed branch offset to test only */
{
    std::vector<uint32_t> instructions;

    instructions.push_back(encode_b(16));
    instructions.push_back(encode_b(-16));
    instructions.push_back(encode_bl(64));
    instructions.push_back(encode_bl(-64));
    
    /* conditional */
    instructions.push_back(encode_b_cond(0x0, 16));
    instructions.push_back(encode_b_cond(0x1, -16));
    instructions.push_back(encode_b_cond(0x2, 64));
    instructions.push_back(encode_b_cond(0x3, -64));
    
    /* compare branch */
    instructions.push_back(encode_cbz(0, 16, false));
    instructions.push_back(encode_cbz(1, -16, true));
    instructions.push_back(encode_cbnz(2, 64, false));
    instructions.push_back(encode_cbnz(3, -64, true));
    
    /* test ad branch if zero */
    instructions.push_back(encode_tbz(0, 0, 16)); 
    instructions.push_back(encode_tbz(1, 31, -16)); 
    instructions.push_back(encode_tbz(2, 63, 64)); 
    instructions.push_back(encode_tbnz(3, 0, -16)); 
    instructions.push_back(encode_tbnz(4, 31, 64)); 
    instructions.push_back(encode_tbnz(5, 63, -64));

    write_binary_file("branch_test.bin", instructions);
}

void test_cmp()
{
    std::vector<uint32_t> instructions;
    
    /* CMP imm */
    instructions.push_back(encode_cmp_imm(0, 0, false)); 
    instructions.push_back(encode_cmp_imm(1, 42, false));
    instructions.push_back(encode_cmp_imm(2, 1000, true));
    instructions.push_back(encode_cmp_imm(3, 4095, true));
    
    /* CMP reg inst */
    instructions.push_back(encode_cmp_reg(0, 1, 0, 0, false));
    instructions.push_back(encode_cmp_reg(2, 3, 0, 0, true));
    instructions.push_back(encode_cmp_reg(4, 5, 0, 2, false));
    instructions.push_back(encode_cmp_reg(6, 7, 1, 4, true));
    instructions.push_back(encode_cmp_reg(8, 9, 2, 8, false));
    
    /* TST reg inst */
    instructions.push_back(encode_tst_reg(0, 1, 0, 0, false)); 
    instructions.push_back(encode_tst_reg(2, 3, 0, 0, true)); 
    instructions.push_back(encode_tst_reg(4, 5, 0, 2, false)); 
    instructions.push_back(encode_tst_reg(6, 7, 1, 4, true));
    instructions.push_back(encode_tst_reg(8, 9, 2, 8, false));
    instructions.push_back(encode_tst_reg(10, 11, 3, 16, true));

    write_binary_file("compare_test.bin", instructions);
}

void test_data_move()
{
    std::vector<uint32_t> instructions;
    
    /* move reg */
    instructions.push_back(encode_mov_reg(0, 1, false));
    instructions.push_back(encode_mov_reg(2, 3, true));
    
    /* move not reg variant */
    instructions.push_back(encode_mvn_reg(0, 1, 0, 0, false)); 
    instructions.push_back(encode_mvn_reg(2, 3, 0, 0, true));
    instructions.push_back(encode_mvn_reg(4, 5, 0, 2, false));
    instructions.push_back(encode_mvn_reg(6, 7, 1, 4, true));
    instructions.push_back(encode_mvn_reg(8, 9, 2, 8, false));
    instructions.push_back(encode_mvn_reg(10, 11, 3, 16, true));
    
    /* move imm */
    instructions.push_back(encode_mov_imm(0, 0, false));
    instructions.push_back(encode_mov_imm(1, 42, false));
    instructions.push_back(encode_mov_imm(2, 0xFFFF, true));
    
    /* move zero */
    instructions.push_back(encode_movz(0, 0, 0, false));
    instructions.push_back(encode_movz(1, 0xABCD, 0, false));
    instructions.push_back(encode_movz(2, 0x1234, 16, false));
    instructions.push_back(encode_movz(3, 0x5678, 0, true));
    instructions.push_back(encode_movz(4, 0x9ABC, 16, true));
    instructions.push_back(encode_movz(5, 0xDEF0, 32, true));
    instructions.push_back(encode_movz(6, 0x1234, 48, true));
    
    /* move not */
    instructions.push_back(encode_movn(0, 0, 0, false));
    instructions.push_back(encode_movn(1, 0xABCD, 0, false));
    instructions.push_back(encode_movn(2, 0x1234, 16, false));
    instructions.push_back(encode_movn(3, 0x5678, 0, true)); 
    instructions.push_back(encode_movn(4, 0x9ABC, 16, true));
    instructions.push_back(encode_movn(5, 0xDEF0, 32, true));
    instructions.push_back(encode_movn(6, 0x1234, 48, true));
    
    /* move keep */
    instructions.push_back(encode_movk(0, 0xABCD, 0, false));
    instructions.push_back(encode_movk(1, 0x1234, 16, false));
    instructions.push_back(encode_movk(2, 0x5678, 0, true));
    instructions.push_back(encode_movk(3, 0x9ABC, 16, true));
    instructions.push_back(encode_movk(4, 0xDEF0, 32, true));
    instructions.push_back(encode_movk(5, 0x1234, 48, true));

    write_binary_file("data_move_test.bin", instructions);
}

void test_logical()
{
    std::vector<uint32_t> instructions;
    
    /* bunchas of reg instrs idk lol */
    instructions.push_back(encode_and_reg(0, 1, 2, 0, 0, false, false));
    instructions.push_back(encode_and_reg(3, 4, 5, 0, 0, true, false));
    instructions.push_back(encode_and_reg(6, 7, 8, 0, 0, false, true));
    
    instructions.push_back(encode_orr_reg(0, 1, 2, 0, 0, false));
    instructions.push_back(encode_orr_reg(3, 4, 5, 0, 0, true));
    
    instructions.push_back(encode_eor_reg(0, 1, 2, 0, 0, false));
    instructions.push_back(encode_eor_reg(3, 4, 5, 0, 0, true));
    
    /* with shift */
    instructions.push_back(encode_and_reg(0, 1, 2, 0, 4, false, false));
    instructions.push_back(encode_orr_reg(3, 4, 5, 1, 8, true)); 
    instructions.push_back(encode_eor_reg(6, 7, 8, 2, 16, false));
    instructions.push_back(encode_eor_reg(9, 10, 11, 3, 24, true));
    
    /* negated ver */
    instructions.push_back(encode_bic_reg(0, 1, 2, 0, 0, false, false));
    instructions.push_back(encode_bic_reg(3, 4, 5, 0, 0, true, true)); 
    
    instructions.push_back(encode_orn_reg(0, 1, 2, 0, 0, false));
    instructions.push_back(encode_orn_reg(3, 4, 5, 1, 8, true));
    
    instructions.push_back(encode_eon_reg(0, 1, 2, 0, 0, false));
    instructions.push_back(encode_eon_reg(3, 4, 5, 2, 16, true));
    
    /* the immediate may or may not match which is expected as the imm used can be invalid */
    /* some test values are taken from: https://gist.github.com/dinfuehr/51a01ac58c0b23e4de9aac313ed6a06a */
    instructions.push_back(encode_and_imm(0, 1, 0x5555555555555555, true, false)); /* alternating 0101 */
    instructions.push_back(encode_and_imm(2, 3, 0xAAAAAAAAAAAAAAAA, true, true));   /* alternating 1010 */
    
    instructions.push_back(encode_orr_imm(0, 1, 0x3333333333333333, false));
    instructions.push_back(encode_orr_imm(2, 3, 0xCCCCCCCCCCCCCCCC, true)); /* repeating 1100 */
    
    instructions.push_back(encode_eor_imm(0, 1, 0x0F0F0F0F0F0F0F0F, false)); /* repeating 00001111 */
    instructions.push_back(encode_eor_imm(2, 3, 0xF0F0F0F0F0F0F0F0, true)); 

    /* some original values i computed */
    instructions.push_back(encode_and_imm(4, 5, 0x3F, false, false));  /* 0b111111 */
    instructions.push_back(encode_orr_imm(4, 5, 0xF0, false)); /* 0xF0 = 0b11110000 */

    write_binary_file("logical_test.bin", instructions);
}

void test_memory()
{
    std::vector<uint32_t> instructions;
    
    /* load imm */
    instructions.push_back(encode_ldr_imm(0, 1, 0, 0)); 
    instructions.push_back(encode_ldr_imm(2, 3, 16, 1)); 
    instructions.push_back(encode_ldr_imm(4, 5, 64, 2));
    instructions.push_back(encode_ldr_imm(6, 7, 128, 3));
    
    /* stre imm */
    instructions.push_back(encode_str_imm(0, 1, 0, 0));
    instructions.push_back(encode_str_imm(2, 3, 16, 1));
    instructions.push_back(encode_str_imm(4, 5, 64, 2));
    instructions.push_back(encode_str_imm(6, 7, 128, 3));
    
    /* load reg */
    instructions.push_back(encode_ldr_reg(0, 1, 2, 2, false, 0));
    instructions.push_back(encode_ldr_reg(3, 4, 5, 2, true, 1));
    instructions.push_back(encode_ldr_reg(6, 7, 8, 3, false, 2));
    instructions.push_back(encode_ldr_reg(9, 10, 11, 3, true, 3));
    
    /* store reg */
    instructions.push_back(encode_str_reg(0, 1, 2, 6, false, 0));
    instructions.push_back(encode_str_reg(3, 4, 5, 6, true, 1));
    instructions.push_back(encode_str_reg(6, 7, 8, 7, false, 2));
    instructions.push_back(encode_str_reg(9, 10, 11, 7, true, 3));
    
    /* load signed word */
    instructions.push_back(encode_ldrsw_imm(0, 1, 0));
    instructions.push_back(encode_ldrsw_imm(2, 3, 16));
    
    /* unscaled load store */
    instructions.push_back(encode_ldur(0, 1, 1, 0));
    instructions.push_back(encode_ldur(2, 3, -1, 1));
    instructions.push_back(encode_ldur(4, 5, 255, 2));
    instructions.push_back(encode_ldur(6, 7, -256, 3));
    
    instructions.push_back(encode_stur(0, 1, 1, 0)); 
    instructions.push_back(encode_stur(2, 3, -1, 1));
    instructions.push_back(encode_stur(4, 5, 255, 2));
    instructions.push_back(encode_stur(6, 7, -256, 3));
    

    /* pair load str */
    instructions.push_back(encode_ldp(0, 1, 2, 0, 0, 2));
    instructions.push_back(encode_ldp(3, 4, 5, 16, 2, 2));
    instructions.push_back(encode_ldp(6, 7, 8, -32, 0, 0)); /* post-index */
    instructions.push_back(encode_ldp(9, 10, 11, 48, 2, 3)); /* pre-index */
    
    instructions.push_back(encode_stp(0, 1, 2, 0, 0, 2));
    instructions.push_back(encode_stp(3, 4, 5, 16, 2, 2));
    instructions.push_back(encode_stp(6, 7, 8, -32, 0, 0)); /* post-index */
    instructions.push_back(encode_stp(9, 10, 11, 48, 2, 3));  /* pre-index */

    write_binary_file("memory_test.bin", instructions);
}

void test_shift()
{
    std::vector<uint32_t> instructions;
    
    /* reg-based shift left + right */
    instructions.push_back(encode_lslv(0, 1, 2, false)); 
    instructions.push_back(encode_lslv(3, 4, 5, true)); 
    
    instructions.push_back(encode_lsrv(0, 1, 2, false));
    instructions.push_back(encode_lsrv(3, 4, 5, true));
    
    instructions.push_back(encode_asrv(0, 1, 2, false));
    instructions.push_back(encode_asrv(3, 4, 5, true));
    
    instructions.push_back(encode_rorv(0, 1, 2, false));
    instructions.push_back(encode_rorv(3, 4, 5, true));
    
    /* immediate shifts*/
    instructions.push_back(encode_lsl_imm(0, 1, 0, false));
    instructions.push_back(encode_lsl_imm(2, 3, 4, false));
    instructions.push_back(encode_lsl_imm(4, 5, 16, false));
    instructions.push_back(encode_lsl_imm(6, 7, 31, false));
    instructions.push_back(encode_lsl_imm(8, 9, 0, true));
    instructions.push_back(encode_lsl_imm(10, 11, 8, true));
    instructions.push_back(encode_lsl_imm(12, 13, 32, true));
    instructions.push_back(encode_lsl_imm(14, 15, 63, true));
    
    instructions.push_back(encode_lsr_imm(0, 1, 0, false));
    instructions.push_back(encode_lsr_imm(2, 3, 4, false));
    instructions.push_back(encode_lsr_imm(4, 5, 16, false));
    instructions.push_back(encode_lsr_imm(6, 7, 31, false));
    instructions.push_back(encode_lsr_imm(8, 9, 0, true));
    instructions.push_back(encode_lsr_imm(10, 11, 8, true));
    instructions.push_back(encode_lsr_imm(12, 13, 32, true));
    instructions.push_back(encode_lsr_imm(14, 15, 63, true));
    
    instructions.push_back(encode_asr_imm(0, 1, 0, false));
    instructions.push_back(encode_asr_imm(2, 3, 4, false));
    instructions.push_back(encode_asr_imm(4, 5, 16, false));
    instructions.push_back(encode_asr_imm(6, 7, 31, false));
    instructions.push_back(encode_asr_imm(8, 9, 0, true));
    instructions.push_back(encode_asr_imm(10, 11, 8, true));
    instructions.push_back(encode_asr_imm(12, 13, 32, true));
    instructions.push_back(encode_asr_imm(14, 15, 63, true));
    
    instructions.push_back(encode_ror_imm(0, 1, 0, false));
    instructions.push_back(encode_ror_imm(2, 3, 4, false));
    instructions.push_back(encode_ror_imm(4, 5, 16, false));
    instructions.push_back(encode_ror_imm(6, 7, 31, false));
    instructions.push_back(encode_ror_imm(8, 9, 0, true));
    instructions.push_back(encode_ror_imm(10, 11, 8, true));
    instructions.push_back(encode_ror_imm(12, 13, 32, true));
    instructions.push_back(encode_ror_imm(14, 15, 63, true)); 

    write_binary_file("shift_test.bin", instructions);
}

void test_system()
{
    std::vector<uint32_t> instructions;
    
    /* sys reg access */
    instructions.push_back(encode_msr_reg(0, 3, 4, 5, 1));
    instructions.push_back(encode_mrs(1, 3, 13, 0, 3));
    
    /* mbar */
    instructions.push_back(encode_dsb(15));
    instructions.push_back(encode_dsb(14));
    instructions.push_back(encode_dsb(11));
    
    instructions.push_back(encode_dmb(15));
    instructions.push_back(encode_dmb(13));
    
    instructions.push_back(encode_isb(15));
    
    /* data cache */
    instructions.push_back(encode_dc(0, 0, 7, 1));
    instructions.push_back(encode_dc(1, 0, 10, 2));
    instructions.push_back(encode_dc(2, 3, 4, 1));
    
    /* exception generation */
    instructions.push_back(encode_svc(0));
    instructions.push_back(encode_svc(42));
    
    instructions.push_back(encode_hvc(0));
    instructions.push_back(encode_hvc(42));
    
    instructions.push_back(encode_smc(0));
    instructions.push_back(encode_smc(42));
    
    /* HINT instructions */
    instructions.push_back(encode_nop()); 
    instructions.push_back(encode_wfe()); 
    instructions.push_back(encode_wfi()); 
    instructions.push_back(encode_sev()); 
    
    /* MSR immediate (directly to system registers) */
    instructions.push_back(encode_msr_imm(0, 4, 2, 0));
    
    /* breakpoint */
    instructions.push_back(encode_brk(0));
    instructions.push_back(encode_brk(0xFFFF));

    write_binary_file("system_test.bin", instructions);
}

void test_fp()
{
    std::vector<uint32_t> instructions;
    
    /* fp compare */
    instructions.push_back(encode_fcmp(0, 1, FP32));
    instructions.push_back(encode_fcmp(2, 3, FP64));
    instructions.push_back(encode_fcmp(4, 5, FP16));
    
    instructions.push_back(encode_fcmp_zero(0, FP32));
    instructions.push_back(encode_fcmp_zero(1, FP64));
    instructions.push_back(encode_fcmp_zero(2, FP16));
    
    /* fp2int */
    /* first parameter: signed/unsigned, Second: 32-bit/64-bit result */
    instructions.push_back(encode_fcvt_to_int(0, 1, true, false, 0, FP32)); 
    instructions.push_back(encode_fcvt_to_int(2, 3, true, true, 0, FP32));
    instructions.push_back(encode_fcvt_to_int(4, 5, false, false, 0, FP32));
    instructions.push_back(encode_fcvt_to_int(6, 7, false, true, 0, FP32));
    
    instructions.push_back(encode_fcvt_to_int(0, 1, true, false, 0, FP64));
    instructions.push_back(encode_fcvt_to_int(2, 3, true, true, 0, FP64));
    instructions.push_back(encode_fcvt_to_int(4, 5, false, false, 0, FP64));
    instructions.push_back(encode_fcvt_to_int(6, 7, false, true, 0, FP64));
    
    instructions.push_back(encode_fcvt_from_int(0, 1, true, false, FP32));
    instructions.push_back(encode_fcvt_from_int(2, 3, true, true, FP32));
    instructions.push_back(encode_fcvt_from_int(4, 5, false, false, FP32));
    instructions.push_back(encode_fcvt_from_int(6, 7, false, true, FP32));
    
    instructions.push_back(encode_fcvt_from_int(0, 1, true, false, FP64));
    instructions.push_back(encode_fcvt_from_int(2, 3, true, true, FP64));
    instructions.push_back(encode_fcvt_from_int(4, 5, false, false, FP64));
    instructions.push_back(encode_fcvt_from_int(6, 7, false, true, FP64)); 
    
    /* instructions.push_back(encode_fmov_imm(0, 1.0f, FP32)); */
    
    instructions.push_back(encode_fabs(0, 1, FP32));
    instructions.push_back(encode_fabs(2, 3, FP64));
    instructions.push_back(encode_fabs(4, 5, FP16));
    
    instructions.push_back(encode_fneg(0, 1, FP32));
    instructions.push_back(encode_fneg(2, 3, FP64));
    instructions.push_back(encode_fneg(4, 5, FP16));
    
    instructions.push_back(encode_fsqrt(0, 1, FP32)); 
    instructions.push_back(encode_fsqrt(2, 3, FP64));
    instructions.push_back(encode_fsqrt(4, 5, FP16));
    
    instructions.push_back(encode_fadd(0, 1, 2, FP32)); 
    instructions.push_back(encode_fadd(3, 4, 5, FP64));
    instructions.push_back(encode_fadd(6, 7, 8, FP16));
    
    instructions.push_back(encode_fsub(0, 1, 2, FP32));
    instructions.push_back(encode_fsub(3, 4, 5, FP64));
    instructions.push_back(encode_fsub(6, 7, 8, FP16));
    
    instructions.push_back(encode_fmul(0, 1, 2, FP32));
    instructions.push_back(encode_fmul(3, 4, 5, FP64));
    instructions.push_back(encode_fmul(6, 7, 8, FP16));
    
    instructions.push_back(encode_fdiv(0, 1, 2, FP32));
    instructions.push_back(encode_fdiv(3, 4, 5, FP64));
    instructions.push_back(encode_fdiv(6, 7, 8, FP16));

    write_binary_file("fp_test.bin", instructions);
}

void test_simd()
{
    std::vector<uint32_t> instructions;
    
    instructions.push_back(encode_movi(0, 0xFF, SIMD_8B));
    instructions.push_back(encode_movi(1, 0xFF, SIMD_16B));
    instructions.push_back(encode_movi(2, 0xFF, SIMD_4H));
    instructions.push_back(encode_movi(3, 0xFF00, SIMD_4H));
    instructions.push_back(encode_movi(4, 0xFF, SIMD_8H));
    instructions.push_back(encode_movi(5, 0xFF, SIMD_2S));
    instructions.push_back(encode_movi(6, 0xFF00, SIMD_2S));
    instructions.push_back(encode_movi(7, 0xFF0000, SIMD_2S));
    instructions.push_back(encode_movi(8, 0xFF000000, SIMD_2S)); 
    
    instructions.push_back(encode_add_simd(0, 1, 2, SIMD_8B));
    instructions.push_back(encode_add_simd(3, 4, 5, SIMD_16B));
    instructions.push_back(encode_add_simd(6, 7, 8, SIMD_4H));
    instructions.push_back(encode_add_simd(9, 10, 11, SIMD_8H));
    instructions.push_back(encode_add_simd(12, 13, 14, SIMD_2S));
    instructions.push_back(encode_add_simd(15, 16, 17, SIMD_4S));
    instructions.push_back(encode_add_simd(18, 19, 20, SIMD_2D));
    
    instructions.push_back(encode_sub_simd(0, 1, 2, SIMD_8B));
    instructions.push_back(encode_sub_simd(3, 4, 5, SIMD_16B));
    instructions.push_back(encode_sub_simd(6, 7, 8, SIMD_4H));
    instructions.push_back(encode_sub_simd(9, 10, 11, SIMD_8H));
    instructions.push_back(encode_sub_simd(12, 13, 14, SIMD_2S));
    instructions.push_back(encode_sub_simd(15, 16, 17, SIMD_4S));
    instructions.push_back(encode_sub_simd(18, 19, 20, SIMD_2D));
    
    instructions.push_back(encode_mul_simd(0, 1, 2, SIMD_8B));
    instructions.push_back(encode_mul_simd(3, 4, 5, SIMD_16B));
    instructions.push_back(encode_mul_simd(6, 7, 8, SIMD_4H));
    instructions.push_back(encode_mul_simd(9, 10, 11, SIMD_8H));
    instructions.push_back(encode_mul_simd(12, 13, 14, SIMD_2S));
    instructions.push_back(encode_mul_simd(15, 16, 17, SIMD_4S));
    
    instructions.push_back(encode_and_simd(0, 1, 2, SIMD_8B));
    instructions.push_back(encode_and_simd(3, 4, 5, SIMD_16B));
    
    instructions.push_back(encode_orr_simd(0, 1, 2, SIMD_8B));
    instructions.push_back(encode_orr_simd(3, 4, 5, SIMD_16B));
    
    instructions.push_back(encode_tbl(0, 1, 2, 0, false));
    instructions.push_back(encode_tbl(3, 4, 5, 1, true));
    instructions.push_back(encode_tbl(6, 7, 8, 2, false));
    instructions.push_back(encode_tbl(9, 10, 11, 3, true));
    
    instructions.push_back(encode_dup_elem(0, 1, 0, SIMD_8B));
    instructions.push_back(encode_dup_elem(2, 3, 1, SIMD_16B));
    instructions.push_back(encode_dup_elem(4, 5, 2, SIMD_4H));
    instructions.push_back(encode_dup_elem(6, 7, 3, SIMD_8H));
    instructions.push_back(encode_dup_elem(8, 9, 0, SIMD_2S));
    instructions.push_back(encode_dup_elem(10, 11, 1, SIMD_4S));
    instructions.push_back(encode_dup_elem(12, 13, 0, SIMD_2D));
    
    instructions.push_back(encode_dup_gen(0, 1, SIMD_8B)); 
    instructions.push_back(encode_dup_gen(2, 3, SIMD_16B));
    instructions.push_back(encode_dup_gen(4, 5, SIMD_4H));
    instructions.push_back(encode_dup_gen(6, 7, SIMD_8H)); 
    instructions.push_back(encode_dup_gen(8, 9, SIMD_2S));
    instructions.push_back(encode_dup_gen(10, 11, SIMD_4S));
    instructions.push_back(encode_dup_gen(12, 13, SIMD_2D));

    write_binary_file("simd_test.bin", instructions);
}

void test_cond()
{
    std::vector<uint32_t> instructions;
    
    /* cond select */
    instructions.push_back(encode_csel(0, 1, 2, 0x0, true));
    instructions.push_back(encode_csel(3, 4, 5, 0xB, false));
    instructions.push_back(encode_csinc(6, 7, 8, 0x1, true));
    instructions.push_back(encode_csinv(9, 10, 11, 0x2, false));
    instructions.push_back(encode_csneg(12, 13, 14, 0x3, true));
    
    /* pseudo-instructions */
    instructions.push_back(encode_cinc(15, 16, 0x4, true));
    instructions.push_back(encode_cinv(17, 18, 0x5, false));
    instructions.push_back(encode_cneg(19, 20, 0x6, true));
    instructions.push_back(encode_cset(21, 0x7, false));
    instructions.push_back(encode_csetm(22, 0x8, true));

    write_binary_file("cond_test.bin", instructions);
}

void test_mul()
{
    std::vector<uint32_t> instructions;
    
    /* mul accumulate */
    instructions.push_back(encode_mul(0, 1, 2, true));
    instructions.push_back(encode_mul(3, 4, 5, false));
    instructions.push_back(encode_madd(6, 7, 8, 9, true));
    instructions.push_back(encode_madd(10, 11, 12, 13, false));
    instructions.push_back(encode_msub(14, 15, 16, 17, true));
    instructions.push_back(encode_mneg(18, 19, 20, false));
    
    /* long mul */
    instructions.push_back(encode_smaddl(21, 22, 23, 24));
    instructions.push_back(encode_smsubl(25, 26, 27, 28));
    instructions.push_back(encode_smull(29, 0, 1));
    instructions.push_back(encode_smnegl(2, 3, 4));
    instructions.push_back(encode_umaddl(5, 6, 7, 8));
    instructions.push_back(encode_umsubl(9, 10, 11, 12));
    instructions.push_back(encode_umull(13, 14, 15));
    instructions.push_back(encode_umnegl(16, 17, 18));

    write_binary_file("mul_test.bin", instructions);
}

void test_bitmanip()
{
    std::vector<uint32_t> instructions;
    
    instructions.push_back(encode_rbit(0, 1, true));
    instructions.push_back(encode_rbit(2, 3, false)); 
    instructions.push_back(encode_rev16(4, 5, true)); 
    instructions.push_back(encode_rev16(6, 7, false));
    instructions.push_back(encode_rev32(8, 9, true));
    instructions.push_back(encode_rev(10, 11, true));
    instructions.push_back(encode_rev(12, 13, false));

    write_binary_file("bitmanip_test.bin", instructions);
}

int main()
{
	test_arithmetic();
    test_atomic();
    test_bitfield();
    test_branch();
    test_cmp();
    test_data_move();
    test_logical();
    test_memory();
    test_shift();
    test_system();
    test_fp();
    test_simd();
    test_bitmanip();
    test_cond();
    test_mul();
	return 0;
}