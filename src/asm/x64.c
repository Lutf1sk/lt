#include <lt/asm.h>
#include <lt/mem.h>
#include <lt/str.h>

#define SZ_1 0
#define SZ_2 1
#define SZ_4 2
#define SZ_8 3

// ModR/M |    AX..BX    |       SP      |      BP      |  SI/DI  | rm
//--------|--------------|---------------|--------------|---------|
// Mod 00 |     [rm]     |     [SIB]     | [IP + dsp32] |  [rm]   |
//--------|--------------|---------------|--------------|---------|
// Mod 01 | [rm + dsp8]  | [SIB + dsp8]  |      [rm + dsp8]       |
//--------|--------------|---------------|------------------------|
// Mod 10 | [rm + dsp32] | [SIB + dsp32] |      [rm + dsp32]      |
//--------|--------------|---------------|------------------------|
// Mod 11 |                          rm                           |

// MMRRRMMM
#define MODRM(mod, reg, rm) (((mod) << 6) | ((reg) << 3) | (rm))
#define MOD_DREG	0b00
#define MOD_DSP8	0b01
#define MOD_DSP32	0b10
#define MOD_REG		0b11
#define MODRM_MOD(mrm) (((mrm) >> 6) & 0b11)
#define MODRM_REG(mrm) (((mrm) >> 3) & 0b111)
#define MODRM_RM(mrm) ((mrm) & 0b111)

// SIB - Mod 00
//         | AX..SP/SI..R12/R14/R15 |        BP/R13       | B.base
//---------|------------------------|---------------------|
// AX..BX  |  [B.base + X.index*s]  | [X.index*s + dsp32] |
//---------|------------------------|---------------------|
// SP      |         [B.base]       |       [dsp32]       |
//---------|------------------------|---------------------|
// BP..R15 |  [B.base + X.index*s]  | [X.index*s + dsp32] |
// X.index

// SIB - Mod 01/10
//         |         AX..BX/BP..R15         |         SP         | B.base
//---------|--------------------------------|--------------------|
// AX..R15 | [B.base + X.index*s + dsp8/32] | [B.base + dsp8/32] |
// X.index

// SSIIIBBB
#define SIB(scale, index, base) (((scale) << 6) | ((index) << 3) | (base))
#define SIB_S1 0
#define SIB_S2 1
#define SIB_S4 2
#define SIB_S8 3
#define SIB_SCALE(sib) (((sib) >> 6) & 0b11)
#define SIB_INDEX(sib) (((sib) >> 3) & 0b111)
#define SIB_BASE(sib) ((sib) & 0b111)

#include "x64reg.c"
#include "x64pfx.c"
#include "x64op.c"

#include <lt/io.h>

usz parse_mod00_sib(lt_instr_stream_t* stream, u8 rex) {
	u8 sib;
	if (!lt_instr_stream_consume(stream, &sib, 1))
		return 0;
	u8 index = (!!(rex & REX_X) << 3) | SIB_INDEX(sib);
	u8 base = SIB_BASE(sib);
	u8 b_base = (!!(rex & REX_B) << 3) | base;

	if (base == REG_BP && index == REG_SP) {
		u32 dsp;
		if (!lt_instr_stream_consume(stream, &dsp, 4))
			return 0;
		return lt_io_printf(stream->callb, stream->usr, "[0x%hd]", dsp);;
	}
	if (base == REG_BP) {
		i32 dsp;
		if (!lt_instr_stream_consume(stream, &dsp, 4))
			return 0;
		u8 dsp_op = '+';
		if (dsp < 0) {
			dsp = -dsp;
			dsp_op = '-';
		}
		return lt_io_printf(stream->callb, stream->usr, "[%S*%ud %c 0x%hd]", regs[index].sized_names[SZ_8], 1 << SIB_SCALE(sib), dsp_op, dsp);
	}
	if (index == REG_SP)
		return lt_io_printf(stream->callb, stream->usr, "[%S]", regs[b_base].sized_names[SZ_8]);
	return lt_io_printf(stream->callb, stream->usr, "[%S + %S*%ud]", regs[b_base].sized_names[SZ_8], regs[index].sized_names[SZ_8], 1 << SIB_SCALE(sib));
}

