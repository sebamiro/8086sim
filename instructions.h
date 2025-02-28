typedef enum type_op
{
	Op_None,
	Op_mov,
} type_op;

char* op_names[2] = {
	"non",
	"mov",
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

instruction_encoding instruction_table_8086[] =
{
	{Op_mov, { B(100010), D, W, MOD, REG, RM } },
	{Op_mov, { B(1100011), W, MOD, B(000), RM, DATA, ImpD(1) } },
	{Op_mov, { B(1011), W, REG, DATA } }
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
	i32						displacement;
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
	type_op	op;
	u16		flags;

	operand	operands[2];
} instruction;
