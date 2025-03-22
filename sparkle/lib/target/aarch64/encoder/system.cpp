#include <sparkle/target/aarch64/encoding/system.hpp>

namespace sprk::aarch64
{
	/* system register move (to system register) */
	uint32_t encode_msr_imm(uint32_t op1, uint32_t CRn, uint32_t CRm,
	                        uint32_t op2)
	{
		/* check field validity */
		if (op1 > 7 || CRn > 15 || CRm > 15 || op2 > 7)
		{
			return 0; /* error: field out of range */
		}

		return (0b1101010100000 << 19) | /* opcode fixed pattern */
		       (op1 << 16) |             /* op1 field */
		       (0b100 << 13) |           /* fixed bits */
		       (CRn << 9) |              /* CRn field */
		       (CRm << 5) |              /* CRm field */
		       (op2 << 2) |              /* op2 field */
		       (0b11111);                /* fixed bits, Rt=XZR */
	}

	uint32_t encode_msr_reg(int rt, uint32_t op1, uint32_t CRn, uint32_t CRm,
	                        uint32_t op2)
	{
		/* check field validity */
		if (op1 > 7 || CRn > 15 || CRm > 15 || op2 > 7)
		{
			return 0; /* error: field out of range */
		}

		/* combine fields to form the 16-bit system register specifier */
		uint32_t sysreg = (1 << 15) |   /* fixed bit for MSR/MRS */
		                  (op1 << 12) | /* op1 field */
		                  (CRn << 8) |  /* CRn field */
		                  (CRm << 4) |  /* CRm field */
		                  op2;          /* op2 field */

		return (0b1101010100 << 22) | /* opcode fixed pattern */
		       (0 << 21) |            /* L=0 for MSR (write) */
		       (sysreg << 5) |        /* system register specifier */
		       rt;                    /* source register */
	}

	uint32_t encode_mrs(int rt, uint32_t op1, uint32_t CRn, uint32_t CRm,
	                    uint32_t op2)
	{
		/* check field validity */
		if (op1 > 7 || CRn > 15 || CRm > 15 || op2 > 7)
		{
			return 0; /* error: field out of range */
		}

		/* combine fields to form the 16-bit system register specifier */
		uint32_t sysreg = (1 << 15) |   /* fixed bit for MSR/MRS */
		                  (op1 << 12) | /* op1 field */
		                  (CRn << 8) |  /* CRn field */
		                  (CRm << 4) |  /* CRm field */
		                  op2;          /* op2 field */

		return (0b1101010100 << 22) | /* opcode fixed pattern */
		       (1 << 21) |            /* L=1 for MRS (read) */
		       (sysreg << 5) |        /* system register specifier */
		       rt;                    /* destination register */
	}
	
	uint32_t encode_dsb(uint32_t option)
	{
		/* option: barrier option (0-15) */
		/* common values: 15=SY (full system), 14=ST, 13=LD, 11=ISH, 7=NSH, 3=OSH */
		if (option > 15)
		{
			return 0; /* error: option out of range */
		}

		uint32_t CRm = option; /* barrier option goes in CRm */
		uint32_t op2 = 1;      /* op2=1 for DSB */

		return (0b1101010100000011 << 16) | /* opcode fixed pattern */
		       (0b0011 << 12) |             /* fixed bits */
		       (CRm << 8) |                 /* barrier option */
		       (op2 << 5) |                 /* operation variant */
		       (0b11111);                   /* fixed bits */
	}
	
	uint32_t encode_dmb(uint32_t option)
	{
		/* option: barrier option (0-15) */
		/* common values: 15=SY (full system), 14=ST, 13=LD, 11=ISH, 7=NSH, 3=OSH */
		if (option > 15)
		{
			return 0; /* error: option out of range */
		}

		uint32_t CRm = option; /* barrier option goes in CRm */
		uint32_t op2 = 2;      /* op2=2 for DMB */

		return (0b1101010100000011 << 16) | /* opcode fixed pattern */
		       (0b0011 << 12) |             /* fixed bits */
		       (CRm << 8) |                 /* barrier option */
		       (op2 << 5) |                 /* operation variant */
		       (0b11111);                   /* fixed bits */
	}
	
	uint32_t encode_isb(uint32_t option)
	{
		/* option: barrier option (usually 15=SY for ISB) */
		if (option > 15)
		{
			return 0; /* error: option out of range */
		}

		uint32_t CRm = option; /* barrier option goes in CRm */
		uint32_t op2 = 3;      /* op2=3 for ISB */

		return (0b1101010100000011 << 16) | /* opcode fixed pattern */
		       (0b0011 << 12) |             /* fixed bits */
		       (CRm << 8) |                 /* barrier option */
		       (op2 << 5) |                 /* operation variant */
		       (0b11111);                   /* fixed bits */
	}
	
