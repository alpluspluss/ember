#include <sparkle/target/aarch64/encoding/memory.hpp>

namespace sprk::aarch64
{
	uint32_t encode_ldr_imm(const int rt, const int rn, const int imm12, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		uint32_t opc = 1; /* LDR=1 for the opc field */

		/* scale the immediate by the data size (byte=1, halfword=2, etc.) */
		uint32_t scaled_imm = imm12 << size;

		/* check for valid immediate range (after scaling) */
		uint32_t max_imm = 0xFFF << size;
		if (scaled_imm > max_imm)
			return 0; /* error: immediate too large after scaling */

		return (size << 30) |     /* size field */
		       (0b111001 << 24) | /* opcode fixed pattern */
		       (opc << 22) |      /* operation: 1 for LDR */
		       (imm12 << 10) |    /* 12-bit immediate */
		       (rn << 5) |        /* base register */
		       rt;                /* target register */
	}

	uint32_t encode_str_imm(const int rt, const int rn, const int imm12, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t opc = 0; /* STR=0 for the opc field */

		/* scale the immediate by the data size (byte=1, halfword=2, etc.) */
		const uint32_t scaled_imm = imm12 << size;

		/* check for valid immediate range (after scaling) */
		if (const uint32_t max_imm = 0xFFF << size;
			scaled_imm > max_imm)
			return 0; /* error: immediate too large after scaling */

		return (size << 30) |     /* size field */
		       (0b111001 << 24) | /* opcode fixed pattern */
		       (opc << 22) |      /* operation: 0 for STR */
		       (imm12 << 10) |    /* 12-bit immediate */
		       (rn << 5) |        /* base register */
		       rt;                /* source register */
	}

	uint32_t encode_ldr_reg(const int rt, const int rn, int rm, const uint32_t option, const bool extend_scale,
	                        const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t opc = 1; /* LDR=1 for the opc field */

		/* option: 010=UXTW (extend 32-bit to 64-bit unsigned) */
		/*         011=LSL  (shifted register, no extension) */
		/*         110=SXTW (extend 32-bit to 64-bit signed) */
		/*         111=SXTX (extend 64-bit to 64-bit signed, i.e., no extension) */

		/* extend_scale: If true, index is multiplied by the data size */
		const uint32_t S = extend_scale ? 1 : 0;

		return (size << 30) |     /* size field */
		       (0b111000 << 24) | /* opcode fixed pattern */
		       (opc << 22) |      /* operation: 1 for LDR */
		       (1 << 21) |        /* fixed bit for register variant */
		       (rm << 16) |       /* index register */
		       (option << 13) |   /* extension/shift option */
		       (S << 12) |        /* scale bit */
		       (0b10 << 10) |     /* fixed bits */
		       (rn << 5) |        /* base register */
		       rt;                /* target register */
	}

	uint32_t encode_ldrsw_imm(const int rt, const int rn, const int imm12)
	{
		constexpr uint32_t size = 2; /* Word size */
		constexpr uint32_t opc = 2;  /* Sign extend to 64-bit */

		/* scale the immediate by the data size (word=4) */
		const uint32_t scaled_imm = imm12 << 2;

		/* check for valid immediate range (after scaling) */
		if (uint32_t max_imm = 0xFFF << 2;
			scaled_imm > max_imm)
			return 0; /* error: immediate too large after scaling */

		return (size << 30) |     /* size field: 2 for word */
		       (0b111001 << 24) | /* opcode fixed pattern */
		       (opc << 22) |      /* operation: 2 for LDRSW */
		       (imm12 << 10) |    /* 12-bit immediate */
		       (rn << 5) |        /* base register */
		       rt;                /* target register */
	}

	uint32_t encode_str_reg(const int rt, const int rn, const int rm, const uint32_t option, const bool extend_scale,
	                        const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t opc = 0; /* STR=0 for the opc field */

		/* option: 010=UXTW (extend 32-bit to 64-bit unsigned) */
		/*         011=LSL  (shifted register, no extension) */
		/*         110=SXTW (extend 32-bit to 64-bit signed) */
		/*         111=SXTX (extend 64-bit to 64-bit signed, i.e., no extension) */

		/* extend_scale: If true, index is multiplied by the data size */
		const uint32_t S = extend_scale ? 1 : 0;

		return (size << 30) |     /* size field */
		       (0b111000 << 24) | /* opcode fixed pattern */
		       (opc << 22) |      /* operation: 0 for STR */
		       (1 << 21) |        /* fixed bit for register variant */
		       (rm << 16) |       /* index register */
		       (option << 13) |   /* extension/shift option */
		       (S << 12) |        /* scale bit */
		       (0b10 << 10) |     /* fixed bits */
		       (rn << 5) |        /* base register */
		       rt;                /* source register */
	}

