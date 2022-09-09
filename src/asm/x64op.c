
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

#define ENC_ZO	0x00 // 
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

// ----- ADD -----
#define NAME CLSTR("add")
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

// ----- ADC -----
#define NAME CLSTR("adc")
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

// ----- AND -----
#define NAME CLSTR("and")
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
#define NAME CLSTR("bsf")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xBC } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xBC } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xBC } },
#undef NAME

// ----- BSR -----
#define NAME CLSTR("bsr")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xBD } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xBD } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xBD } },
#undef NAME

// ----- BSWAP -----
#define NAME CLSTR("bswap")
	{ NAME, ENC_O, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0xC8 } },
	{ NAME, ENC_O, OSZ1(SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xC8 } },
#undef NAME

// ----- BT -----
#define NAME CLSTR("bt")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 4, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 4, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 4, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xA3 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xA3 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xA3 } },
#undef NAME

// ----- BTC -----
#define NAME CLSTR("btc")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 7, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 7, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 7, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xBB } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xBB } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xBB } },
#undef NAME

// ----- BTR -----
#define NAME CLSTR("btr")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 6, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 6, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 6, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xB3 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xB3 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xB3 } },
#undef NAME

// ----- BTS -----
#define NAME CLSTR("bts")
	{ NAME, ENC_MI, OSZ2(SZ_2, SZ_1), 5, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_4, SZ_1), 5, OPF_OPXT|0,		2,{ 0x0F, 0xBA } },
	{ NAME, ENC_MI, OSZ2(SZ_8, SZ_1), 5, OPF_OPXT|OPF_REXW,	2,{ 0x0F, 0xBA } },

	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xAB } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xAB } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xAB } },
#undef NAME

// ----- CALL -----
#define NAME CLSTR("call")
	{ NAME, ENC_D, OSZ1(SZ_4), 0, 0,		1,{ 0xE8 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 2, OPF_OPXT,	1,{ 0xFF } },

	// TODO
#undef NAME

// ----- CBW/CWDE/CDQE -----
	{ CLSTR("cbw"),		ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x98 } },
	{ CLSTR("cwde"),	ENC_ZO, 0, 0, 0,		1,{ 0x98 } },
	{ CLSTR("cdqe"),	ENC_ZO, 0, 0, OPF_REXW,	1,{ 0x98 } },

// ----- CLC -----
	{ CLSTR("clc"), ENC_ZO, 0, 0, 0, 1,{ 0xF8 } },

// ----- CLD -----
	{ CLSTR("cld"), ENC_ZO, 0, 0, 0, 1,{ 0xFC } },

// ----- CLI -----
	{ CLSTR("cli"), ENC_ZO, 0, 0, 0, 1,{ 0xFA } },

// ----- CLTS -----
	{ CLSTR("clts"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x06 } },

// ----- CMC -----
	{ CLSTR("cmc"), ENC_ZO, 0, 0, 0, 1,{ 0xF5 } },

