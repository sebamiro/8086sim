typedef enum type_op
{
	Op_None,
	Op_mov,
	Op_add,
	Op_sub,
	Op_cmp,
	Op_je,
	Op_jl,
	Op_jle,
	Op_jb,
	Op_jbe,
	Op_jp,
	Op_jo,
	Op_js,
	Op_jne,
	Op_jnl,
	Op_jnle,
	Op_jnb,
	Op_jnbe,
	Op_jnp,
	Op_jno,
	Op_jns,
	Op_loop,
	Op_loopz,
	Op_loopnz,
	Op_jcxz,

} type_op;

char* op_names[] = {
	"non",
	"mov",
	"add",
	"sub",
	"cmp",
	"je",
	"jl",
	"jle",
	"jb",
	"jbe",
	"jp",
	"jo",
	"js",
	"jne",
	"jnl",
	"jnle",
	"jnb",
	"jnbe",
	"jnp",
	"jno",
	"jns",
	"loop",
	"loopz",
	"loopnz",
	"jcxz",
};

typedef enum type_bit
{
	Bit_End,
	Bit_Literal,
	Bit_D,
	Bit_W,
	Bit_S,
	Bit_MOD,
	Bit_REG,
	Bit_RM,
	Bit_SR,
	Bit_Data,
	Bit_Disp,
	Bit_Disp_W,
	Bit_Len,
} type_bit;

typedef struct instruction_bit
{
	type_bit	typ;
	u8			count;
	u8			shift;
	u8			value;
} instruction_bit;

typedef struct instruction_encoding
{
	type_op			op;
	instruction_bit	bits[16];
} instruction_encoding;


#define B(bits) {Bit_Literal, sizeof(#bits)-1, 0, 0b##bits}

#define D		{Bit_D, 1, 0, 0}
#define W		{Bit_W, 1, 0, 0}
#define S		{Bit_S, 1, 0, 0}
#define MOD		{Bit_MOD, 2, 0, 0}
#define REG		{Bit_REG, 3, 0, 0}
#define RM		{Bit_RM, 3, 0, 0}
#define SR		{Bit_SR, 2, 0, 0}
#define DATA	{Bit_Data, 0, 0, 0}
#define DISP	{Bit_Disp, 0, 0, 0}

#define ImpW(Value)		{Bit_W, 0, 0, Value}
#define ImpD(Value)		{Bit_D, 0, 0, Value}
#define ImpMOD(Value)	{Bit_MOD, 0, 0, Value}
#define ImpREG(Value)	{Bit_REG, 0, 0, Value}
#define ImpRM(Value)	{Bit_RM, 0, 0, Value}

#define ADDR	{Bit_Disp, 0, 0, 0}, {Bit_Disp_W, 0, 0, 1}

instruction_encoding instruction_table_8086[] =
{
	{Op_mov, { B(100010), D, W, MOD, REG, RM } },
	{Op_mov, { B(1100011), W, MOD, B(000), RM, DATA, ImpD(0) } },
	{Op_mov, { B(1011), W, REG, DATA, ImpD(1) } },
	{Op_mov, { B(1010000), W, ADDR, ImpREG(0), ImpMOD(0), ImpRM(0b110), ImpD(1) } },
	{Op_mov, { B(1010001), W, ADDR, ImpREG(0), ImpMOD(0), ImpRM(0b110), ImpD(0) } },
	{Op_mov, { B(100011), D, B(0), MOD, B(0), SR, RM, ImpW(1) } },

	{Op_add, { B(000000), D, W, MOD, REG, RM } },
	{Op_add, { B(100000), S, W, MOD, B(000), RM, DATA, ImpD(0) } },
	{Op_add, { B(0000010), W, DATA, ImpREG(0), ImpD(1) } },

	{Op_sub, { B(001010), D, W, MOD, REG, RM } },
	{Op_sub, { B(100000), S, W, MOD, B(101), RM, DATA, ImpD(0) } },
	{Op_sub, { B(0010110), W, DATA, ImpREG(0), ImpD(1) } },

	{Op_cmp, { B(001110), D, W, MOD, REG, RM } },
	{Op_cmp, { B(100000), S, W, MOD, B(111), RM, DATA, ImpD(0) } },
	{Op_cmp, { B(0011110), W, DATA, ImpREG(0), ImpD(1) } },

	{Op_je, { B(01110100), DATA } },
	{Op_jl, { B(01111100), DATA } },
	{Op_jle, { B(01111110), DATA } },
	{Op_jb, { B(01110010), DATA } },
	{Op_jbe, { B(01110110), DATA } },
	{Op_jp, { B(01111010), DATA } },
	{Op_jo, { B(01110000), DATA } },
	{Op_js, { B(01111000), DATA } },
	{Op_jne, { B(01110101), DATA } },
	{Op_jnl, { B(01111101), DATA } },
	{Op_jnle, { B(01111111), DATA } },
	{Op_jnb, { B(01110011), DATA } },
	{Op_jnbe, { B(01110111), DATA } },
	{Op_jnp, { B(01111011), DATA } },
	{Op_jno, { B(01110001), DATA } },
	{Op_jns, { B(01111001), DATA } },
	{Op_loop, { B(11100010), DATA } },
	{Op_loopz, { B(11100001), DATA } },
	{Op_loopnz, { B(11100000), DATA } },
	{Op_jcxz, { B(11100011), DATA } },
};

#undef B
#undef D
#undef MOD
#undef REG
#undef SR

enum registers
{
	Register_al,
	Register_cl,
	Register_dl,
	Register_bl,
	Register_ah,
	Register_ch,
	Register_dh,
	Register_bh,
	Register_ax,
	Register_cx,
	Register_dx,
	Register_bx,
	Register_sp,
	Register_bp,
	Register_si,
	Register_di,
	Register_es,
	Register_cs,
	Register_ss,
	Register_ds,
};

char* register_names[20] = {
	"al",
	"cl",
	"dl",
	"bl",
	"ah",
	"ch",
	"dh",
	"bh",
	"ax",
	"cx",
	"dx",
	"bx",
	"sp",
	"bp",
	"si",
	"di",
	"es",
	"cs",
	"ss",
	"ds",
};

typedef struct register_access
{
	enum registers	index;
} register_access;

typedef struct effective_address_term
{
	register_access reg;
	i16				scale;
} effective_address_term;

typedef struct effective_address
{
	effective_address_term	terms[2];
	u16						displacement;
} effective_address;

typedef struct inmediate
{
	i32	count;
} inmediate;

typedef enum operand_type
{
	Operand_None,
	Operand_Register,
	Operand_Memory,
	Operand_Inmediate,
} operand_type;

typedef struct operand
{
	operand_type	typ;
	union
	{
		effective_address	address;
		register_access		register_access;
		inmediate			inmediate;
	};
} operand;

enum instruction_flag
{
    Inst_Lock = 0x1,
    Inst_Rep = 0x2,
    Inst_Segment = 0x4,
    Inst_Wide = 0x8,
    Inst_Far = 0x10,
};

typedef struct instruction
{
	u32		size;
	type_op	op;
	u16		flags;

	operand	operands[2];
} instruction;
