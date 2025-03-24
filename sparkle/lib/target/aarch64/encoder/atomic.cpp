#include <cstdint>

namespace sprk::aarch64
{
	uint32_t encode_ldxr(const int rt, const int rn, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t L = 1;  /* L=1 for load */
		constexpr uint32_t o0 = 0; /* o0=0 for standard exclusive */

		return (size << 30) |     /* size field */
		       (0b001000 << 24) | /* opcode fixed pattern */
		       (L << 22) |        /* L bit for load */
		       (0 << 21) |        /* not pair operation */
		       (0b11111 << 16) |  /* Rs=11111 for LDXR */
		       (o0 << 15) |       /* o0=0 for LDXR */
		       (0b11111 << 10) |  /* Rt2=11111 for LDXR */
		       (rn << 5) |        /* base register */
		       rt;                /* target register */
	}

	uint32_t encode_stxr(const int rs, const int rt, const int rn, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t L = 0;  /* L=0 for store */
		constexpr uint32_t o0 = 0; /* o0=0 for standard exclusive */

		return (size << 30) |     /* size field */
		       (0b001000 << 24) | /* opcode fixed pattern */
		       (L << 22) |        /* L bit for store */
		       (0 << 21) |        /* not pair operation */
		       (rs << 16) |       /* status register */
		       (o0 << 15) |       /* o0=0 for STXR */
		       (0b11111 << 10) |  /* Rt2=11111 for STXR */
		       (rn << 5) |        /* base register */
		       rt;                /* source register */
	}

	uint32_t encode_ldaxr(const int rt, const int rn, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t L = 1;  /* L=1 for load */
		constexpr uint32_t o0 = 1; /* o0=1 for acquire exclusive */

		return (size << 30) |     /* size field */
		       (0b001000 << 24) | /* opcode fixed pattern */
		       (L << 22) |        /* L bit for load */
		       (0 << 21) |        /* not pair operation */
		       (0b11111 << 16) |  /* Rs=11111 for LDAXR */
		       (o0 << 15) |       /* o0=1 for LDAXR */
		       (0b11111 << 10) |  /* Rt2=11111 for LDAXR */
		       (rn << 5) |        /* base register */
		       rt;                /* target register */
	}

	uint32_t encode_stlxr(const int rs, const int rt, const int rn, const uint32_t size)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		constexpr uint32_t L = 0;  /* L=0 for store */
		constexpr uint32_t o0 = 1; /* o0=1 for release exclusive */

		return (size << 30) |     /* size field */
		       (0b001000 << 24) | /* opcode fixed pattern */
		       (L << 22) |        /* L bit for store */
		       (0 << 21) |        /* not pair operation */
		       (rs << 16) |       /* status register */
		       (o0 << 15) |       /* o0=1 for STLXR */
		       (0b11111 << 10) |  /* Rt2=11111 for STLXR */
		       (rn << 5) |        /* base register */
		       rt;                /* source register */
	}

	uint32_t encode_cas(const int rs, const int rt, const int rn, const uint32_t size, const bool acquire, const bool release)
	{
		/* size: 2=word, 3=doubleword only - byte & halfword not supported */
		/* see: https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/CAS--CASA--CASAL--CASL--Compare-and-swap-word-or-doubleword-in-memory-?lang=en */
		if (size != 2 && size != 3) 
			return 0;
		
		const uint32_t L = acquire ? 1 : 0;  /* L=1 for acquire semantics */
		const uint32_t o0 = release ? 1 : 0; /* o0=1 for release semantics */

		return (size << 30) |      /* size field */
			(0b0010001 << 23) |    /* opcode fixed pattern */
			(L << 22) |            /* L bit for acquire */
			(1 << 21) |            /* */
			(rs << 16) |           /* Rs register (expected value) */
			(o0 << 15) |           /* o0 bit for release semantics */
			(0b11111 << 10) |      /* Rt2=11111 for CAS */
			(rn << 5) |            /* Rn register (memory address) */
			rt;                    /* Rt register (new/result value) */
	}

	uint32_t encode_ldadd(const int rs, const int rt, const int rn, const uint32_t size, const bool acquire, const bool release)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		uint32_t A = acquire ? 1 : 0; /* A=1 for acquire semantics */
		uint32_t R = release ? 1 : 0; /* R=1 for release semantics */
		uint32_t o3 = 0;              /* o3=0 for LDADD */
		uint32_t opc = 0;             /* opc=0 for ADD */

		return (size << 30) |  /* size field */
		       (0b111 << 27) | /* fixed bits */
		       (A << 23) |     /* A bit for acquire */
		       (R << 22) |     /* R bit for release */
		       (1 << 21) |     /* fixed bit */
		       (rs << 16) |    /* source register */
		       (o3 << 15) |    /* o3=0 for standard atomic */
		       (opc << 12) |   /* opc=0 for ADD */
		       (0 << 10) |     /* fixed bits */
		       (rn << 5) |     /* base register */
		       rt;             /* target/result register */
	}

	uint32_t encode_swp(const int rs, const int rt, const int rn, const uint32_t size, const bool acquire, const bool release)
	{
		/* size: 0=byte, 1=halfword, 2=word, 3=doubleword */
		uint32_t A = acquire ? 1 : 0; /* A=1 for acquire semantics */
		uint32_t R = release ? 1 : 0; /* R=1 for release semantics */

		return (size << 30) |     /* size field */
		       (0b111000 << 24) | /* opcode fixed pattern */
		       (A << 23) |        /* A bit for acquire */
		       (R << 22) |        /* R bit for release */
		       (1 << 21) |        /* fixed bit */
		       (rs << 16) |       /* source register */
		       (1 << 15) |        /* fixed bit */
		       (0 << 14) |        /* fixed bit */
		       (0 << 13) |        /* fixed bit */
		       (0 << 12) |        /* fixed bit */
		       (0 << 10) |        /* fixed bits */
		       (rn << 5) |        /* base register */
		       rt;                /* target/result register */
	}
}