usz parse_dsp_sib(lt_instr_stream_t* stream, u8 rex, usz dsp_bytes) {
	u8 sib;
	if (!lt_instr_stream_consume(stream, &sib, 1))
		return 0;
	u32 dsp;
	if (!lt_instr_stream_consume(stream, &dsp, dsp_bytes))
		return 0;

	u8 index = (!!(rex & REX_X) << 3) | SIB_INDEX(sib);
	u8 base = (!!(rex & REX_B) << 3) | SIB_BASE(sib);

	if (base == REG_SP)
		return lt_io_printf(stream->callb, stream->usr, "[%S + 0x%hd]", regs[base].sized_names[SZ_8], dsp);
	else
		return lt_io_printf(stream->callb, stream->usr, "[%S + %S*%ud + 0x%hd]", regs[base].sized_names[SZ_8], regs[index].sized_names[SZ_8], 1 << SIB_SCALE(sib), dsp);
}

usz parse_mrm(lt_instr_stream_t* stream, u8 size, u8 mrm, u8 rex) {
	u8 mod = MODRM_MOD(mrm);
	u8 rm = MODRM_RM(mrm);

	i32 dsp;
	switch (mod) {
	case 0b00:
		if (rm == REG_SP)
			return parse_mod00_sib(stream, rex);
		else if (rm == REG_BP) {
			if (!lt_instr_stream_consume(stream, &dsp, 4))
				return 0;
			if (dsp < 0)
				return lt_io_printf(stream->callb, stream->usr, "[rip - 0x%hd]", -dsp);
			else
				return lt_io_printf(stream->callb, stream->usr, "[rip + 0x%hd]", dsp);
		}
		return lt_io_printf(stream->callb, stream->usr, "[%S]", regs[rm].sized_names[SZ_8]);;

	case 0b01:
		if (rm == REG_SP)
			return parse_dsp_sib(stream, rex, 1);
		if (!lt_instr_stream_consume(stream, &dsp, 1))
			return 0;
		return lt_io_printf(stream->callb, stream->usr, "[%S + 0x%ud]", regs[rm].sized_names[SZ_8], dsp);

	case 0b10:
		if (rm == REG_SP)
			return parse_dsp_sib(stream, rex, 4);
		if (!lt_instr_stream_consume(stream, &dsp, 4))
			return 0;
		return lt_io_printf(stream->callb, stream->usr, "[%S + 0x%ud]", regs[rm].sized_names[SZ_8], dsp);

	case 0b11:
		return lt_io_printf(stream->callb, stream->usr, "%S", regs[rm].sized_names[size]);

	default: LT_ASSERT_NOT_REACHED(); return 0;
	}
}

