#ifdef TARGET_DEFS_ONLY

#define EM_TCC_TARGET		EM_LC3

#define R_DATA_32		R_LC3_32
#define R_DATA_PTR		R_LC3_16
#define R_JMP_SLOT		R_LC3_NONE
#define R_GLOB_DAT		R_LC3_32
#define R_COPY			R_LC3_NONE
#define R_RELATIVE		R_LC3_NONE

#define R_NUM			R_LC3_NUM

#define ELF_START_ADDR		0x0000
#define ELF_PAGE_SIZE		16

#define PCRELATIVE_DLLPLT	0
#define RELOCATE_DLLPLT		0

#else

// #define DEBUG_RELOC
#include"tcc.h"

int
code_reloc(int reloc_type)
{
	switch (reloc_type) {
	case R_LC3_PCREL_9:
	case R_LC3_NEAR_CALL:
		return 1;

	case R_LC3_16:
	case R_LC3_32:
	case R_LC3_5:
	case R_LC3_8_HI:
	case R_LC3_8_LO:
	case R_LC3_PCREL_5:
		return 0;
	}
	return -1;
}


int
gotplt_entry_type(int reloc_type)
{
	return 0;
}

ST_FUNC unsigned int
create_plt_entry(TCCState *s1, unsigned int got_offset, struct sym_attr *attr)
{
	return -1;
}

ST_FUNC void
relocate_plt(TCCState *s1)
{
	return;
}

static void
check_overflow(TCCState *s1, const char *type, int bits, int n)
{
	if ((n >> bits) + ((n >> (bits - 1)) & 1))
		tcc_warning("%s: relocation overflow", type);
}

void
relocate(TCCState *s1, ElfW_Rel *rel, int type, unsigned char *ptr, addr_t addr,
	 addr_t val)
{
//	int sym_index = ELFW(R_SYM)(rel->r_info), esym_index;
//	ElfW(Sym) *sym = &((ElfW(Sym)*)symtab_section->data)[sym_index];

	switch (type) {
	case R_LC3_NONE:
		tcc_error_noabort("Invalid relocation R_LC3_NONE");
		return;
	case R_LC3_16:
		write16le(ptr, val);
		return;
	case R_LC3_32:
		write32le(ptr, val);
		return;
	case R_LC3_5:
		write16le(ptr, (read16le(ptr) & 0xffe0) | (val & 0x1f));
		return;
	case R_LC3_8_HI:
		write16le(ptr, (read16le(ptr) & 0x00ff) | (val & 0xff00));
		return;
	case R_LC3_8_LO:
		write16le(ptr, (read16le(ptr) & 0xff00) | (val & 0x00ff));
		return;
	case R_LC3_PCREL_5:
		check_overflow(s1, "R_LC3_PCREL_5", 5, val - addr - 1);
		write16le(ptr, (read16le(ptr) & 0xffe0) |
				(((val - addr - 1) >> 1) & 0x1f));
		return;
	case R_LC3_PCREL_9:
		check_overflow(s1, "R_LC3_PCREL_9", 9, val - addr - 1);
		write16le(ptr, (read16le(ptr) & 0xfe00) |
				(((val - addr - 1) >> 1) & 0x01ff));
		return;
	case R_LC3_NEAR_CALL:
		check_overflow(s1, "R_LC3_NEAR_CALL", 11, val - addr - 1);
		write16le(ptr, (read16le(ptr) & 0xf800) |
				(((val - addr - 1) >> 1) & 0x07ff));
		return;
	default:
		tcc_error_noabort("Unimplemented relocation %d", type);
	}

	return;
}

#endif