// ----- CMOVcc -----
	{ CLSTR("cmova"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x47 } },
	{ CLSTR("cmova"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x47 } },
	{ CLSTR("cmova"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x47 } },

	{ CLSTR("cmovae"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x43 } },
	{ CLSTR("cmovae"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x43 } },
	{ CLSTR("cmovae"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x43 } },

	{ CLSTR("cmovb"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x42 } },
	{ CLSTR("cmovb"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x42 } },
	{ CLSTR("cmovb"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x42 } },

	{ CLSTR("cmovbe"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x46 } },
	{ CLSTR("cmovbe"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x46 } },
	{ CLSTR("cmovbe"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x46 } },

	{ CLSTR("cmovc"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x42 } },
	{ CLSTR("cmovc"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x42 } },
	{ CLSTR("cmovc"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x42 } },

	{ CLSTR("cmove"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x44 } },
	{ CLSTR("cmove"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x44 } },
	{ CLSTR("cmove"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x44 } },

	{ CLSTR("cmovg"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4F } },
	{ CLSTR("cmovg"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4F } },
	{ CLSTR("cmovg"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4F } },

	{ CLSTR("cmovge"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4D } },
	{ CLSTR("cmovge"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4D } },
	{ CLSTR("cmovge"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4D } },

	{ CLSTR("cmovl"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4C } },
	{ CLSTR("cmovl"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4C } },
	{ CLSTR("cmovl"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4C } },

	{ CLSTR("cmovle"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4E } },
	{ CLSTR("cmovle"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4E } },
	{ CLSTR("cmovle"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4E } },

	{ CLSTR("cmovna"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x46 } },
	{ CLSTR("cmovna"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x46 } },
	{ CLSTR("cmovna"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x46 } },

	{ CLSTR("cmovnae"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x42 } },
	{ CLSTR("cmovnae"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x42 } },
	{ CLSTR("cmovnae"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x42 } },

	{ CLSTR("cmovnb"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x43 } },
	{ CLSTR("cmovnb"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x43 } },
	{ CLSTR("cmovnb"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x43 } },

	{ CLSTR("cmovnbe"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x47 } },
	{ CLSTR("cmovnbe"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x47 } },
	{ CLSTR("cmovnbe"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x47 } },

	{ CLSTR("cmovnc"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x43 } },
	{ CLSTR("cmovnc"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x43 } },
	{ CLSTR("cmovnc"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x43 } },

	{ CLSTR("cmovne"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x45 } },
	{ CLSTR("cmovne"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x45 } },
	{ CLSTR("cmovne"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x45 } },

	{ CLSTR("cmovng"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4E } },
	{ CLSTR("cmovng"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4E } },
	{ CLSTR("cmovng"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4E } },

	{ CLSTR("cmovnge"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4C } },
	{ CLSTR("cmovnge"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4C } },
	{ CLSTR("cmovnge"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4C } },

	{ CLSTR("cmovnl"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4D } },
	{ CLSTR("cmovnl"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4D } },
	{ CLSTR("cmovnl"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4D } },

	{ CLSTR("cmovnle"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4F } },
	{ CLSTR("cmovnle"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4F } },
	{ CLSTR("cmovnle"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4F } },

	{ CLSTR("cmovno"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x41 } },
	{ CLSTR("cmovno"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x41 } },
	{ CLSTR("cmovno"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x41 } },

	{ CLSTR("cmovnp"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4B } },
	{ CLSTR("cmovnp"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4B } },
	{ CLSTR("cmovnp"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4B } },

	{ CLSTR("cmovns"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x49 } },
	{ CLSTR("cmovns"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x49 } },
	{ CLSTR("cmovns"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x49 } },

	{ CLSTR("cmovnz"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x45 } },
	{ CLSTR("cmovnz"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x45 } },
	{ CLSTR("cmovnz"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x45 } },

	{ CLSTR("cmovo"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x40 } },
	{ CLSTR("cmovo"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x40 } },
	{ CLSTR("cmovo"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x40 } },

	{ CLSTR("cmovp"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4A } },
	{ CLSTR("cmovp"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4A } },
	{ CLSTR("cmovp"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4A } },

	{ CLSTR("cmovpe"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4A } },
	{ CLSTR("cmovpe"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4A } },
	{ CLSTR("cmovpe"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4A } },

	{ CLSTR("cmovpo"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x4B } },
	{ CLSTR("cmovpo"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x4B } },
	{ CLSTR("cmovpo"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x4B } },

	{ CLSTR("cmovs"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x48 } },
	{ CLSTR("cmovs"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x48 } },
	{ CLSTR("cmovs"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x48 } },

	{ CLSTR("cmovz"), ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0x44 } },
	{ CLSTR("cmovz"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0x44 } },
	{ CLSTR("cmovz"), ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0x44 } },