usz lt_x64_disasm_instr(lt_instr_stream_t* stream) {
	usz lpfx_count = 0;
	u16 lpfx_bits = 0;

	usz len = 0;

	// Prefixes
	u8 v;
	for (;;) {
		if (!lt_instr_stream_read(stream, &v, 1))
			return 0;

		lt_x64_pfx_t* pfx = lpfx_find_by_val(v);
		if (!pfx)
			break;
		++stream->it;

		if (++lpfx_count > 4)
			return 0;
		lpfx_bits |= pfx->bit;
		len += lt_io_printf(stream->callb, stream->usr, "%S ", pfx->name);
	}

	// TODO: Mandatory prefix

	// REX prefix
	u8 rex_present = 0;
	u8 rex = REX(0, 0, 0, 0);
	if ((v & 0xF0) == rex) {
		rex_present = 1;
		rex = v;

		char rex_str_data[8] = "REX", *str_it = rex_str_data + 3;
		if (rex & 0x0F) *str_it++ = '.';
		if (rex & REX_W) *str_it++ = 'W';
		if (rex & REX_R) *str_it++ = 'R';
		if (rex & REX_X) *str_it++ = 'X';
		if (rex & REX_B) *str_it++ = 'B';
		len += lt_io_printf(stream->callb, stream->usr, "%S ", LSTR(rex_str_data, str_it - rex_str_data));
	}

	// Opcode
	usz op_len = 1;
	u8 opcode[3];
	if (!lt_instr_stream_consume(stream, &opcode[0], 1))
		return 0;
	if (opcode[0] == 0x0F) {
		if (!lt_instr_stream_consume(stream, &opcode[1], 1))
			return 0;
		++op_len;
	}

	u8 mrm_avail = 0;
	u8 mrm = 0;
	mrm_avail = lt_instr_stream_read(stream, &mrm, 1);

	for (usz i = 0; i < sizeof(ops) / sizeof(*ops); ++i) {
		lt_x64_op_t* op = &ops[i];
		if (op->op_len != op_len)
			continue;

		if (!!(op->flags & OPF_OPSZ) != !!(lpfx_bits & LPFX_OPSZ))
			continue;
		if (!!(op->flags & OPF_REXW) != !!(rex & REX_R))
			continue;
		if (((op->flags & OPF_REXW) || (op->flags & OPF_REXR)) && !rex_present)
			continue;

		u8 opr_min = op->opcode[op->op_len - 1];
		u8 opr = opcode[op_len - 1];
		if (op->encoding == ENC_O || op->encoding == ENC_OI) {
			if (memcmp(opcode, op->opcode, op_len - 1) != 0)
				continue;
			u8 opr_max = opr_min + 7;
			if (opr < opr_min || opr > opr_max)
				continue;
		}
		else if (memcmp(opcode, op->opcode, op_len) != 0)
			continue;

		if (op->flags & OPF_OPXT) {
			u8 mrm_reg = MODRM_REG(mrm);
			if (!mrm_avail || op->reg != mrm_reg)
				continue;
		}

		len += stream->callb(stream->usr, op->name.str, op->name.len);
		if (op->encoding != ENC_ZO)
			len += stream->callb(stream->usr, " ", 1);

		u8 sz0 = OSZ(op->sizes, 0);
		u8 sz1 = OSZ(op->sizes, 1);
		u8 sz2 = OSZ(op->sizes, 2);

		#define MRM() { \
			if (!mrm_avail) \
				return 0; \
			++stream->it; \
		}
		#define M(n) { \
			usz res = parse_mrm(stream, sz##n, mrm, rex); \
			if (!res) \
				return 0; \
			len += res; \
		}
		#define R(n) { \
			lstr_t name = regs[MODRM_REG(mrm)].sized_names[sz##n]; \
			len += stream->callb(stream->usr, name.str, name.len); \
		}
		#define I(n) { \
			u64 v = 0; \
			usz bytes = 1 << sz##n; \
			if (!lt_instr_stream_read(stream, &v, bytes)) \
				return 0; \
			stream->it += bytes; \
			len += lt_io_printf(stream->callb, stream->usr, "0x%hq", v); \
		}
		#define O(n) { \
			lstr_t name = regs[opr - opr_min].sized_names[sz##n]; \
			len += stream->callb(stream->usr, name.str, name.len); \
		}
		#define COMMA() len += stream->callb(stream->usr, ", ", 2)

		switch (op->encoding) {
		case ENC_ZO: break;
		case ENC_M: MRM(); M(0); break;
		case ENC_I: I(0); break;
		case ENC_O: O(0); break;
		case ENC_D: I(0); break;
		case ENC_MI: MRM(); M(0); COMMA(); I(1); break;
		case ENC_MR: MRM(); M(0); COMMA(); R(1); break;
		case ENC_RM: MRM(); R(0); COMMA(); M(1); break;
		case ENC_II: I(0); COMMA(); I(1); break;
		case ENC_RMI: MRM(); R(0); COMMA(); M(1); COMMA(); I(2); break;
		case ENC_OI: O(0); COMMA(); I(1); break;
		case ENC_M1: MRM(); M(0); len += stream->callb(stream->usr, ", 1", 2); break;
		case ENC_MC: MRM(); M(0); len += lt_io_printf(stream->callb, stream->usr, ", %S", regs[opr - opr_min].sized_names[sz1]); break;
			break;
		}
		return len;
	}

	return 0;
}

