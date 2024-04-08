#include <lt/dwarf.h>
#include <lt/debug.h>
#include <lt/mem.h>

void lt_dwarf_lns_init(lt_dwarf_lns_t m[static 1], const lt_dwarf_stmt_prologue_t p[static 1]) {
	m->addr = 0;
	m->op_index = 0;
	m->file = 1;
	m->line = 1;
	m->column = 0;
	m->is_stmt = p->default_is_stmt;
	m->basic_block = 0;
	m->end_sequence = 0;
	m->prologue_end = 0;
	m->epilogue_begin = 0;
	m->isa = 0;
	m->discriminator = 0;
}

u32 lt_dwarf_uleb128(u8* sp[static 1]) {
	u8 shl = 0;
	u32 i = 0;
	u8 b;
	do {
		b = *(*sp)++;
		i |= ((b & 0x7F) << shl);
		shl += 7;
	} while (b & 0x80);
	return i;
}

i32 lt_dwarf_sleb128(u8* sp[static 1]) {
	u8 shl = 0;
	i32 i = 0;
	u8 b;
	do {
		b = *(*sp)++;
		i |= (b & 0x7F) << shl;
		shl += 7;
	} while (b & 0x80);

	if (shl < 32 && b & 0x40)
		i |= -(1 << shl);
	return i;
}

static LT_INLINE
usz exec_ext_instr(lt_dwarf_lns_t m[static 1], u8* instr) {
	u8* it = instr;
	u32 op_len = lt_dwarf_uleb128(&it);
	u8* op_ptr = it++;
	u8 opcode = *op_ptr;
	switch (opcode) {
	case LT_DWARF_LNE_END_SEQUENCE:								break;
	case LT_DWARF_LNE_SET_DISCRIMINATOR: lt_dwarf_uleb128(&it);	break;

	case LT_DWARF_LNE_SET_ADDRESS:
		memcpy(&m->addr, it, sizeof(u64));
		it += sizeof(u64);
		break;

	default:
		lt_werrf("unknown extended opcode 0x%hb\n", opcode);
		it = op_ptr + op_len;
		break;
	}
	LT_ASSERT(it == op_ptr + op_len);

	return it - instr;
}

usz lt_dwarf_execute(lt_dwarf_lns_t m[static 1], const lt_dwarf_stmt_prologue_t p[static 1], u8* instr) {
	u8* it = instr;
	u8 opcode = *it++;
	switch (opcode) {

	// extended opcodes
	case 0: it += exec_ext_instr(m, it); break;

	// standard opcodes
	case LT_DWARF_LNS_COPY:					break;
	case LT_DWARF_LNS_SET_BASIC_BLOCK:		break;
	case LT_DWARF_LNS_SET_PROLOGUE_END:		break;
	case LT_DWARF_LNS_SET_EPILOGUE_BEGIN:	break;
	case LT_DWARF_LNS_ADVANCE_PC:			m->addr += lt_dwarf_uleb128(&it);	break;
	case LT_DWARF_LNS_ADVANCE_LINE:			m->line += lt_dwarf_sleb128(&it);	break;
	case LT_DWARF_LNS_SET_FILE:				m->file = lt_dwarf_uleb128(&it);		break;
	case LT_DWARF_LNS_SET_COLUMN:			m->column = lt_dwarf_uleb128(&it);	break;
	case LT_DWARF_LNS_NEGATE_STMT:			m->is_stmt = !m->is_stmt;	break;
	case LT_DWARF_LNS_SET_ISA:				lt_dwarf_uleb128(&it);				break;

	case LT_DWARF_LNS_CONST_ADD_PC:
		m->addr += (0xFF - p->opcode_base) / p->line_range * p->min_instr_len;
		break;

	case LT_DWARF_LNS_FIXED_ADVANCE_PC:
		u16 offs;
		memcpy(&offs, it, sizeof(u16));
		m->addr += offs;
		it += sizeof(u16);
		break;

	// special opcodes
	default: {
		u8 adj = opcode - p->opcode_base;
		m->addr += p->min_instr_len * (adj / p->line_range);
		m->line += p->line_base + (adj % p->line_range);
	}	break;
	}

	return it - instr;
}

