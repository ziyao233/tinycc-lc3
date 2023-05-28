#ifdef TARGET_DEFS_ONLY

#define NB_REGS		8
#define NB_ASM_REGS	8
#define CONFIG_TCC_ASM

#define TREG_R(x) (x)

#define RC_INT		(1 << 0)
#define RC_FLOAT	(1 << 1)
#define RC_R(x)		(1 << (2 + (x)))
#define RC_F(x)		(1 << 15)

#define RC_IRET		RC_R(0)
#define RC_IRE2		RC_R(1)
#define RC_FRET		RC_R(0)

#define REG_IRET	TREG_R(0)
#define REG_IRE2	TREG_R(1)
#define REG_FRET	TREG_R(0)

#define PTR_SIZE 4

#define LDOUBLE_SIZE	8
#define LDOUBLE_ALIGN	8

#define MAX_ALIGN	16

#define CHAR_IS_UNSIGNED

#else
#define USING_GLOBALS
#include<assert.h>
#include"tcc.h"

ST_DATA const char * const target_machine_defs =
	"__lc3\0"
	;

ST_FUNC void gsym_addr
(int t, int a) {}

ST_FUNC void
load(int r, SValue *sv) {}

ST_FUNC void
store(int r, SValue *v) {}

ST_FUNC int
gfunc_sret(CType *vt, int variadic, CType *ret, int *align, int *regsize)
{ return 0; }

ST_FUNC void
gfunc_call(int nb_args)
{}

ST_FUNC void
gfunc_prolog(Sym *func_sym)
{}

ST_FUNC void
gfunc_epilog(void)
{}

ST_FUNC void
gen_fill_nops(int size)
{}

ST_FUNC int
gjmp(int t)
{ return 0; }

ST_FUNC void
gjmp_addr(int a)
{}

ST_FUNC int
gjmp_cond(int op, int t)
{ return 0; }

ST_FUNC int
gjmp_append(int n, int t)
{ return 0; }

ST_FUNC void
gen_opi(int op)
{}

ST_FUNC void
gen_opf(int op)
{}

ST_FUNC void
gen_cvt_ftoi(int t)
{}

ST_FUNC void
gen_cvt_itof(int t)
{}

ST_FUNC void
gen_cvt_ftof(int t)
{}

ST_FUNC void
ggoto(void)
{}

ST_FUNC void
gen_vla_sp_save(int addr)
{}

ST_FUNC void
gen_vla_sp_restore(int addr)
{}

ST_FUNC void
gen_vla_alloc(CType *type, int align)
{}

#endif
