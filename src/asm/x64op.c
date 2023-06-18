
#define OPF_REXW	0x01
#define OPF_OPSZ	0x02
#define OPF_ADDRSZ	0x04
#define OPF_OPXT	0x08
#define OPF_REXR	0x10

#define OSZ0() 0
#define OSZ1(a) a
#define OSZ2(a, b) ((a) | ((b) << 2))
#define OSZ3(a, b, c) ((a) | ((b) << 2) | ((c) << 4))
#define OSZ(v, n) (((v) >> (n * 2)) & 0b11)

#define ENC_ZO	0x00 // None
#define ENC_M	0x01 // ModR/M mod
#define ENC_I	0x03 // Immediate
#define ENC_O	0x04 // Opcode+reg
#define ENC_D	0x05 // Relative immediate
#define ENC_MI	0x10
#define ENC_MR	0x11
#define ENC_RM	0x12
#define ENC_II	0x13
#define ENC_RMI	0x14
#define ENC_OI	0x15
#define ENC_M1	0x16
#define ENC_MC	0x17

typedef
struct lt_x64_op {
	lstr_t name;
	u8 encoding;
	u8 sizes;
	u8 reg;
	u8 flags;

	u8 op_len;
	u8 opcode[4];
} lt_x64_op_t;

static
lt_x64_op_t ops[] = {
// ----- ADC -----
#define NAME CLSTRI("adc")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0x14 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x15 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0x15 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x15 } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 2, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 2, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 2, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 2, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 2, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x10 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x11 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x11 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x11 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x12 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x13 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x13 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x13 } },
#undef NAME

// ----- ADD -----
#define NAME CLSTRI("add")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0x04 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x05 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0x05 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x05 } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 0, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 0, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 0, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 0, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 0, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x00 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x01 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x01 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x01 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x02 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x03 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x03 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x03 } },
#undef NAME

// ----- AND -----
#define NAME CLSTRI("and")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,			1,{ 0x24 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x25 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,			1,{ 0x25 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x25 } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 4, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 4, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 4, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x20 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x21 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x21 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x21 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x22 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x23 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x23 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x23 } },
#undef NAME

// ----- BSF -----
#define NAME CLSTRI("bsf")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xBC } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xBC } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xBC } },
#undef NAME

// ----- BSR -----
#define NAME CLSTRI("bsr")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xBD } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xBD } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xBD } },
#undef NAME

// ----- BSWAP -----
#define NAME CLSTRI("bswap")
	{ NAME, ENC_O, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0xC8 } },
	{ NAME, ENC_O, OSZ1(SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xC8 } },
#undef NAME

// ----- BT -----
#define NAME CLSTRI("bt")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xA3 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xA3 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xA3 } },
#undef NAME

// ----- BTC -----
#define NAME CLSTRI("btc")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 7, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 7, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 7, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xBB } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xBB } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xBB } },
#undef NAME

// ----- BTR -----
#define NAME CLSTRI("btr")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 6, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 6, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 6, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xB3 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xB3 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xB3 } },
#undef NAME

// ----- BTS -----
#define NAME CLSTRI("bts")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 5, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 5, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 5, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xAB } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xAB } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xAB } },
#undef NAME

// ----- CALL -----
#define NAME CLSTRI("call")
	{ NAME, ENC_D, OSZ1(SZ_4), 0, 0,		1,{ 0xE8 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 2, OPF_OPXT,	1,{ 0xFF } },

	// TODO
#undef NAME

// ----- CBW/CWDE/CDQE -----
	{ CLSTRI("cbw"),		ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x98 } },
	{ CLSTRI("cwde"),	ENC_ZO, 0, 0, 0,		1,{ 0x98 } },
	{ CLSTRI("cdqe"),	ENC_ZO, 0, 0, OPF_REXW,	1,{ 0x98 } },

// ----- CLC -----
	{ CLSTRI("clc"), ENC_ZO, 0, 0, 0, 1,{ 0xF8 } },

// ----- CLD -----
	{ CLSTRI("cld"), ENC_ZO, 0, 0, 0, 1,{ 0xFC } },

// ----- CLI -----
	{ CLSTRI("cli"), ENC_ZO, 0, 0, 0, 1,{ 0xFA } },

// ----- CLTS -----
	{ CLSTRI("clts"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x06 } },

// ----- CMC -----
	{ CLSTRI("cmc"), ENC_ZO, 0, 0, 0, 1,{ 0xF5 } },