// ----- CMP -----
#define NAME CLSTR("cmp")
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
	{ CLSTR("cmpsb"), ENC_ZO, 0, 0, 0,			1,{ 0xA6 } },
	{ CLSTR("cmpsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xA7 } },
	{ CLSTR("cmpsd"), ENC_ZO, 0, 0, 0,			1,{ 0xA7 } },
	{ CLSTR("cmpsq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xA7 } },

// ----- CPUID -----
	{ CLSTR("cpuid"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0xA2 } },

// ----- DEC -----
#define NAME CLSTR("dec")
	{ NAME, ENC_M, OSZ1(SZ_1), 1, OPF_OPXT,				1,{ 0xFE } },
	{ NAME, ENC_M, OSZ1(SZ_2), 1, OPF_OPXT|OPF_OPSZ,	1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_4), 1, OPF_OPXT, 			1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_8), 1, OPF_OPXT|OPF_REXW,	1,{ 0xFF } },
#undef NAME

// ----- DIV -----
#define NAME CLSTR("div")
	{ NAME, ENC_M, OSZ1(SZ_1), 6, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 6, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 6, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 6, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- ENTER -----
	{ CLSTR("enter"), ENC_II, OSZ2(SZ_2, SZ_1), 0, 0, 1,{ 0xC8 } },

// ----- HLT -----
	{ CLSTR("hlt"), ENC_ZO, 0, 0, 0, 1,{ 0xF4 } },

// ----- IDIV -----
#define NAME CLSTR("idiv")
	{ NAME, ENC_M, OSZ1(SZ_1), 7, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 7, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 7, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 7, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- IMUL -----
#define NAME CLSTR("imul")
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
#define NAME CLSTR("in")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0xE4 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0xE5 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0xE5 } },

	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xEC } },
	{ NAME, ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xED } },
	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xED } },
#undef NAME

// ----- INC -----
#define NAME CLSTR("inc")
	{ NAME, ENC_M, OSZ1(SZ_1), 0, OPF_OPXT,				1,{ 0xFE } },
	{ NAME, ENC_M, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_4), 0, OPF_OPXT, 			1,{ 0xFF } },
	{ NAME, ENC_M, OSZ1(SZ_8), 0, OPF_OPXT|OPF_REXW,	1,{ 0xFF } },
#undef NAME

