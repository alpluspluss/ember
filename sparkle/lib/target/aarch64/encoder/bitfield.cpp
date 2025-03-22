#include <sparkle/target/aarch64/encoding/bitfield.hpp>

namespace sprk::aarch64
{
	uint32_t encode_bfm(const int rd, const int rn, const uint32_t immr, const uint32_t imms, const bool is64bit,
	                    const uint32_t opc)
	{
		/* opc: 0=SBFM (signed), 1=BFM (insert), 2=UBFM (unsigned) */
		const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
		const uint32_t N = sf;               /* N must match sf for bitfield ops */

		if (const uint32_t max_imm = is64bit ? 63 : 31;
			immr > max_imm || imms > max_imm)
			return 0; /* error: immediate out of range */

		return (sf << 31) |       /* size flag */
		       (opc << 29) |      /* operation code */
		       (0b100110 << 23) | /* opcode fixed pattern */
		       (N << 22) |        /* N bit (must match sf) */
		       (immr << 16) |     /* immr field */
		       (imms << 10) |     /* imms field */
		       (rn << 5) |        /* source register */
		       rd;                /* destination register */
	}

	uint32_t encode_bfi(const int rd, const int rn, const uint32_t lsb, const uint32_t width, const bool is64bit)
	{
		/* BFI is an alias of BFM with specific encoding */
		constexpr uint32_t opc = 1; /* BFM=1 */
		const uint32_t reg_size = is64bit ? 64 : 32;

		/* conv lsb and width to immr and imms */
		const uint32_t immr = (reg_size - lsb) % reg_size;
		const uint32_t imms = width - 1;

		/* check validity */
		if (lsb + width > reg_size)
			return 0; /* error: field extends beyond register */

		return encode_bfm(rd, rn, immr, imms, is64bit, opc);
	}

	uint32_t encode_bfxil(const int rd, const int rn, const uint32_t lsb, const uint32_t width, const bool is64bit)
	{
		/* BFXIL is an alias of BFM with specific encoding */
		constexpr uint32_t opc = 1; /* BFM=1 */

		/* convert lsb and width to immr and imms */
		const uint32_t immr = lsb;
		const uint32_t imms = lsb + width - 1;

		/* check validity */
		if (const uint32_t reg_size = is64bit ? 64 : 32;
			imms >= reg_size)
			return 0; /* error: field extends beyond register */
		return encode_bfm(rd, rn, immr, imms, is64bit, opc);
	}

	uint32_t encode_sbfx(const int rd, const int rn, const uint32_t lsb, const uint32_t width, const bool is64bit)
	{
		/* SBFX is an alias of SBFM with specific encoding */
		constexpr uint32_t opc = 0; /* SBFM=0 */

		/* convert lsb and width to immr and imms */
		const uint32_t immr = lsb;
		const uint32_t imms = lsb + width - 1;

		/* check validity */
		if (const uint32_t reg_size = is64bit ? 64 : 32;
			imms >= reg_size)
			return 0; /* error: field extends beyond register */

		return encode_bfm(rd, rn, immr, imms, is64bit, opc);
	}

	uint32_t encode_ubfx(const int rd, const int rn, const uint32_t lsb, const uint32_t width, const bool is64bit)
	{
		/* UBFX is an alias of UBFM with specific encoding */
		constexpr uint32_t opc = 2; /* UBFM=2 */

		/* convert lsb and width to immr and imms */
		const uint32_t immr = lsb;
		const uint32_t imms = lsb + width - 1;

		if (const uint32_t reg_size = is64bit ? 64 : 32;
			imms >= reg_size)
			return 0; /* error: field extends beyond register */

		return encode_bfm(rd, rn, immr, imms, is64bit, opc);
	}

	uint32_t encode_asr_imm_alias(const int rd, const int rn, const uint32_t shift, const bool is64bit)
	{
		/* ASR is an alias of SBFM with specific encoding */
		constexpr uint32_t opc = 0; /* SBFM=0 */
		const uint32_t reg_size = is64bit ? 64 : 32;

		/* for ASR, immr = shift, imms = reg_size - 1 */
		const uint32_t immr = shift;
		const uint32_t imms = reg_size - 1;

		if (shift >= reg_size)
			return 0; /* error: shift too large */
		return encode_bfm(rd, rn, immr, imms, is64bit, opc);
	}

	uint32_t encode_extr(const int rd, const int rn, const int rm, const uint32_t lsb, const bool is64bit)
	{
		const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
		const uint32_t N = sf;               /* N must match sf for extract */
		const uint32_t imms = lsb;           /* lsb position for extract */

		if (const uint32_t reg_size = is64bit ? 64 : 32;
			lsb >= reg_size)
			return 0; /* error: lsb out of range */

		return (sf << 31) |         /* size flag */
		       (0b00100111 << 23) | /* opcode fixed pattern */
		       (N << 22) |          /* N bit (must match sf) */
		       (0 << 21) |          /* o0 bit (always 0 for EXTR) */
		       (rm << 16) |         /* second source register */
		       (imms << 10) |       /* imms field (lsb position) */
		       (rn << 5) |          /* first source register */
		       rd;                  /* destination register */
	}
}