// ----- CMOVcc -----
	{ CLSTRI("cmova"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x47 } },
	{ CLSTRI("cmova"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x47 } },
	{ CLSTRI("cmova"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x47 } },

	{ CLSTRI("cmovae"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x43 } },
	{ CLSTRI("cmovae"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x43 } },
	{ CLSTRI("cmovae"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x43 } },

	{ CLSTRI("cmovb"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x42 } },
	{ CLSTRI("cmovb"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x42 } },
	{ CLSTRI("cmovb"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x42 } },

	{ CLSTRI("cmovbe"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x46 } },
	{ CLSTRI("cmovbe"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x46 } },
	{ CLSTRI("cmovbe"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x46 } },

	{ CLSTRI("cmovc"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x42 } },
	{ CLSTRI("cmovc"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x42 } },
	{ CLSTRI("cmovc"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x42 } },

	{ CLSTRI("cmove"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x44 } },
	{ CLSTRI("cmove"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x44 } },
	{ CLSTRI("cmove"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x44 } },

	{ CLSTRI("cmovg"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4F } },
	{ CLSTRI("cmovg"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4F } },
	{ CLSTRI("cmovg"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4F } },

	{ CLSTRI("cmovge"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4D } },
	{ CLSTRI("cmovge"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4D } },
	{ CLSTRI("cmovge"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4D } },

	{ CLSTRI("cmovl"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4C } },
	{ CLSTRI("cmovl"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4C } },
	{ CLSTRI("cmovl"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4C } },

	{ CLSTRI("cmovle"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4E } },
	{ CLSTRI("cmovle"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4E } },
	{ CLSTRI("cmovle"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4E } },

	{ CLSTRI("cmovna"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x46 } },
	{ CLSTRI("cmovna"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x46 } },
	{ CLSTRI("cmovna"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x46 } },

	{ CLSTRI("cmovnae"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x42 } },
	{ CLSTRI("cmovnae"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,		2,{ 0x0F, 0x42 } },
	{ CLSTRI("cmovnae"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x42 } },

	{ CLSTRI("cmovnb"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x43 } },
	{ CLSTRI("cmovnb"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x43 } },
	{ CLSTRI("cmovnb"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x43 } },

	{ CLSTRI("cmovnbe"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x47 } },
	{ CLSTRI("cmovnbe"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,		2,{ 0x0F, 0x47 } },
	{ CLSTRI("cmovnbe"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x47 } },

	{ CLSTRI("cmovnc"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x43 } },
	{ CLSTRI("cmovnc"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x43 } },
	{ CLSTRI("cmovnc"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x43 } },

	{ CLSTRI("cmovne"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x45 } },
	{ CLSTRI("cmovne"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x45 } },
	{ CLSTRI("cmovne"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x45 } },

	{ CLSTRI("cmovng"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4E } },
	{ CLSTRI("cmovng"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4E } },
	{ CLSTRI("cmovng"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4E } },

	{ CLSTRI("cmovnge"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4C } },
	{ CLSTRI("cmovnge"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,		2,{ 0x0F, 0x4C } },
	{ CLSTRI("cmovnge"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4C } },

	{ CLSTRI("cmovnl"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4D } },
	{ CLSTRI("cmovnl"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4D } },
	{ CLSTRI("cmovnl"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4D } },

	{ CLSTRI("cmovnle"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4F } },
	{ CLSTRI("cmovnle"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,		2,{ 0x0F, 0x4F } },
	{ CLSTRI("cmovnle"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4F } },

	{ CLSTRI("cmovno"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x41 } },
	{ CLSTRI("cmovno"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x41 } },
	{ CLSTRI("cmovno"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x41 } },

	{ CLSTRI("cmovnp"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4B } },
	{ CLSTRI("cmovnp"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4B } },
	{ CLSTRI("cmovnp"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4B } },

	{ CLSTRI("cmovns"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x49 } },
	{ CLSTRI("cmovns"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x49 } },
	{ CLSTRI("cmovns"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x49 } },

	{ CLSTRI("cmovnz"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x45 } },
	{ CLSTRI("cmovnz"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x45 } },
	{ CLSTRI("cmovnz"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x45 } },

	{ CLSTRI("cmovo"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x40 } },
	{ CLSTRI("cmovo"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x40 } },
	{ CLSTRI("cmovo"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x40 } },

	{ CLSTRI("cmovp"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4A } },
	{ CLSTRI("cmovp"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4A } },
	{ CLSTRI("cmovp"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4A } },

	{ CLSTRI("cmovpe"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4A } },
	{ CLSTRI("cmovpe"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4A } },
	{ CLSTRI("cmovpe"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4A } },

	{ CLSTRI("cmovpo"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4B } },
	{ CLSTRI("cmovpo"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4B } },
	{ CLSTRI("cmovpo"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4B } },

	{ CLSTRI("cmovs"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x48 } },
	{ CLSTRI("cmovs"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x48 } },
	{ CLSTRI("cmovs"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x48 } },

	{ CLSTRI("cmovz"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x44 } },
	{ CLSTRI("cmovz"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x44 } },
	{ CLSTRI("cmovz"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x44 } },

// ----- CMP -----
#define NAME CLSTRI("cmp")
	{ NAME, ENC_I, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x3C } },
	{ NAME, ENC_I, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x3D } },
	{ NAME, ENC_I, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x3D } },
	{ NAME, ENC_I, OSZ2(SZ_8, SZ_4), 0, OPF_REXW,	1,{ 0x3D } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 7, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 7, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 7, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 7, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 7, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x38 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x39 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x39 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x39 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x3A } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x3B } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x3B } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x3B } },