	uint32_t encode_dc(int rt, uint32_t op1, uint32_t CRm, uint32_t op2)
	{
		/* DC (data cache) instruction */
		/* check field validity */
		if (op1 > 7 || CRm > 15 || op2 > 7)
		{
			return 0; /* error: field out of range */
		}

		/* common DC operations: */
		/* DC IVAC: op1=0, CRm=7, op2=1 (invalidate by VA to PoC) */
		/* DC ISW:  op1=0, CRm=6, op2=2 (invalidate by set/way) */
		/* DC CSW:  op1=0, CRm=10, op2=2 (clean by set/way) */
		/* DC CISW: op1=0, CRm=14, op2=2 (clean and invalidate by set/way) */
		/* DC ZVA:  op1=3, CRm=4, op2=1 (zero by VA) */
		/* DC CVAC: op1=3, CRm=10, op2=1 (clean by VA to PoC) */
		/* DC CVAU: op1=3, CRm=11, op2=1 (clean by VA to PoU) */
		/* DC CIVAC:op1=3, CRm=14, op2=1 (clean and invalidate by VA to PoC) */

		return (0b1101010100 << 22) | /* opcode fixed pattern */
		       (0 << 21) |            /* L=0 for DC operations */
		       (1 << 20) |            /* fixed bit */
		       (op1 << 17) |          /* op1 field */
		       (0b000 << 14) |        /* fixed bits */
		       (CRm << 10) |          /* CRm field */
		       (op2 << 7) |           /* op2 field */
		       (rt);                  /* address register */
	}
	
	uint32_t encode_svc(uint32_t imm16)
	{
		/* SVC (supervisor call) - triggers exception to switch to EL1 */
		if (imm16 > 0xFFFF)
		{
			return 0; /* error: immediate out of range */
		}

		return (0b11010100000 << 21) | /* opcode fixed pattern */
		       (imm16 << 5) |          /* 16-bit immediate */
		       (0b00001);              /* LL=01 for SVC */
	}
	
	uint32_t encode_hvc(uint32_t imm16)
	{
		/* HVC (hypervisor call) - triggers exception to switch to EL2 */
		if (imm16 > 0xFFFF)
		{
			return 0; /* error: immediate out of range */
		}

		return (0b11010100000 << 21) | /* opcode fixed pattern */
		       (imm16 << 5) |          /* 16-bit immediate */
		       (0b00010);              /* LL=10 for HVC */
	}

	uint32_t encode_smc(uint32_t imm16)
	{
		/* SMC (secure monitor call) - triggers exception to switch to EL3 */
		if (imm16 > 0xFFFF)
		{
			return 0; /* error: immediate out of range */
		}

		return (0b11010100000 << 21) | /* opcode fixed pattern */
		       (imm16 << 5) |          /* 16-bit immediate */
		       (0b00011);              /* LL=11 for SMC */
	}

	/* system HINT instructions */
	uint32_t encode_hint(uint32_t imm)
	{
		/* HINT instruction for various processor hints */
		/* common values: */
		/* 0=NOP, 1=YIELD, 2=WFE, 3=WFI, 4=SEV, 5=SEVL */
		if (imm > 127)
		{
			return 0; /* error: immediate out of range */
		}

		uint32_t CRm = imm >> 3;  /* upper bits go to CRm */
		uint32_t op2 = imm & 0x7; /* lower bits go to op2 */

		return (0b1101010100000011 << 16) | /* opcode fixed pattern */
		       (0b0010 << 12) |             /* fixed bits */
		       (CRm << 8) |                 /* upper bits of hint */
		       (op2 << 5) |                 /* lower bits of hint */
		       (0b11111);                   /* fixed bits */
	}
	
	uint32_t encode_wfe()
	{
		/* WFE - wait for event (special case of HINT) */
		return encode_hint(2);
	}

	uint32_t encode_wfi()
	{
		/* WFI - wait for interrupt (special case of HINT) */
		return encode_hint(3);
	}
	
	uint32_t encode_sev()
	{
		/* SEV - send event (special case of HINT) */
		return encode_hint(4);
	}
	
	uint32_t encode_nop()
	{
		/* NOP - no operation (special case of HINT) */
		return encode_hint(0);
	}
	
	uint32_t encode_brk(uint32_t imm16)
	{
		/* BRK - breakpoint (triggers a debugger exception) */
		if (imm16 > 0xFFFF)
		{
			return 0; /* error: immediate out of range */
		}

		return (0b11010100001 << 21) | /* opcode fixed pattern */
		       (imm16 << 5) |          /* 16-bit immediate */
		       (0b00000);              /* fixed bits */
	}
}