	uint32_t encode_ldur(const int rt, const int rn, const int offset, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		uint32_t opc = 1; /* LDR=1 for the opc field */

		/* LDUR uses a 9-bit signed offset, unscaled */
		if (offset < -256 || offset > 255)
			return 0; /* error: offset out of range */

		return (size << 30) |             /* size field */
		       (0b111000 << 24) |         /* opcode fixed pattern */
		       (opc << 22) |              /* operation: 1 for LDUR */
		       (0 << 21) |                /* fixed bit for unscaled offset */
		       ((offset & 0x1FF) << 12) | /* 9-bit signed offset */
		       (0b00 << 10) |             /* fixed bits for LDUR */
		       (rn << 5) |                /* base register */
		       rt;                        /* target register */
	}

	uint32_t encode_stur(const int rt, const int rn, const int offset, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		uint32_t opc = 0; /* STR=0 for the opc field */

		/* STUR uses a 9-bit signed offset, unscaled */
		if (offset < -256 || offset > 255)
			return 0; /* error: offset out of range */

		return (size << 30) |             /* size field */
		       (0b111000 << 24) |         /* opcode fixed pattern */
		       (opc << 22) |              /* operation: 0 for STUR */
		       (0 << 21) |                /* fixed bit for unscaled offset */
		       ((offset & 0x1FF) << 12) | /* 9-bit signed offset */
		       (0b00 << 10) |             /* fixed bits for STUR */
		       (rn << 5) |                /* base register */
		       rt;                        /* source register */
	}

	uint32_t encode_ldp(const int rt1, const int rt2, const int rn, const int imm7, const uint32_t size,
	                    const uint32_t option)
	{
		/* size: 0=32-bit, 1=FP32, 2=64-bit, 3=FP64, etc. */
		uint32_t opc;
		if (size <= 1)
		{
			opc = 0; /* 32-bit registers or FP32 */
		}
		else
		{
			opc = 2; /* 64-bit registers or FP64 */
		}

		const uint32_t V = (size == 1 || size == 3) ? 1 : 0; /* V=1 for FP registers */
		constexpr uint32_t L = 1;                            /* L=1 for loading */

		/* the immediate is a 7-bit signed value, scaled by the register size */
		if (imm7 < -64 || imm7 > 63)
			return 0; /* error: immediate out of range */

		/* option: 0=normal (post-index), 2=normal (signed offset), 3=normal (pre-index) */
		const uint32_t op2 = option & 3; /* Extract the option */

		return (opc << 30) |           /* size-related field */
		       (0b10100 << 25) |       /* opcode fixed pattern */
		       (V << 26) |             /* V bit for register type */
		       (op2 << 23) |           /* addressing mode */
		       (L << 22) |             /* L bit for load */
		       ((imm7 & 0x7F) << 15) | /* 7-bit signed immediate */
		       (rt2 << 10) |           /* second target register */
		       (rn << 5) |             /* base register */
		       rt1;                    /* first target register */
	}

	uint32_t encode_stp(const int rt1, const int rt2, const int rn, const int imm7, const uint32_t size,
	                    const uint32_t option)
	{
		/* size: 0=32-bit, 1=FP32, 2=64-bit, 3=FP64, etc. */
		const uint32_t opc = size <= 1 ? 0 : 2;

		const uint32_t V = (size == 1 || size == 3) ? 1 : 0; /* V=1 for FP registers */
		constexpr uint32_t L = 0;                            /* L=0 for storing */

		/* the immediate is a 7-bit signed value, scaled by the register size */
		if (imm7 < -64 || imm7 > 63)
			return 0; /* error: immediate out of range */

		/* option: 0=normal (post-index), 2=normal (signed offset), 3=normal (pre-index) */
		const uint32_t op2 = option & 3; /* Extract the option */

		return (opc << 30) |           /* size-related field */
		       (0b10100 << 25) |       /* opcode fixed pattern */
		       (V << 26) |             /* V bit for register type */
		       (op2 << 23) |           /* addressing mode */
		       (L << 22) |             /* L bit for store */
		       ((imm7 & 0x7F) << 15) | /* 7-bit signed immediate */
		       (rt2 << 10) |           /* second source register */
		       (rn << 5) |             /* base register */
		       rt1;                    /* first source register */
	}
}