#undef NAME

// ----- CMPS/CMPSB/CMPSW/CMPSD/CMPSQ -----
	{ CLSTRI("cmpsb"), ENC_ZO, 0, 0, 0,			1,{ 0xA6 } },
	{ CLSTRI("cmpsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xA7 } },
	{ CLSTRI("cmpsd"), ENC_ZO, 0, 0, 0,			1,{ 0xA7 } },
	{ CLSTRI("cmpsq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xA7 } },

// ----- CPUID -----
	{ CLSTRI("cpuid"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0xA2 } },

// ----- DEC -----
#define NAME CLSTRI("dec")
	{ NAME, ENC_M, OSZ1(SZ_1), 1, OPF_OPXT,				1,{ 0xFE } },
	{ NAME, ENC_M, OSZ1(SZ_2), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_4), 1, OPF_OPXT, 			1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_8), 1, OPF_OPXT|OPF_REXW,	1,{ 0xFF } },
#undef NAME

// ----- DIV -----
#define NAME CLSTRI("div")
	{ NAME, ENC_M, OSZ1(SZ_1), 6, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 6, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 6, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 6, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- ENTER -----
	{ CLSTRI("enter"), ENC_II, OSZ2(SZ_2, SZ_1), 0, 0, 1,{ 0xC8 } },

// ----- HLT -----
	{ CLSTRI("hlt"), ENC_ZO, 0, 0, 0, 1,{ 0xF4 } },

// ----- IDIV -----
#define NAME CLSTRI("idiv")
	{ NAME, ENC_M, OSZ1(SZ_1), 7, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 7, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 7, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- IMUL -----
#define NAME CLSTRI("imul")
	{ NAME, ENC_M, OSZ1(SZ_1), 5, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 5, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 5, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 5, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },

	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xAF } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0, 		2,{ 0x0F, 0xAF } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xAF } },

	{ NAME, ENC_RMI, OSZ3(SZ_2, SZ_2, SZ_1), 0, OPF_OPSZ,	1,{ 0x6B } }, // S
	{ NAME, ENC_RMI, OSZ3(SZ_4, SZ_4, SZ_1), 0, 0, 			1,{ 0x6B } }, // S
	{ NAME, ENC_RMI, OSZ3(SZ_8, SZ_8, SZ_1), 0, OPF_REXW,	1,{ 0x6B } }, // S

	{ NAME, ENC_RMI, OSZ3(SZ_2, SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x69 } },
	{ NAME, ENC_RMI, OSZ3(SZ_4, SZ_4, SZ_4), 0, 0, 			1,{ 0x69 } },
	{ NAME, ENC_RMI, OSZ3(SZ_8, SZ_8, SZ_4), 0, OPF_REXW,	1,{ 0x69 } },
#undef NAME

// ----- IN -----
#define NAME CLSTRI("in")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0xE4 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0xE5 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0xE5 } },

	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xEC } },
	{ NAME, ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xED } },
	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xED } },
#undef NAME

// ----- INC -----
#define NAME CLSTRI("inc")
	{ NAME, ENC_M, OSZ1(SZ_1), 0, OPF_OPXT,				1,{ 0xFE } },
	{ NAME, ENC_M, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_4), 0, OPF_OPXT, 			1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_8), 0, OPF_OPXT|OPF_REXW,	1,{ 0xFF } },
#undef NAME

