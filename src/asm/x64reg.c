
#define REG_A	0b0000
#define REG_C	0b0001
#define REG_D	0b0010
#define REG_B	0b0011
#define REG_SP	0b0100
#define REG_BP	0b0101
#define REG_SI	0b0110
#define REG_DI	0b0111
#define REG_8	0b1000
#define REG_9	0b1001
#define REG_10	0b1010
#define REG_11	0b1011
#define REG_12	0b1100
#define REG_13	0b1101
#define REG_14	0b1110
#define REG_15	0b1111

#define REG_REX_BIT 0b1000

#define REG_COUNT 16

typedef
struct lt_x64_register {
	lstr_t name;
	lstr_t sized_names[4];
} lt_x64_register_t;

lt_x64_register_t regs[REG_COUNT] = {
	{ CLSTR("ax"),	{ CLSTR("al"), CLSTR("ax"), CLSTR("eax"), CLSTR("rax") } },
	{ CLSTR("cx"),	{ CLSTR("cl"), CLSTR("cx"), CLSTR("ecx"), CLSTR("rcx") } },
	{ CLSTR("dx"),	{ CLSTR("dl"), CLSTR("dx"), CLSTR("edx"), CLSTR("rdx") } },
	{ CLSTR("bx"),	{ CLSTR("bl"), CLSTR("bx"), CLSTR("ebx"), CLSTR("rbx") } },
	{ CLSTR("sp"),	{ CLSTR("spl"), CLSTR("sp"), CLSTR("esp"), CLSTR("rsp") } },
	{ CLSTR("bp"),	{ CLSTR("bpl"), CLSTR("bp"), CLSTR("ebp"), CLSTR("rbp") } },
	{ CLSTR("si"),	{ CLSTR("sil"), CLSTR("si"), CLSTR("esi"), CLSTR("rsi") } },
	{ CLSTR("di"),	{ CLSTR("dil"), CLSTR("di"), CLSTR("edi"), CLSTR("rdi") } },
	{ CLSTR("r8"),	{ CLSTR("r8b"), CLSTR("r8d"), CLSTR("r8w"), CLSTR("r8") } },
	{ CLSTR("r9"),	{ CLSTR("r9b"), CLSTR("r9d"), CLSTR("r9w"), CLSTR("r9") } },
	{ CLSTR("r10"),	{ CLSTR("r10b"), CLSTR("r10d"), CLSTR("r10w"), CLSTR("r10") } },
	{ CLSTR("r11"),	{ CLSTR("r11b"), CLSTR("r11d"), CLSTR("r11w"), CLSTR("r11") } },
	{ CLSTR("r12"),	{ CLSTR("r12b"), CLSTR("r12d"), CLSTR("r12w"), CLSTR("r12") } },
	{ CLSTR("r13"),	{ CLSTR("r13b"), CLSTR("r13d"), CLSTR("r13w"), CLSTR("r13") } },
	{ CLSTR("r14"),	{ CLSTR("r14b"), CLSTR("r14d"), CLSTR("r14w"), CLSTR("r14") } },
	{ CLSTR("r15"),	{ CLSTR("r15b"), CLSTR("r15d"), CLSTR("r15w"), CLSTR("r15") } },
};

