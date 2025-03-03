enum index_registers register_access_to_index(enum registers val)
{
	switch (val)
	{
		case Register_al:
		case Register_ah:
		case Register_ax:
			return IndexRegister_Accumulator;
		case Register_bl:
		case Register_bh:
		case Register_bx:
			return IndexRegister_Base;
		case Register_cl:
		case Register_ch:
		case Register_cx:
			return IndexRegister_Count;
		case Register_dl:
		case Register_dh:
		case Register_dx:
			return IndexRegister_Data;
		case Register_sp:
			return IndexRegister_StackPointer;
		case Register_bp:
			return IndexRegister_BasePointer;
		case Register_si:
			return IndexRegister_SourceIndex;
		case Register_di:
			return IndexRegister_DestinationIndex;
		case Register_es:
			return IndexRegister_ExtraSegment;
		case Register_ss:
			return IndexRegister_StackSegment;
		case Register_cs:
			return IndexRegister_CodeSegment;
		case Register_ds:
			return IndexRegister_DataSegment;
	}
}

u8* get_operand_dest(Memory* mem, operand operand)
{
	assert(operand.typ == Operand_Register);
	u16* reg = &mem->registers[register_access_to_index(operand.register_access.index)];
	if (operand.register_access.index & 0x4)
	{
		return ((u8*)reg) + 1;
	}
	return (u8*)reg;
}

u16 get_operand_src_value(Memory* mem, operand operand)
{
	assert(operand.typ != Operand_None);
	assert(operand.typ != Operand_Memory);
	(void)mem;

	if (operand.typ == Operand_Inmediate)
	{
		return (u16)operand.inmediate.count;
	}
	return 0;
}

void exec_instruction(Memory* mem, instruction inst)
{
	assert(inst.op);
	u8* pointer_des;
	u16 val_src;

	pointer_des = get_operand_dest(mem, inst.operands[0]);
	val_src = get_operand_src_value(mem, inst.operands[1]);

	DEBUG("[%s] = %x -> %x\n", register_names[inst.operands[0].register_access.index], *pointer_des, val_src)
	switch (inst.op)
	{
		case Op_mov:
			if (inst.flags & Inst_Wide)
			{
				u16* u16pointer = (u16*)pointer_des;
				*u16pointer = val_src;
			}
			else
			{
				*pointer_des = (u8)val_src;
			}
			break;
		default:
			DEBUG("NOT IMPLEMENTED EXECUTE\n")
	}
}