// ----- INS/INSB/INSW/INSD -----
	{ CLSTRI("insb"), ENC_ZO, 0, 0, 0,			1,{ 0x6C } },
	{ CLSTRI("insw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x6D } },
	{ CLSTRI("insd"), ENC_ZO, 0, 0, 0,			1,{ 0x6D } },

// ----- INT/INTO/INT3/INT1 -----
	{ CLSTRI("int3"), ENC_ZO, 0, 0, 0,			1,{ 0xCC } },
	{ CLSTRI("int"), ENC_I, OSZ1(SZ_1), 0, 0,	1,{ 0xCD } },
	{ CLSTRI("into"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xCE } },
	{ CLSTRI("int1"), ENC_ZO, 0, 0, 0,			1,{ 0xF1 } },

// ----- INVD -----
	{ CLSTRI("invd"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x08 } },

// ----- INVLPG -----
	{ CLSTRI("invlpg"), ENC_M, 0, 7, OPF_OPXT, 2,{ 0x0F, 0x01 } },

// ----- IRET/IRETD/IRETQ -----
	{ CLSTRI("iret"), ENC_ZO, 0, 0, 0,			1,{ 0x6F } },
	{ CLSTRI("iretd"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x6F } },
	{ CLSTRI("iretq"), ENC_ZO, 0, 0, 0,			1,{ 0x6F } },

// ----- Jcc -----
	{ CLSTRI("ja"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x77 } },
	{ CLSTRI("jae"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x73 } },
	{ CLSTRI("jb"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x72 } },
	{ CLSTRI("jbe"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x76 } },
	{ CLSTRI("jc"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x72 } },
	{ CLSTRI("jecxz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE3 } },
	{ CLSTRI("jrcxz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE3 } },
	{ CLSTRI("je"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x74 } },
	{ CLSTRI("jg"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7F } },
	{ CLSTRI("jge"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7D } },
	{ CLSTRI("jl"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7C } },
	{ CLSTRI("jle"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7E } },
	{ CLSTRI("jna"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x76 } },
	{ CLSTRI("jnae"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x72 } },
	{ CLSTRI("jnb"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x73 } },
	{ CLSTRI("jnbe"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x77 } },
	{ CLSTRI("jnc"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x73 } },
	{ CLSTRI("jne"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x75 } },
	{ CLSTRI("jng"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7E } },
	{ CLSTRI("jnge"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7C } },
	{ CLSTRI("jnl"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7D } },
	{ CLSTRI("jnle"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7F } },
	{ CLSTRI("jno"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x71 } },
	{ CLSTRI("jnp"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7B } },
	{ CLSTRI("jns"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x79 } },
	{ CLSTRI("jnz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x75 } },
	{ CLSTRI("jo"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x70 } },
	{ CLSTRI("jp"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7A } },
	{ CLSTRI("jpe"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7A } },
	{ CLSTRI("jpo"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7B } },
	{ CLSTRI("js"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x78 } },
	{ CLSTRI("jz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x74 } },

	{ CLSTRI("ja"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x87 } },
	{ CLSTRI("jae"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x83 } },
	{ CLSTRI("jb"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x82 } },
	{ CLSTRI("jbe"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x86 } },
	{ CLSTRI("jc"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x82 } },
	{ CLSTRI("je"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x84 } },
	{ CLSTRI("jg"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8F } },
	{ CLSTRI("jge"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8D } },
	{ CLSTRI("jl"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8C } },
	{ CLSTRI("jle"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8E } },
	{ CLSTRI("jna"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x86 } },
	{ CLSTRI("jnae"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x82 } },
	{ CLSTRI("jnb"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x83 } },
	{ CLSTRI("jnbe"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x87 } },
	{ CLSTRI("jnc"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x83 } },
	{ CLSTRI("jne"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x85 } },
	{ CLSTRI("jng"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8E } },
	{ CLSTRI("jnge"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8C } },
	{ CLSTRI("jnl"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8D } },
	{ CLSTRI("jnle"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8F } },
	{ CLSTRI("jno"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x81 } },
	{ CLSTRI("jnp"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8B } },
	{ CLSTRI("jns"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x89 } },
	{ CLSTRI("jnz"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x85 } },
	{ CLSTRI("jo"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x80 } },
	{ CLSTRI("jp"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8A } },
	{ CLSTRI("jpe"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8A } },
	{ CLSTRI("jpo"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8B } },
	{ CLSTRI("js"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x88 } },
	{ CLSTRI("jz"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x84 } },

// ----- JMP -----
#define NAME CLSTRI("jmp")
	{ NAME, ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0xEB } },
	{ NAME, ENC_D, OSZ1(SZ_4), 0, 0,		1,{ 0xE9 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 4, OPF_OPXT,	1,{ 0xFF } },

	// TODO
#undef NAME

// ----- LAR -----
#define NAME CLSTRI("lar")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2, { 0x0F, 0x02 } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2, { 0x0F, 0x02 } }, // !!
#undef NAME

// !! LDS/LES/LFS/LGS/LSS

// ----- LEA -----
#define NAME CLSTRI("lea")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_8), 0, OPF_OPSZ,	1,{ 0x8D } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_8), 0, 0,			1,{ 0x8D } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x8D } },
#undef NAME

// ----- LEAVE -----
	{ CLSTRI("leave"), ENC_ZO, 0, 0, 0, 1,{ 0xC9 } },

// ----- LGDT/LIDT -----
	{ CLSTRI("lgdt"), ENC_M, OSZ1(SZ_8), 2, OPF_OPXT, 1,{ 0x0F, 0x01 } },
	{ CLSTRI("lidt"), ENC_M, OSZ1(SZ_8), 3, OPF_OPXT, 1,{ 0x0F, 0x01 } },

// ----- LLDT -----
	{ CLSTRI("lldt"), ENC_M, OSZ1(SZ_2), 2, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- LMSW -----
	{ CLSTRI("lmsw"), ENC_M, OSZ1(SZ_2), 6, OPF_OPXT, 2,{ 0x0F, 0x01 } },

// ----- LODS/LODSB/LODSW/LODSD/LODSQ -----
	{ CLSTRI("lodsb"), ENC_ZO, 0, 0, 0,			1,{ 0xAC } },
	{ CLSTRI("lodsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xAD } },
	{ CLSTRI("lodsd"), ENC_ZO, 0, 0, 0,			1,{ 0xAD } },
	{ CLSTRI("lodsq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xAD } },

// ----- LOOP ------
	{ CLSTRI("loop"), ENC_D, OSZ1(SZ_1), 0, 0, 		1,{ 0xE2 } },
	{ CLSTRI("loope"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0xE1 } },
	{ CLSTRI("loopne"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE0 } },

// ----- LSL -----
#define NAME CLSTRI("lsl")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x0F, 0x03 } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x0F, 0x03 } }, // !!
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_4), 0, OPF_REXW,	1,{ 0x0F, 0x03 } }, // !!
#undef NAME

// ----- LTR -----
	{ CLSTRI("ltr"), ENC_M, OSZ1(SZ_2), 3, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- MOV -----
#define NAME CLSTRI("mov")
	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x88 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x89 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0, 		1,{ 0x89 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x89 } },

	{ NAME, ENC_RM, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x8A } },
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x8B } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0, 		1,{ 0x8B } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x8B } },

	{ NAME, ENC_OI, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0xB0 } },
	{ NAME, ENC_OI, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0xB8 } },
	{ NAME, ENC_OI, OSZ2(SZ_4, SZ_4), 0, 0, 		1,{ 0xB8 } },
	{ NAME, ENC_OI, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0xB8 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 0, OPF_OPXT,			1,{ 0xC6 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xc7 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 0, OPF_OPXT, 			1,{ 0xC7 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 0, OPF_OPXT|OPF_REXW,	1,{ 0xC7 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, 0,					2,{ 0x0F, 0x20 }}, // CR
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_OPXT|OPF_REXR,	2,{ 0x0F, 0x20 }}, // CR
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, 0,					2,{ 0x0F, 0x22 }}, // CR
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_OPXT|OPF_REXR,	2,{ 0x0F, 0x22 }}, // CR

	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, 0,					2,{ 0x0F, 0x21 }}, // DR
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, 0,					2,{ 0x0F, 0x23 }}, // DR

	// TODO
#undef NAME

// ----- MOVS/MOVSB/MOVSW/MOVSD/MOVSQ -----
	{ CLSTRI("movsb"), ENC_ZO, 0, 0, 0,			1,{ 0xA4 } },
	{ CLSTRI("movsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xA5 } },
	{ CLSTRI("movsd"), ENC_ZO, 0, 0, 0,			1,{ 0xA5 } },
	{ CLSTRI("movsq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xA5 } },

// ----- MOVSX/MOVSXD -----
#define NAME CLSTRI("movsx")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_1), 0, OPF_OPSZ,	2,{ 0x0F, 0xBE } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_1), 0, 0, 		2,{ 0x0F, 0xBE } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_1), 0, OPF_REXW,	2,{ 0x0F, 0xBE } },

	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_2), 0, 0, 		2,{ 0x0F, 0xBF } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_2), 0, OPF_REXW,	2,{ 0x0F, 0xBF } },
#undef NAME
#define NAME CLSTRI("movsxd")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x63 } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0, 		1,{ 0x63 } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_4), 0, OPF_REXW,	1,{ 0x63 } },
#undef NAME

// ----- MOVZX -----
#define NAME CLSTRI("movzx")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_1), 0, OPF_OPSZ,	2,{ 0x0F, 0xB6 } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_1), 0, 0, 		2,{ 0x0F, 0xB6 } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_1), 0, OPF_REXW,	2,{ 0x0F, 0xB6 } },

	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_2), 0, 0, 		2,{ 0x0F, 0xB7 } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_2), 0, OPF_REXW,	2,{ 0x0F, 0xB7 } },
#undef NAME

// ----- MUL -----
#define NAME CLSTRI("mul")
	{ NAME, ENC_M, OSZ1(SZ_1), 4, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 4, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 4, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- NEG -----
#define NAME CLSTRI("neg")
	{ NAME, ENC_M, OSZ1(SZ_1), 3, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 3, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 3, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- NOP -----
#define NAME CLSTRI("nop")
	{ NAME, ENC_ZO, 0, 0, 0, 1,{ 0x90 } }, // NP

	{ NAME, ENC_M, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0x1F } }, // NP
	{ NAME, ENC_M, OSZ1(SZ_4), 0, OPF_OPXT, 			2,{ 0x0F, 0x1F } }, // NP
#undef NAME

// ----- NOT -----
#define NAME CLSTRI("not")
	{ NAME, ENC_M, OSZ1(SZ_1), 2, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 2, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 2, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- OR -----
#define NAME CLSTRI("or")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0x0C } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x0D } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0x0D } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x0D } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 1, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 1, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 1, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 1, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 1, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x08 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x09 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x09 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x09 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x0A } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x0B } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x0B } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x0B } },
#undef NAME

// ----- OUT -----
#define NAME CLSTRI("out")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0xE6 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0xE7 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0xE7 } },

	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xEE } },
	{ NAME, ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xEF } },
	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xEF } },
