
#define LPFX_LOCK		0xF0
#define LPFX_REPNZ		0xF2
#define LPFX_REP		0xF3
#define LPFX_CS_OVERR	0x2E
#define LPFX_SS_OVERR	0x36
#define LPFX_DS_OVERR	0x3E
#define LPFX_ES_OVERR	0x26
#define LPFX_FS_OVERR	0x64
#define LPFX_GS_OVERR	0x65
#define LPFX_BTAKEN		0x2E
#define LPFX_NOBTAKEN	0x3E
#define LPFX_OPSZ		0x66
#define LPFX_ADDRSZ		0x67

#define LPFX_LOCK_BIT		0x0001
#define LPFX_REPNZ_BIT		0x0002
#define LPFX_REP_BIT		0x0004
#define LPFX_CS_OVERR_BIT	0x0008
#define LPFX_SS_OVERR_BIT	0x0010
#define LPFX_DS_OVERR_BIT	0x0020
#define LPFX_ES_OVERR_BIT	0x0040
#define LPFX_FS_OVERR_BIT	0x0080
#define LPFX_GS_OVERR_BIT	0x0100
#define LPFX_BTAKEN_BIT		0x0200
#define LPFX_NOBTAKEN_BIT	0x0400
#define LPFX_OPSZ_BIT		0x0800
#define LPFX_ADDRSZ_BIT		0x1000

#define LPFX_COUNT 13

// 0100WRXB
// W - Promotes to 64-bit
// R - Extends ModRM reg
// X - Extends SIB index
// B - Extends ModRM rm
#define REX(w, r, x, b) ((0b0100 << 4) | ((w) << 3) | ((r) << 2) | ((x) << 1) | (b))
#define REX_W (1 << 3)
#define REX_R (1 << 2)
#define REX_X (1 << 1)
#define REX_B (1 << 0)

typedef
struct lt_x64_pfx {
	lstr_t name;
	u8 val;
	u16 bit;
} lt_x64_pfx_t;

static
lt_x64_pfx_t legacy_pfxs[LPFX_COUNT] = {
	{ CLSTRI("lock"),				LPFX_LOCK,		LPFX_LOCK_BIT		},
	{ CLSTRI("repnz"),				LPFX_REPNZ,		LPFX_REPNZ_BIT		},
	{ CLSTRI("rep"),				LPFX_REP,		LPFX_REP_BIT		},
	{ CLSTRI("cs"),					LPFX_CS_OVERR,	LPFX_CS_OVERR_BIT	},
	{ CLSTRI("ss"),					LPFX_SS_OVERR,	LPFX_SS_OVERR_BIT	},
	{ CLSTRI("ds"),					LPFX_DS_OVERR,	LPFX_DS_OVERR_BIT	},
	{ CLSTRI("es"),					LPFX_ES_OVERR,	LPFX_ES_OVERR_BIT	},
	{ CLSTRI("fs"),					LPFX_FS_OVERR,	LPFX_FS_OVERR_BIT	},
	{ CLSTRI("gs"),					LPFX_GS_OVERR,	LPFX_GS_OVERR_BIT	},
	{ CLSTRI("branch_taken"),		LPFX_BTAKEN,	LPFX_BTAKEN_BIT		},
	{ CLSTRI("branch_not_taken"),	LPFX_NOBTAKEN,	LPFX_NOBTAKEN_BIT	},
	{ CLSTRI("opsz"),				LPFX_OPSZ,		LPFX_OPSZ_BIT		},
	{ CLSTRI("addrsz"),				LPFX_ADDRSZ,	LPFX_ADDRSZ_BIT		},
};

static
lt_x64_pfx_t* lpfx_find_by_val(u8 v) {
	for (usz i = 0; i < LPFX_COUNT; ++i)
		if (legacy_pfxs[i].val == v)
			return &legacy_pfxs[i];
	return NULL;
}

static
lt_x64_pfx_t* lpfx_find_by_name(lstr_t name) {
	for (usz i = 0; i < LPFX_COUNT; ++i)
		if (lt_lseq(legacy_pfxs[i].name, name))
			return &legacy_pfxs[i];
	return NULL;
}

