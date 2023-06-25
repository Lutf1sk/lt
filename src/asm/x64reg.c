
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
	{ CLSTRI("ax"), { CLSTRI("al"), CLSTRI("ax"), CLSTRI("eax"), CLSTRI("rax") } },
	{ CLSTRI("cx"), { CLSTRI("cl"), CLSTRI("cx"), CLSTRI("ecx"), CLSTRI("rcx") } },
	{ CLSTRI("dx"), { CLSTRI("dl"), CLSTRI("dx"), CLSTRI("edx"), CLSTRI("rdx") } },
	{ CLSTRI("bx"), { CLSTRI("bl"), CLSTRI("bx"), CLSTRI("ebx"), CLSTRI("rbx") } },
	{ CLSTRI("sp"), { CLSTRI("spl"), CLSTRI("sp"), CLSTRI("esp"), CLSTRI("rsp") } },
	{ CLSTRI("bp"), { CLSTRI("bpl"), CLSTRI("bp"), CLSTRI("ebp"), CLSTRI("rbp") } },
	{ CLSTRI("si"), { CLSTRI("sil"), CLSTRI("si"), CLSTRI("esi"), CLSTRI("rsi") } },
	{ CLSTRI("di"), { CLSTRI("dil"), CLSTRI("di"), CLSTRI("edi"), CLSTRI("rdi") } },
	{ CLSTRI("r8"), { CLSTRI("r8b"), CLSTRI("r8w"), CLSTRI("r8d"), CLSTRI("r8") } },
	{ CLSTRI("r9"), { CLSTRI("r9b"), CLSTRI("r9w"), CLSTRI("r9d"), CLSTRI("r9") } },
	{ CLSTRI("r10"), { CLSTRI("r10b"), CLSTRI("r10w"), CLSTRI("r10d"), CLSTRI("r10") } },
	{ CLSTRI("r11"), { CLSTRI("r11b"), CLSTRI("r11w"), CLSTRI("r11d"), CLSTRI("r11") } },
	{ CLSTRI("r12"), { CLSTRI("r12b"), CLSTRI("r12w"), CLSTRI("r12d"), CLSTRI("r12") } },
	{ CLSTRI("r13"), { CLSTRI("r13b"), CLSTRI("r13w"), CLSTRI("r13d"), CLSTRI("r13") } },
	{ CLSTRI("r14"), { CLSTRI("r14b"), CLSTRI("r14w"), CLSTRI("r14d"), CLSTRI("r14") } },
	{ CLSTRI("r15"), { CLSTRI("r15b"), CLSTRI("r15w"), CLSTRI("r15d"), CLSTRI("r15") } },
};

