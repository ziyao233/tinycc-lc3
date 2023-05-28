#ifdef TARGET_DEFS_ONLY

#define CONFIG_TCC_ASM
#define NB_ASM_REGS		8

ST_FUNC void g(int c);
ST_FUNC void gen_le16(int c);
ST_FUNC void gen_le32(int c);

#else

#define USING_GLOBALS
#include"tcc.h"

/*	TODO:	Make code generating functions faster	*/

ST_FUNC void
g(int c)
{
	int ind1;
	if (nocode_wanted)
		return;
	ind1 = ind + 1;
	if (ind1 > cur_text_section->data_allocated)
		section_realloc(cur_text_section, ind1);
	cur_text_section->data[ind] = c;
	ind = ind1;
	return;
}

ST_FUNC void
gen_le16(int i)
{
	g(i);
	g(i >> 8);
	return;
}

ST_FUNC void
gen_le32(int i)
{
	int ind1;
	g(i);
	g((i >> 8) & 0xff);
	g((i >> 16) & 0xff);
	g((i >> 24) & 0xff);
	return;
}

ST_FUNC void
gen_expr32(ExprValue *pe)
{
	gen_le32(pe->v);
}

ST_FUNC void
asm_opcode(TCCState *s1, int token)
{}

ST_FUNC void
subst_asm_operand(CString *add_str, SValue *sv, int modifier)
{
	tcc_error("LC3 inline asm not implmented");
}

ST_FUNC void
asm_gen_code(ASMOperand *operands, int nb_operands, int nb_outputs,
	     int is_output, uint8_t *clobber_regs, int out_reg)
{}

ST_FUNC void
asm_compute_constraints(ASMOperand *operands, int nb_operands, int nb_outputs,
			const uint8_t *clobber_regs, int *pout_reg)
{}

ST_FUNC void
asm_clobber(uint8_t *clobber_regs, const char *str)
{}

ST_FUNC int
asm_parse_regvar(int t)
{
	return -1;
}

#endif
