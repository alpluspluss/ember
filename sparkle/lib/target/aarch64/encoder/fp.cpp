#include <sparkle/target/aarch64/common.hpp>
#include <sparkle/target/aarch64/encoding/fp.hpp>

namespace sprk::aarch64
{
	uint32_t encode_fcmp(const int rn, const int rm, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16:
				ftype = 3;
				break;
			case FP32:
				ftype = 0;
				break;
			case FP64:
				ftype = 1;
				break;
			default:
				return 0;
		}

		uint32_t opc = 0;           /* opcode for FCMP */
		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (rm << 16) |         /* second source register */
		       (0b001000 << 10) |   /* fixed pattern for FCMP */
		       (rn << 5) |          /* first source register */
		       (opc << 3) |         /* operation variant */
		       (0b000);             /* fixed bits */
	}

	uint32_t encode_fcmp_zero(const int rn, const uint32_t size)
	{
		uint32_t ftype;

		/* Convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* Half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* Single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* Double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* Error: invalid size */
		}

		uint32_t opc = 1; /* opcode for FCMP with zero */
		uint32_t rm = 0;  /* special encoding for zero */

		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (rm << 16) |         /* zero register (special encoding) */
		       (0b001000 << 10) |   /* fixed pattern for FCMP */
		       (rn << 5) |          /* register to compare */
		       (opc << 3) |         /* operation variant */
		       (0b000);             /* fixed bits */
	}

	uint32_t encode_fcvt_to_int(const int rd, int rn, bool is_signed, bool is64bit, uint32_t rounding, uint32_t size)
	{
		const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) integer result */
		constexpr uint32_t S = 0;                /* Fixed bit */
		uint32_t ftype;
		const uint32_t opcode = is_signed ? 0 : 1; /* 0=signed, 1=unsigned */

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* Error: invalid size */
		}

		/* rounding mode: 0=TIEEVEN, 1=POSINF, 2=NEGINF, 3=ZERO */
		if (rounding > 3)
			return 0; /* Error: invalid rounding mode */

		const uint32_t rmode = rounding;
		return (sf << 31) |       /* size flag for integer output */
		       (S << 29) |        /* fixed bit */
		       (0b11110 << 24) |  /* opcode fixed pattern */
		       (ftype << 22) |    /* floating-point type */
		       (1 << 21) |        /* fixed bit */
		       (rmode << 19) |    /* rounding mode */
		       (opcode << 16) |   /* operation variant (signed/unsigned) */
		       (0b000000 << 10) | /* fixed pattern */
		       (rn << 5) |        /* source floating-point register */
		       rd;                /* destination integer register */
	}

	uint32_t encode_fcvt_from_int(const int rd, int rn, bool is_signed, bool is64bit, uint32_t size)
	{
		uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) integer source */
		uint32_t S = 0;                /* Fixed bit */
		uint32_t ftype;
		uint32_t rmode = 0;                  /* Always TIEEVEN for this direction */
		uint32_t opcode = is_signed ? 0 : 1; /* 0=signed, 1=unsigned */

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		/* For integer to float conversion, the opcode needs to be shifted */
		opcode += 2;

		return (sf << 31) |       /* size flag for integer input */
		       (S << 29) |        /* fixed bit */
		       (0b11110 << 24) |  /* opcode fixed pattern */
		       (ftype << 22) |    /* floating-point type */
		       (1 << 21) |        /* fixed bit */
		       (rmode << 19) |    /* rounding mode (always 0 for this direction) */
		       (opcode << 16) |   /* operation variant */
		       (0b000000 << 10) | /* fixed pattern */
		       (rn << 5) |        /* source integer register */
		       rd;                /* destination floating-point register */
	}

	uint32_t encode_fabs(const int rd, const int rn, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* Error: invalid size */
		}

		constexpr uint32_t opc = 1; /* opcode for FABS */

		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (0b00 << 19) |       /* fixed bits */
		       (opc << 15) |        /* operation code */
		       (0b10000 << 10) |    /* fixed pattern */
		       (rn << 5) |          /* source register */
		       rd;                  /* destination register */
	}

	uint32_t encode_fneg(const int rd, const int rn, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		uint32_t opc = 2; /* opcode for FNEG */

		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (0b00 << 19) |       /* fixed bits */
		       (opc << 15) |        /* operation code */
		       (0b10000 << 10) |    /* fixed pattern */
		       (rn << 5) |          /* source register */
		       rd;                  /* destination register */
	}

	uint32_t encode_fsqrt(const int rd, const int rn, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		constexpr uint32_t opc = 3; /* opcode for FSQRT */

		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (0b00 << 19) |       /* fixed bits */
		       (opc << 15) |        /* operation code */
		       (0b10000 << 10) |    /* fixed pattern */
		       (rn << 5) |          /* source register */
		       rd;                  /* destination register */
	}

	uint32_t encode_fadd(const int rd, const int rn, const int rm, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		constexpr uint32_t opc = 2; /* opcode for FADD */

		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (rm << 16) |         /* second source register */
		       (opc << 12) |        /* operation code */
		       (0b10 << 10) |       /* fixed bits */
		       (rn << 5) |          /* first source register */
		       rd;                  /* destination register */
	}

	uint32_t encode_fsub(const int rd, const int rn, const int rm, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		constexpr uint32_t opc = 3; /* opcode for FSUB */

		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (rm << 16) |         /* second source register */
		       (opc << 12) |        /* operation code */
		       (0b10 << 10) |       /* fixed bits */
		       (rn << 5) |          /* first source register */
		       rd;                  /* destination register */
	}

	uint32_t encode_fmul(const int rd, const int rn, const int rm, const uint32_t size)
	{
		uint32_t ftype;

		/* convert size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		constexpr uint32_t opc = 0;     /* opcode for FMUL */
		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (rm << 16) |         /* second source register */
		       (opc << 12) |        /* operation code */
		       (0b10 << 10) |       /* fixed bits */
		       (rn << 5) |          /* first source register */
		       rd;                  /* destination register */
	}

	uint32_t encode_fdiv(const int rd, const int rn, const int rm, const uint32_t size)
	{
		uint32_t ftype;

		/* conv size (0=16-bit, 1=32-bit, 2=64-bit) to ftype field */
		switch (size)
		{
			case FP16: /* half precision (FP16) */
				ftype = 3;
				break;
			case FP32: /* single precision (FP32) */
				ftype = 0;
				break;
			case FP64: /* double precision (FP64) */
				ftype = 1;
				break;
			default:
				return 0; /* error: invalid size */
		}

		constexpr uint32_t opc = 1; /* opcode for FDIV */
		return (0b00011110 << 24) | /* opcode fixed pattern */
		       (ftype << 22) |      /* floating-point type */
		       (1 << 21) |          /* fixed bit */
		       (rm << 16) |         /* second source register */
		       (opc << 12) |        /* operation code */
		       (0b10 << 10) |       /* fixed bits */
		       (rn << 5) |          /* first source register */
		       rd;                  /* destination register */
	}
}