#undef NAME

// ----- OUTS/OUTSB/OUTSW/OUTSD -----
	{ CLSTRI("outsb"), ENC_ZO, 0, 0, 0,			1,{ 0x6E } },
	{ CLSTRI("outsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x6F } },
	{ CLSTRI("outsd"), ENC_ZO, 0, 0, 0,			1,{ 0x6F } },

// ----- POP -----
#define NAME CLSTRI("pop")
	{ NAME, ENC_M, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0x8F } },
	{ NAME, ENC_M, OSZ1(SZ_8), 0, OPF_OPXT,				1,{ 0x8F } },

	{ NAME, ENC_O, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x58 } },
	{ NAME, ENC_O, OSZ1(SZ_8), 0, 0,		1,{ 0x58 } },

	// TODO
#undef NAME

// ----- POPF/POPFQ -----
	{ CLSTRI("popf"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x9D } },
	{ CLSTRI("popfq"), ENC_ZO, 0, 0, 0,			1,{ 0x9D } },

// ----- PUSH -----
#define NAME CLSTRI("push")
	{ NAME, ENC_M, OSZ1(SZ_2), 6, OPF_OPXT|OPF_OPSZ,	1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_8), 6, OPF_OPXT,				1,{ 0xFF } },

	{ NAME, ENC_O, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x50 } },
	{ NAME, ENC_O, OSZ1(SZ_8), 0, 0,		1,{ 0x50 } },

	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0x6A } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x6B } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0x6B } },

	// TODO
