#ifndef LT_DWARF_H
#define LT_DWARF_H 1

#include <lt/err.h>

// Line opcodes
#define LT_DWARF_LNS_COPY 1
#define LT_DWARF_LNS_ADVANCE_PC 2
#define LT_DWARF_LNS_ADVANCE_LINE 3
#define LT_DWARF_LNS_SET_FILE 4
#define LT_DWARF_LNS_SET_COLUMN 5
#define LT_DWARF_LNS_NEGATE_STMT 6
#define LT_DWARF_LNS_SET_BASIC_BLOCK 7
#define LT_DWARF_LNS_CONST_ADD_PC 8
#define LT_DWARF_LNS_FIXED_ADVANCE_PC 9
#define LT_DWARF_LNS_SET_PROLOGUE_END 10
#define LT_DWARF_LNS_SET_EPILOGUE_BEGIN 11
#define LT_DWARF_LNS_SET_ISA 12

// Line extended opcodes
#define LT_DWARF_LNE_END_SEQUENCE 1
#define LT_DWARF_LNE_SET_ADDRESS 2
#define LT_DWARF_LNE_DEFINE_FILE 3
#define LT_DWARF_LNE_SET_DISCRIMINATOR 4

// Line content description
#define LT_DWARF_LNCT_PATH 1
#define LT_DWARF_LNCT_DIR_INDEX 2
#define LT_DWARF_LNCT_TIMESTAMP 3
#define LT_DWARF_LNCT_SIZE 4
#define LT_DWARF_LNCT_MD5 5
#define LT_DWARF_LNCT_LO_USER 0x2000
#define LT_DWARF_LNCT_HI_USER 0x3FFF

// Form encodings
#define LT_DWARF_FORM_ADDR 0x01
#define LT_DWARF_FORM_BLOCK2 0x03
#define LT_DWARF_FORM_BLOCK4 0x04
#define LT_DWARF_FORM_DATA2 0x05
#define LT_DWARF_FORM_DATA4 0x06
#define LT_DWARF_FORM_DATA8 0x07
#define LT_DWARF_FORM_STRING 0x08
#define LT_DWARF_FORM_BLOCK 0x09
#define LT_DWARF_FORM_BLOCK1 0x0A
#define LT_DWARF_FORM_DATA1 0x0B
#define LT_DWARF_FORM_FLAG 0x0C
#define LT_DWARF_FORM_SDATA 0x0D
#define LT_DWARF_FORM_STRP 0x0E
#define LT_DWARF_FORM_UDATA 0x0F
#define LT_DWARF_FORM_REF_ADDR 0x10
#define LT_DWARF_FORM_REF1 0x11
#define LT_DWARF_FORM_REF2 0x12
#define LT_DWARF_FORM_REF4 0x13
#define LT_DWARF_FORM_REF8 0x14
#define LT_DWARF_FORM_REF_UDATA 0x15
#define LT_DWARF_FORM_INDIRECT 0x16
#define LT_DWARF_FORM_SEC_OFFSET 0x17
#define LT_DWARF_FORM_EXPRLOC 0x18
#define LT_DWARF_FORM_FLAG_PRESENT 0x19
#define LT_DWARF_FORM_STRX 0x1A
#define LT_DWARF_FORM_ADDRX 0x1B
#define LT_DWARF_FORM_REF_SUP4 0x1C
#define LT_DWARF_FORM_STRP_SUP 0x1D
#define LT_DWARF_FORM_DATA16 0x1E
#define LT_DWARF_FORM_LINE_STRP 0x1F
#define LT_DWARF_FORM_REF_SIG8 0x20
#define LT_DWARF_FORM_IMPLICIT_CONST 0x21
#define LT_DWARF_FORM_LOCLISTX 0x22
#define LT_DWARF_FORM_RNGLISTX 0x23
#define LT_DWARF_FORM_REF_SUP8 0x24
#define LT_DWARF_FORM_STRX1 0x25
#define LT_DWARF_FORM_STRX2 0x26
#define LT_DWARF_FORM_STRX3 0x27
#define LT_DWARF_FORM_STRX4 0x28
#define LT_DWARF_FORM_ADDRX1 0x29
#define LT_DWARF_FORM_ADDRX2 0x2A
#define LT_DWARF_FORM_ADDRX3 0x2B
#define LT_DWARF_FORM_ADDRX4 0x2C

typedef
struct lt_dwarf_lns {
	usz addr;
	usz op_index;
	usz file;
	usz line;
	usz column;
	b8 is_stmt;
	b8 basic_block;
	b8 end_sequence;
	b8 prologue_end;
	b8 epilogue_begin;
	u32 isa;
	u32 discriminator;
} lt_dwarf_lns_t;

typedef
LT_PACKED_STRUCT(lt_dwarf_stmt_prologue) {
	u32 unit_len;
	u16 version;
	u8 addr_size;
	u8 segment_selector_size;
	u32 header_len;
	u8 min_instr_len;
	u8 max_ops_per_instr;
	u8 default_is_stmt;
	i8 line_base;
	u8 line_range;
	u8 opcode_base;
} lt_dwarf_stmt_prologue_t;

void lt_dwarf_lns_init(lt_dwarf_lns_t* m, lt_dwarf_stmt_prologue_t* p);

u32 lt_dwarf_uleb128(u8** sp);
i32 lt_dwarf_sleb128(u8** sp);

usz lt_dwarf_execute(lt_dwarf_lns_t* m, lt_dwarf_stmt_prologue_t* p, u8* instr);

#endif
