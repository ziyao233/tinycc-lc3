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
	int ind1 = ind + 1;
	if (nocode_wanted)
		return;
	if (ind1 > cur_text_section->data_allocated)
		section_realloc(cur_text_section, ind1);
	cur_text_section->data[ind] = c & 0xff;
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

#define inst_op(op)	((op) << 12)
#define inst_dr(r)	(((r) & 7) << 9)
#define inst_sr1(r)	(((r) & 7) << 6)
#define inst_base(r)	inst_sr1(r)
#define inst_imm(t)	((t != 0) << 5)
#define inst_sr2(r)	(((r) & 7) << 0)
#define inst_imm5(i)	(((i) & 0x1f) << 0)
#define inst_brflag(f)	inst_dr(f)

static int
parse_reg(void)
{
	int reg = asm_parse_regvar(tok);
	if (reg < 0)
		expect("register");
	next();
	return reg;
}

ST_FUNC void
asm_op_compute(TCCState *s1, int op)
{
	int inst = op == TOK_ASM_add ? inst_op(1) : inst_op(5);
	inst |= inst_dr(parse_reg());
	skip(',');
	inst |= inst_sr1(parse_reg());
	skip(',');

	if (asm_parse_regvar(tok) > 0) {
		inst |= inst_imm(0);
		inst |= inst_sr2(asm_parse_regvar(tok));
		next();
	} else {
		ExprValue ev;
		asm_expr(s1, &ev);
		if (ev.sym)
			expect("5-bit immediate");
		inst |= inst_imm(1);
		inst |= inst_imm5(ev.v);
	}
	gen_le16(inst);
}

static void
asm_op_branch(TCCState *s1, int token)
{
	const int flags[] = { 7, 4, 2, 1, 6, 5, 3, 7 };
	int inst = inst_op(0) | inst_brflag(flags[token - TOK_ASM_br]);

	ExprValue ev;
	asm_expr(s1, &ev);
	if (ev.sym)
		greloc(cur_text_section, ev.sym, ind, R_LC3_PCREL_9);
	else
		inst |= ev.v & 0x01ff;
	gen_le16(inst);
	return;
}

static void
asm_op_jmp(TCCState *s1, int token)
{
	gen_le16(inst_op(token == TOK_ASM_jmp ? 12 : 4)  | inst_base(parse_reg()));
	return;
}

static void
asm_op_nearcall(TCCState *s1, int token)
{
	ExprValue ev;
	asm_expr(s1, &ev);

	if (ev.sym)
		greloc(cur_text_section, ev.sym, ind, R_LC3_NEAR_CALL);

	gen_le16(inst_op(4) | (1 << 11) | (ev.v & 0x07ff));
	return;
}

static void
asm_op_raddress(TCCState *s1, int token)
{
	int inst = inst_op(token == TOK_ASM_ld	? 2	:
			   token == TOK_ASM_ldi	? 10	:
			   token == TOK_ASM_st	? 3	:
			   token == TOK_ASM_sti ? 11	:
			   			  14);
	ExprValue ev;

	inst |= inst_dr(parse_reg());
	skip(',');

	asm_expr(s1, &ev);
	if (ev.sym)
		greloc(cur_text_section, ev.sym, ind, R_LC3_PCREL_9);
	gen_le16(inst | (ev.v & 0x01ff));

	return;
}

static void
asm_op_baddress(TCCState *s1, int token)
{
	int inst = inst_op(token == TOK_ASM_ldr ? 6 : 7);
	ExprValue ev;
	inst |= inst_dr(parse_reg());

	skip(',');
	asm_expr(s1, &ev);
	if (ev.sym)
		expect("6-bit offset");
	inst |= 0x3f & ev.v;

	skip('(');
	inst |= parse_reg();
	skip(')');
	gen_le16(inst);

	return;
}

static void
asm_op_not(TCCState *s1, int token)
{
	int inst = inst_op(9) | 0x3f;
	inst |= inst_dr(parse_reg());
	skip(',');
	inst |= inst_sr1(parse_reg());
	gen_le16(inst);
	return;
}

static void
asm_op_trap(TCCState *s1, int token)
{
	ExprValue ev;
	asm_expr(s1, &ev);

	if (ev.sym)
		expect("trap vector");

	gen_le16(inst_op(15) | (ev.v & 0xff));
	return;
}

ST_FUNC void
asm_opcode(TCCState *s1, int token)
{
	switch (token) {
	case TOK_ASM_add:
	case TOK_ASM_and:
		asm_op_compute(s1, token);
		return;
	case TOK_ASM_br:
	case TOK_ASM_brn:
	case TOK_ASM_brz:
	case TOK_ASM_brp:
	case TOK_ASM_brnz:
	case TOK_ASM_brnp:
	case TOK_ASM_brzp:
	case TOK_ASM_brnzp:
		asm_op_branch(s1, token);
		return;
	case TOK_ASM_jmp:
	case TOK_ASM_jsrr:
		asm_op_jmp(s1, token);
		return;
	case TOK_ASM_jsr:
		asm_op_nearcall(s1, token);
		return;
	case TOK_ASM_ld:
	case TOK_ASM_ldi:
	case TOK_ASM_lea:
	case TOK_ASM_st:
	case TOK_ASM_sti:
		asm_op_raddress(s1, token);
		return;
	case TOK_ASM_ldr:
	case TOK_ASM_str:
		asm_op_baddress(s1, token);
		return;
	case TOK_ASM_not:
		asm_op_not(s1, token);
		return;
	case TOK_ASM_trap:
		asm_op_trap(s1, token);
		return;
	case TOK_ASM_ret:
		gen_le16(0xc100);
		return;
	case TOK_ASM_rti:
		gen_le16(0x8000);
		return;
	default:
		expect("known opcode");
	}
	return;
}

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
	return t >= TOK_ASM_r0 && t <= TOK_ASM_ra ?
			t == TOK_ASM_ra ? 7			:
			t == TOK_ASM_sp ? 6			:
					  t - TOK_ASM_r0	:
			-1;
}

#endif