#undef NAME

// ----- PUSHF/PUSHFQ -----
	{ CLSTRI("pushf"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x9C } },
	{ CLSTRI("pushfq"), ENC_ZO, 0, 0, 0,		1,{ 0x9C } },

// ----- RCL/RCR/ROL/ROR -----
#define NAME CLSTRI("rcl")
	{ NAME, ENC_M1, OSZ1(SZ_1), 2, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 2, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 2, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 2, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 2, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 2, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 2, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 2, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 2, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME
#define NAME CLSTRI("rcr")
	{ NAME, ENC_M1, OSZ1(SZ_1), 3, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 3, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 3, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 3, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 3, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 3, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 3, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 3, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 3, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME
#define NAME CLSTRI("rol")
	{ NAME, ENC_M1, OSZ1(SZ_1), 0, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 0, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 0, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 0, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 0, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 0, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 0, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 0, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 0, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME
#define NAME CLSTRI("ror")
	{ NAME, ENC_M1, OSZ1(SZ_1), 1, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 1, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 1, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 1, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 1, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 1, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 1, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 1, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 1, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME

// ----- RDMSR -----
	{ CLSTRI("rdmsr"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x32 } },

// ----- RDTSC -----
	{ CLSTRI("rdtsc"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x31 } },

// ----- RET -----
#define NAME CLSTRI("ret")
	{ NAME, ENC_ZO, 0, 0, 0, 1,{ 0xC3 } },
	{ NAME, ENC_ZO, 0, 0, 0, 1,{ 0xCB } }, // FAR
	{ NAME, ENC_I, OSZ1(SZ_2), 0, 0, 1,{ 0xC2 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, 0, 1,{ 0xCA } }, // FAR
#undef NAME

// ----- RSM -----
	{ CLSTRI("rsm"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0xAA } },

// ----- SAL/SAR/SHL/SHR -----
#define NAME CLSTRI("sal")
	{ NAME, ENC_M1, OSZ1(SZ_1), 4, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 4, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 4, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 4, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 4, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME
#define NAME CLSTRI("sar")
	{ NAME, ENC_M1, OSZ1(SZ_1), 7, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 7, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 7, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 7, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 7, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 7, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 7, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 7, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 7, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME
#define NAME CLSTRI("shl")
	{ NAME, ENC_M1, OSZ1(SZ_1), 4, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 4, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 4, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 4, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 4, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME
#define NAME CLSTRI("shr")
	{ NAME, ENC_M1, OSZ1(SZ_1), 5, OPF_OPXT,			1,{ 0xD0 } },
	{ NAME, ENC_M1, OSZ1(SZ_2), 5, OPF_OPXT|OPF_OPSZ,	1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_4), 5, OPF_OPXT, 			1,{ 0xD1 } },
	{ NAME, ENC_M1, OSZ1(SZ_8), 5, OPF_OPXT|OPF_REXW,	1,{ 0xD1 } },

	{ NAME, ENC_MC, OSZ2(SZ_1, SZ_1), 5, OPF_OPXT,			1,{ 0xD2 } },
	{ NAME, ENC_MC, OSZ2(SZ_2, SZ_1), 5, OPF_OPXT|OPF_OPSZ,	1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_4, SZ_1), 5, OPF_OPXT, 			1,{ 0xD3 } },
	{ NAME, ENC_MC, OSZ2(SZ_8, SZ_1), 5, OPF_OPXT|OPF_REXW,	1,{ 0xD3 } },

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 5, OPF_OPXT,			1,{ 0xC0 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 5, OPF_OPXT|OPF_OPSZ,	1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 5, OPF_OPXT, 			1,{ 0xC1 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 5, OPF_OPXT|OPF_REXW,	1,{ 0xC1 } },
#undef NAME

// ----- SBB -----
#define NAME CLSTRI("sbb")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0x1C } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x1D } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0x1D } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x1D } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 3, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 3, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 3, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 3, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 3, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x18 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x19 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x19 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x19 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x1A } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x1B } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x1B } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x1B } },
#undef NAME

// ----- SCAS/SCASB/SCASW/SCASD/SCASQ -----
	{ CLSTRI("scasb"), ENC_ZO, 0, 0, 0,			1,{ 0xAE } },
	{ CLSTRI("scasw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xAF } },
	{ CLSTRI("scasd"), ENC_ZO, 0, 0, 0,			1,{ 0xAF } },
	{ CLSTRI("scasq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xAF } },

// ----- SETcc -----
	{ CLSTRI("seta"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x97 } },
	{ CLSTRI("setae"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x93 } },
	{ CLSTRI("setb"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x92 } },
	{ CLSTRI("setbe"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x96 } },
	{ CLSTRI("setc"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x92 } },
	{ CLSTRI("sete"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x94 } },
	{ CLSTRI("setg"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9F } },
	{ CLSTRI("setge"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9D } },
	{ CLSTRI("setl"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9C } },
	{ CLSTRI("setle"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9E } },
	{ CLSTRI("setna"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x96 } },
	{ CLSTRI("setnae"), ENC_M, OSZ1(SZ_1), 0, 0,2,{ 0x0F, 0x92 } },
	{ CLSTRI("setnb"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x93 } },
	{ CLSTRI("setnbe"), ENC_M, OSZ1(SZ_1), 0, 0,2,{ 0x0F, 0x97 } },
	{ CLSTRI("setnc"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x93 } },
	{ CLSTRI("setne"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x95 } },
	{ CLSTRI("setng"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9E } },
	{ CLSTRI("setnge"), ENC_M, OSZ1(SZ_1), 0, 0,2,{ 0x0F, 0x9C } },
	{ CLSTRI("setnl"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9D } },
	{ CLSTRI("setnle"), ENC_M, OSZ1(SZ_1), 0, 0,2,{ 0x0F, 0x9F } },
	{ CLSTRI("setno"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x91 } },
	{ CLSTRI("setnp"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9B } },
	{ CLSTRI("setns"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x99 } },
	{ CLSTRI("setnz"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x95 } },
	{ CLSTRI("seto"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x90 } },
	{ CLSTRI("setp"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9A } },
	{ CLSTRI("setpe"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9A } },
	{ CLSTRI("setpo"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9B } },
	{ CLSTRI("sets"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x98 } },
	{ CLSTRI("setz"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x94 } },

// ----- SGDT -----
	{ CLSTRI("sgdt"), ENC_M, OSZ1(SZ_8), 0, OPF_OPXT, 1,{ 0x0F, 0x01 } }, // !!

// ----- SIDT -----
	{ CLSTRI("sidt"), ENC_M, OSZ1(SZ_8), 1, OPF_OPXT, 1,{ 0x0F, 0x01 } }, // !!

// ----- SLDT -----
	{ CLSTRI("sldt"), ENC_M, OSZ1(SZ_2), 0, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- SMSW -----
#define NAME CLSTRI("smsw")
	{ NAME, ENC_M, OSZ1(SZ_2), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0x0F, 0x01 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 4, OPF_OPXT,				1,{ 0x0F, 0x01 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 4, OPF_OPXT|OPF_REXW,	1,{ 0x0F, 0x01 } },
#undef NAME

// ----- STC -----
	{ CLSTRI("stc"), ENC_ZO, 0, 0, 0, 1,{ 0xF9 } },

// ----- STD -----
	{ CLSTRI("std"), ENC_ZO, 0, 0, 0, 1,{ 0xFD } },

// ----- STI -----
	{ CLSTRI("sti"), ENC_ZO, 0, 0, 0, 1,{ 0xFB } },

// ----- STOS/STOSB/STOSW/STOSD/STOSQ -----
	{ CLSTRI("stosb"), ENC_ZO, 0, 0, 0,			1,{ 0xAA } },
	{ CLSTRI("stosw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xAB } },
	{ CLSTRI("stosd"), ENC_ZO, 0, 0, 0,			1,{ 0xAB } },
	{ CLSTRI("stosq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xAB } },

// ----- STR -----
	{ CLSTRI("str"), ENC_M, OSZ1(SZ_2), 1, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- SUB -----
#define NAME CLSTRI("sub")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,			1,{ 0x2C } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x2D } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,			1,{ 0x2D } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x2D } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 5, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 5, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 5, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 5, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 5, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 5, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 5, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x28 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x29 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x29 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x29 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x2A } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x2B } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x2B } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x2B } },
#undef NAME

// ----- SYSCALL -----
	{ CLSTRI("syscall"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x05 } },

// ----- SYSENTER -----
	{ CLSTRI("sysenter"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x34 } },

// ----- SYSEXIT -----
	{ CLSTRI("sysexit"), ENC_ZO, 0, 0, 0,		2,{ 0x0F, 0x35 } },
	{ CLSTRI("sysexit"), ENC_ZO, 0, 0, OPF_REXW,2,{ 0x0F, 0x35 } },

// ----- SYSRET -----
	{ CLSTRI("sysret"), ENC_ZO, 0, 0, 0,		2,{ 0x0F, 0x07 } },
	{ CLSTRI("sysret"), ENC_ZO, 0, 0, OPF_REXW,	2,{ 0x0F, 0x07 } },

// ----- TEST -----
#define NAME CLSTRI("test")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0xA8 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0xA9 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0xA9 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0xA9 } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 0, OPF_OPXT,			1,{ 0xF6 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 0, OPF_OPXT,			1,{ 0xF7 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 0, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x84 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x85 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x85 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x85 } },
#undef NAME

// ----- UD -----
	{ CLSTRI("ud0"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,	2,{ 0x0F, 0xFF } },
	{ CLSTRI("ud1"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0, 	2,{ 0x0F, 0xB9 } },
	{ CLSTRI("ud2"), ENC_ZO, 0, 0, 0,					2,{ 0x0F, 0x0B } },

// ----- WAIT/FWAIT -----
	{ CLSTRI("wait"), ENC_ZO, 0, 0, 0,	1,{ 0x9B } },
	{ CLSTRI("fwait"), ENC_ZO, 0, 0, 0,	1,{ 0x9B } },

// ----- WRMSR -----
	{ CLSTRI("wrmsr"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x30 } },

// ----- XADD -----
#define NAME CLSTRI("xadd")
	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			2,{ 0x0F, 0xC0 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xC1 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xC1 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xC1 } },
#undef NAME

// ----- XCHG -----
#define NAME CLSTRI("xchg")
	{ NAME, ENC_O, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x90 } },
	{ NAME, ENC_O, OSZ1(SZ_4), 0, 0,		1,{ 0x90 } },
	{ NAME, ENC_O, OSZ1(SZ_8), 0, OPF_REXW,	1,{ 0x90 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x86 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x87 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x87 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x87 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x86 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x87 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x87 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x87 } },
#undef NAME

// ----- XLAT/XLATB -----
	{ CLSTRI("xlat"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xD7 } },
	{ CLSTRI("xlatb"), ENC_ZO, 0, 0, 0,			1,{ 0xD7 } },
	{ CLSTRI("xlatb"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xD7 } },

// ----- XOR -----
#define NAME CLSTRI("xor")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0x34 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x35 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0x35 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, OPF_REXW,	1,{ 0x35 } }, // S

	{ NAME, ENC_MI, OSZ2(SZ_1, SZ_1), 6, OPF_OPXT,			1,{ 0x80 } },
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 6, OPF_OPXT|OPF_OPSZ,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 6, OPF_OPXT,			1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 6, OPF_OPXT|OPF_REXW,	1,{ 0x83 } }, // S
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_2), 6, OPF_OPXT|OPF_OPSZ,	1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_4), 6, OPF_OPXT,			1,{ 0x81 } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_4), 6, OPF_OPXT|OPF_REXW,	1,{ 0x81 } }, // S

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x30 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x31 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x31 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x31 } },

	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			1,{ 0x32 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x33 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			1,{ 0x33 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x33 } },
#undef NAME

};