// ----- INS/INSB/INSW/INSD -----
	{ CLSTR("insb"), ENC_ZO, 0, 0, 0,			1,{ 0x6C } },
	{ CLSTR("insw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x6D } },
	{ CLSTR("insd"), ENC_ZO, 0, 0, 0,			1,{ 0x6D } },

// ----- INT/INTO/INT3/INT1 -----
	{ CLSTR("int3"), ENC_ZO, 0, 0, 0,			1,{ 0xCC } },
	{ CLSTR("int"), ENC_I, OSZ1(SZ_1), 0, 0,	1,{ 0xCD } },
	{ CLSTR("into"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xCE } },
	{ CLSTR("int1"), ENC_ZO, 0, 0, 0,			1,{ 0xF1 } },

// ----- INVD -----
	{ CLSTR("invd"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x08 } },

// ----- INVLPG -----
	{ CLSTR("invlpg"), ENC_M, 0, 7, OPF_OPXT, 2,{ 0x0F, 0x01 } },

// ----- IRET/IRETD/IRETQ -----
	{ CLSTR("iret"), ENC_ZO, 0, 0, 0,			1,{ 0x6F } },
	{ CLSTR("iretd"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x6F } },
	{ CLSTR("iretq"), ENC_ZO, 0, 0, 0,			1,{ 0x6F } },

// ----- Jcc -----
	{ CLSTR("ja"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x77 } },
	{ CLSTR("jae"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x73 } },
	{ CLSTR("jb"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x72 } },
	{ CLSTR("jbe"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x76 } },
	{ CLSTR("jc"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x72 } },
	{ CLSTR("jecxz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE3 } },
	{ CLSTR("jrcxz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE3 } },
	{ CLSTR("je"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x74 } },
	{ CLSTR("jg"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x7F } },
	{ CLSTR("jge"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7D } },
	{ CLSTR("jl"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x7C } },
	{ CLSTR("jle"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7E } },
	{ CLSTR("jna"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x76 } },
	{ CLSTR("jnae"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x72 } },
	{ CLSTR("jnb"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x73 } },
	{ CLSTR("jnbe"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x77 } },
	{ CLSTR("jnc"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x73 } },
	{ CLSTR("jne"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x75 } },
	{ CLSTR("jng"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7E } },
	{ CLSTR("jnge"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7C } },
	{ CLSTR("jnl"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7D } },
	{ CLSTR("jnle"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7F } },
	{ CLSTR("jno"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x71 } },
	{ CLSTR("jnp"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7B } },
	{ CLSTR("jns"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x79 } },
	{ CLSTR("jnz"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x75 } },
	{ CLSTR("jo"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x70 } },
	{ CLSTR("jp"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x7A } },
	{ CLSTR("jpe"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7A } },
	{ CLSTR("jpo"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0x7B } },
	{ CLSTR("js"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x78 } },
	{ CLSTR("jz"), ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0x74 } },

	{ CLSTR("ja"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x87 } },
	{ CLSTR("jae"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x83 } },
	{ CLSTR("jb"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x82 } },
	{ CLSTR("jbe"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x86 } },
	{ CLSTR("jc"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x82 } },
	{ CLSTR("je"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x84 } },
	{ CLSTR("jg"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x8F } },
	{ CLSTR("jge"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8D } },
	{ CLSTR("jl"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x8C } },
	{ CLSTR("jle"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8E } },
	{ CLSTR("jna"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x86 } },
	{ CLSTR("jnae"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x82 } },
	{ CLSTR("jnb"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x83 } },
	{ CLSTR("jnbe"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x87 } },
	{ CLSTR("jnc"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x83 } },
	{ CLSTR("jne"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x85 } },
	{ CLSTR("jng"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8E } },
	{ CLSTR("jnge"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8C } },
	{ CLSTR("jnl"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8D } },
	{ CLSTR("jnle"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8F } },
	{ CLSTR("jno"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x81 } },
	{ CLSTR("jnp"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8B } },
	{ CLSTR("jns"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x89 } },
	{ CLSTR("jnz"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x85 } },
	{ CLSTR("jo"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x80 } },
	{ CLSTR("jp"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x8A } },
	{ CLSTR("jpe"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8A } },
	{ CLSTR("jpo"), ENC_D, OSZ1(SZ_4), 0, 0,	2,{ 0x0F, 0x8B } },
	{ CLSTR("js"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x88 } },
	{ CLSTR("jz"), ENC_D, OSZ1(SZ_4), 0, 0,		2,{ 0x0F, 0x84 } },

// ----- JMP -----
#define NAME CLSTR("jmp")
	{ NAME, ENC_D, OSZ1(SZ_1), 0, 0,		1,{ 0xEB } },
	{ NAME, ENC_D, OSZ1(SZ_4), 0, 0,		1,{ 0xE9 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 4, OPF_OPXT,	1,{ 0xFF } },

	// TODO
#undef NAME

// !! LAR
// !! LDS/LES/LFS/LGS/LSS

// ----- LEA -----
#define NAME CLSTR("lea")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_8), 0, OPF_OPSZ,	1,{ 0x8D } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_8), 0, 0,		1,{ 0x8D } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	1,{ 0x8D } },
#undef NAME

// ----- LEAVE -----
	{ CLSTR("leave"), ENC_ZO, 0, 0, OPF_REXW, 1,{ 0xC9 } },

// !! LGDT/LIDT

// ----- LLDT -----
	{ CLSTR("lldt"), ENC_M, OSZ1(SZ_2), 2, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- LMSW -----
	{ CLSTR("lmsw"), ENC_M, OSZ1(SZ_2), 6, OPF_OPXT, 2,{ 0x0F, 0x01 } },

// ----- LODS/LODSB/LODSW/LODSD/LODSQ -----
	{ CLSTR("lodsb"), ENC_ZO, 0, 0, 0,			1,{ 0xAC } },
	{ CLSTR("lodsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xAD } },
	{ CLSTR("lodsd"), ENC_ZO, 0, 0, 0,			1,{ 0xAD } },
	{ CLSTR("lodsq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xAD } },

// ----- LOOP ------
	{ CLSTR("loop"), ENC_D, OSZ1(SZ_1), 0, 0, 	1,{ 0xE2 } },
	{ CLSTR("loope"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE1 } },
	{ CLSTR("loopne"), ENC_D, OSZ1(SZ_1), 0, 0,	1,{ 0xE0 } },

// !! LSL

// ----- LTR -----
	{ CLSTR("ltr"), ENC_M, OSZ1(SZ_2), 3, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- MOV -----
#define NAME CLSTR("mov")
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

// !! MOVBE

// ----- MOVS/MOVSB/MOVSW/MOVSD/MOVSQ -----
	{ CLSTR("movsb"), ENC_ZO, 0, 0, 0,			1,{ 0xA4 } },
	{ CLSTR("movsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xA5 } },
	{ CLSTR("movsd"), ENC_ZO, 0, 0, 0,			1,{ 0xA5 } },
	{ CLSTR("movsq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xA5 } },

// ----- MOVSX/MOVSXD -----
#define NAME CLSTR("movsx")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_1), 0, OPF_OPSZ,	2,{ 0x0F, 0xBE } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_1), 0, 0, 		2,{ 0x0F, 0xBE } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_1), 0, OPF_REXW,	2,{ 0x0F, 0xBE } },

	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_2), 0, 0, 		2,{ 0x0F, 0xBF } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_2), 0, OPF_REXW,	2,{ 0x0F, 0xBF } },
#undef NAME
#define NAME CLSTR("movsxd")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	1,{ 0x63 } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0, 		1,{ 0x63 } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_4), 0, OPF_REXW,	1,{ 0x63 } },
#undef NAME

// ----- MOVZX -----
#define NAME CLSTR("movzx")
	{ NAME, ENC_RM, OSZ2(SZ_2, SZ_1), 0, OPF_OPSZ,	2,{ 0x0F, 0xB6 } },
	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_1), 0, 0, 		2,{ 0x0F, 0xB6 } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_1), 0, OPF_REXW,	2,{ 0x0F, 0xB6 } },

	{ NAME, ENC_RM, OSZ2(SZ_4, SZ_2), 0, 0, 		2,{ 0x0F, 0xB7 } },
	{ NAME, ENC_RM, OSZ2(SZ_8, SZ_2), 0, OPF_REXW,	2,{ 0x0F, 0xB7 } },
#undef NAME

// ----- MUL -----
#define NAME CLSTR("mul")
	{ NAME, ENC_M, OSZ1(SZ_1), 4, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 4, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 4, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 4, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- NEG -----
#define NAME CLSTR("neg")
	{ NAME, ENC_M, OSZ1(SZ_1), 3, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 3, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 3, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 3, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- NOP -----
#define NAME CLSTR("nop")
	{ NAME, ENC_ZO, 0, 0, 0, 1,{ 0x90 } }, // NP

	{ NAME, ENC_M, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	2,{ 0x0F, 0x1F } }, // NP
	{ NAME, ENC_M, OSZ1(SZ_4), 0, OPF_OPXT, 			2,{ 0x0F, 0x1F } }, // NP
#undef NAME

// ----- NOT -----
#define NAME CLSTR("not")
	{ NAME, ENC_M, OSZ1(SZ_1), 2, OPF_OPXT,				1,{ 0xF6 } },
	{ NAME, ENC_M, OSZ1(SZ_2), 2, OPF_OPXT|OPF_OPSZ,	1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_4), 2, OPF_OPXT, 			1,{ 0xF7 } },
	{ NAME, ENC_M, OSZ1(SZ_8), 2, OPF_OPXT|OPF_REXW,	1,{ 0xF7 } },
#undef NAME

// ----- OR -----
#define NAME CLSTR("or")
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
#define NAME CLSTR("out")
	{ NAME, ENC_I, OSZ1(SZ_1), 0, 0,		1,{ 0xE6 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0xE7 } },
	{ NAME, ENC_I, OSZ1(SZ_4), 0, 0,		1,{ 0xE7 } },

	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xEE } },
	{ NAME, ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xEF } },
	{ NAME, ENC_ZO, 0, 0, 0,		1,{ 0xEF } },
#undef NAME

// ----- OUTS/OUTSB/OUTSW/OUTSD -----
	{ CLSTR("outsb"), ENC_ZO, 0, 0, 0,			1,{ 0x6E } },
	{ CLSTR("outsw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x6F } },
	{ CLSTR("outsd"), ENC_ZO, 0, 0, 0,			1,{ 0x6F } },

// ----- POP -----
#define NAME CLSTR("pop")
	{ NAME, ENC_M, OSZ1(SZ_2), 0, OPF_OPXT|OPF_OPSZ,	1,{ 0x8F } },
	{ NAME, ENC_M, OSZ1(SZ_8), 0, OPF_OPXT,				1,{ 0x8F } },

	{ NAME, ENC_O, OSZ1(SZ_2), 0, OPF_OPSZ,	1,{ 0x58 } },
	{ NAME, ENC_O, OSZ1(SZ_8), 0, 0,		1,{ 0x58 } },

	// TODO
#undef NAME

// ----- POPF/POPFQ -----
	{ CLSTR("popf"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x9D } },
	{ CLSTR("popfq"), ENC_ZO, 0, 0, 0,			1,{ 0x9D } },

// ----- PUSH -----
#define NAME CLSTR("push")
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
	{ CLSTR("pushf"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0x9C } },
	{ CLSTR("pushfq"), ENC_ZO, 0, 0, 0,			1,{ 0x9C } },

// ----- RCL/RCR/ROL/ROR -----
#define NAME CLSTR("rcl")
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
#define NAME CLSTR("rcr")
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
#define NAME CLSTR("rol")
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
#define NAME CLSTR("ror")
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
	{ CLSTR("rdmsr"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x32 } },

// ----- RDTSC -----
	{ CLSTR("rdtsc"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x31 } },

// ----- RET -----
#define NAME CLSTR("ret")
	{ NAME, ENC_ZO, 0, 0, 0, 1,{ 0xC3 } },
	{ NAME, ENC_ZO, 0, 0, 0, 1,{ 0xCB } }, // FAR
	{ NAME, ENC_I, OSZ1(SZ_2), 0, 0, 1,{ 0xC2 } },
	{ NAME, ENC_I, OSZ1(SZ_2), 0, 0, 1,{ 0xCA } }, // FAR
#undef NAME

// ----- RSM -----
	{ CLSTR("rsm"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0xAA } },

// ----- SAL/SAR/SHL/SHR -----
#define NAME CLSTR("sal")
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
#define NAME CLSTR("sar")
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
#define NAME CLSTR("shl")
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
#define NAME CLSTR("shr")
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
#define NAME CLSTR("sbb")
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
	{ CLSTR("scasb"), ENC_ZO, 0, 0, 0,			1,{ 0xAE } },
	{ CLSTR("scasw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xAF } },
	{ CLSTR("scasd"), ENC_ZO, 0, 0, 0,			1,{ 0xAF } },
	{ CLSTR("scasq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xAF } },

// ----- SETcc -----
	{ CLSTR("seta"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x97 } },
	{ CLSTR("setae"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x93 } },
	{ CLSTR("setb"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x92 } },
	{ CLSTR("setbe"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x96 } },
	{ CLSTR("setc"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x92 } },
	{ CLSTR("sete"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x94 } },
	{ CLSTR("setg"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9F } },
	{ CLSTR("setge"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9D } },
	{ CLSTR("setl"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9C } },
	{ CLSTR("setle"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9E } },
	{ CLSTR("setna"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x96 } },
	{ CLSTR("setnae"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x92 } },
	{ CLSTR("setnb"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x93 } },
	{ CLSTR("setnbe"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x97 } },
	{ CLSTR("setnc"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x93 } },
	{ CLSTR("setne"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x95 } },
	{ CLSTR("setng"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9E } },
	{ CLSTR("setnge"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9C } },
	{ CLSTR("setnl"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9D } },
	{ CLSTR("setnle"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9F } },
	{ CLSTR("setno"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x91 } },
	{ CLSTR("setnp"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9B } },
	{ CLSTR("setns"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x99 } },
	{ CLSTR("setnz"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x95 } },
	{ CLSTR("seto"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x90 } },
	{ CLSTR("setp"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9A } },
	{ CLSTR("setpe"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9A } },
	{ CLSTR("setpo"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x9B } },
	{ CLSTR("sets"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x98 } },
	{ CLSTR("setz"), ENC_M, OSZ1(SZ_1), 0, 0,	2,{ 0x0F, 0x94 } },

// !! SGDT
// !! SIDT

// ----- SLDT -----
	{ CLSTR("sldt"), ENC_M, OSZ1(SZ_2), 0, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// !! SMSW

// ----- STC -----
	{ CLSTR("stc"), ENC_ZO, 0, 0, 0, 1,{ 0xF9 } },

// ----- STD -----
	{ CLSTR("std"), ENC_ZO, 0, 0, 0, 1,{ 0xFD } },

// ----- STI -----
	{ CLSTR("sti"), ENC_ZO, 0, 0, 0, 1,{ 0xFB } },

// ----- STOS/STOSB/STOSW/STOSD/STOSQ -----
	{ CLSTR("stosb"), ENC_ZO, 0, 0, 0,			1,{ 0xAA } },
	{ CLSTR("stosw"), ENC_ZO, 0, 0, OPF_OPSZ,	1,{ 0xAB } },
	{ CLSTR("stosd"), ENC_ZO, 0, 0, 0,			1,{ 0xAB } },
	{ CLSTR("stosq"), ENC_ZO, 0, 0, OPF_REXW,	1,{ 0xAB } },

// ----- STR -----
	{ CLSTR("str"), ENC_M, OSZ1(SZ_2), 1, OPF_OPXT, 2,{ 0x0F, 0x00 } },

// ----- SUB -----
#define NAME CLSTR("sub")
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
	{ CLSTR("syscall"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x05 } },

// ----- SYSENTER -----
	{ CLSTR("sysenter"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x34 } },

// ----- SYSEXIT -----
	{ CLSTR("sysexit"), ENC_ZO, 0, 0, 0,		2,{ 0x0F, 0x35 } },
	{ CLSTR("sysexit"), ENC_ZO, 0, 0, OPF_REXW,	2,{ 0x0F, 0x35 } },

// ----- SYSRET -----
	{ CLSTR("sysret"), ENC_ZO, 0, 0, 0,			2,{ 0x0F, 0x07 } },
	{ CLSTR("sysret"), ENC_ZO, 0, 0, OPF_REXW,	2,{ 0x0F, 0x07 } },

// ----- TEST -----
#define NAME CLSTR("test")
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
	{ CLSTR("ud0"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0,		2,{ 0x0F, 0xFF } },
	{ CLSTR("ud1"), ENC_RM, OSZ2(SZ_4, SZ_4), 0, 0, 	2,{ 0x0F, 0xB9 } },
	{ CLSTR("ud2"), ENC_ZO, 0, 0, 0,					2,{ 0x0F, 0x0B } },

// ----- WAIT/FWAIT -----
	{ CLSTR("wait"), ENC_ZO, 0, 0, 0,	1,{ 0x9B } },
	{ CLSTR("fwait"), ENC_ZO, 0, 0, 0,	1,{ 0x9B } },

// ----- WRMSR -----
	{ CLSTR("wrmsr"), ENC_ZO, 0, 0, 0, 2,{ 0x0F, 0x30 } },

// ----- XADD -----
#define NAME CLSTR("xadd")
	{ NAME, ENC_MR, OSZ2(SZ_1, SZ_1), 0, 0,			2,{ 0x0F, 0xC0 } },
	{ NAME, ENC_MR, OSZ2(SZ_2, SZ_2), 0, OPF_OPSZ,	2,{ 0x0F, 0xC1 } },
	{ NAME, ENC_MR, OSZ2(SZ_4, SZ_4), 0, 0,			2,{ 0x0F, 0xC1 } },
	{ NAME, ENC_MR, OSZ2(SZ_8, SZ_8), 0, OPF_REXW,	2,{ 0x0F, 0xC1 } },
#undef NAME

// ----- XCHG -----
#define NAME CLSTR("xchg")
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

// !! XLAT/XLATB

// ----- XOR -----
#define NAME CLSTR("xor")
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

