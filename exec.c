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

u16* get_operand_dest(Memory* mem, operand operand)
{
	if (operand.typ == Operand_None)
	{
		return NULL;
	}
	u16* res = NULL;
	if(operand.typ == Operand_Register)
	{
		res = &mem->registers[register_access_to_index(operand.register_access.index)];
	}
	else if(operand.typ == Operand_Memory)
	{
		u16 off = operand.address.displacement;
		for (u8 iter_terms = 0; iter_terms < 2; ++iter_terms)
		{
			if (operand.address.terms[iter_terms].reg.index)
			{
				off += mem->registers[register_access_to_index(operand.address.terms[iter_terms].reg.index)];
			}
		}
		DEBUG(" LOAD: %d\n", off)
		res = (u16*)(mem->buf + off);
	}
	return res;
}

u16 get_operand_src_value(Memory* mem, operand operand)
{
	assert(operand.typ != Operand_None);

	if (operand.typ == Operand_Inmediate)
	{
		return (u16)operand.inmediate.count;
	}
	else if (operand.typ == Operand_Register)
	{
		u16 src =  mem->registers[register_access_to_index(operand.register_access.index)];
		if (operand.register_access.index < 4)
		{
			return 0xFF & src;
		}
		else if (operand.register_access.index < 8)
		{
			return src >> 8;
		}
		return src;
	}
	else if (operand.typ == Operand_Memory)
	{
		u16 off = operand.address.displacement;
		for (u8 iter_terms = 0; iter_terms < 2; ++iter_terms)
		{
			if (operand.address.terms[iter_terms].reg.index)
			{
				off += mem->registers[register_access_to_index(operand.address.terms[iter_terms].reg.index)];
			}
		}
		return *(u16*)(mem->buf + off);
	}
	return 0;
}

void exec_instruction(Memory* mem, instruction inst)
{
	assert(inst.op);
	u16* pointer_des;
	u16 val_src;

	pointer_des = get_operand_dest(mem, inst.operands[0]);
	val_src = get_operand_src_value(mem, inst.operands[1]);

	if (inst.operands[0].typ != Op_None)
	{
		DEBUG("[%s] = 0x%X", register_names[inst.operands[0].register_access.index], *pointer_des)
	}
	switch (inst.op)
	{
		case Op_mov:
			if (inst.flags & Inst_Wide)
			{
				*pointer_des = val_src;
			}
			else
			{
				if (inst.operands[0].register_access.index < 4)
				{
					DEBUG(" ( [0]: 0x%X )", ((*pointer_des & 0xFF00) | val_src))
					*pointer_des = ((*pointer_des & 0xFF00) | val_src);
				}
				else
				{
					DEBUG(" ( [1]: 0x%X )", ((*pointer_des & 0x00FF) | (val_src << 8)))
					*pointer_des = ((*pointer_des & 0x00FF) | (val_src << 8));
				}
			}
			break;
		case Op_add:
			*pointer_des = *pointer_des + val_src;
			if (*pointer_des == 0)
			{
				mem->registers[IndexRegister_Flags] |= Flag_Zero;
			}
			else
			{
				mem->registers[IndexRegister_Flags] &= ~Flag_Zero;
			}
			if (0x8000 & *pointer_des)
			{
				mem->registers[IndexRegister_Flags] |= Flag_Sign;
			}
			else
			{
				mem->registers[IndexRegister_Flags] &= ~Flag_Sign;
			}
			break;
		case Op_sub:
			*pointer_des = *pointer_des - val_src;
			if (*pointer_des == 0)
			{
				mem->registers[IndexRegister_Flags] |= Flag_Zero;
			}
			else
			{
				mem->registers[IndexRegister_Flags] &= ~Flag_Zero;
			}
			if (0x8000 & *pointer_des)
			{
				mem->registers[IndexRegister_Flags] |= Flag_Sign;
			}
			else
			{
				mem->registers[IndexRegister_Flags] &= ~Flag_Sign;
			}
			break;
		case Op_cmp:
		{
			u16 res = *pointer_des - val_src;
			if (res == 0)
			{
				mem->registers[IndexRegister_Flags] |= Flag_Zero;
			}
			else
			{
				mem->registers[IndexRegister_Flags] &= ~Flag_Zero;
			}
			if (0x8000 & res)
			{
				mem->registers[IndexRegister_Flags] |= Flag_Sign;
			}
			else
			{
				mem->registers[IndexRegister_Flags] &= ~Flag_Sign;
			}
			break;
		}
		case Op_je:
		{
			if (mem->registers[IndexRegister_Flags] & Flag_Zero)
			{
				mem->registers[IndexRegister_IP] += (i8)val_src;
			}
			break;
		}
		case Op_jne:
		{
			if (~mem->registers[IndexRegister_Flags] & Flag_Zero)
			{
				mem->registers[IndexRegister_IP] += (i8)val_src;
			}
			break;
		}
		default:
			DEBUG("NOT IMPLEMENTED EXECUTE\n")
	}

	if (pointer_des)
	{
		DEBUG(" -> 0x%X", *pointer_des)
	}
	DEBUG("; ip: 0x%X", mem->registers[IndexRegister_IP])
	if (mem->registers[IndexRegister_Flags] & Flag_Sign)
	{
		DEBUG(" FlagSign");
	}
	if (mem->registers[IndexRegister_Flags] & Flag_Zero)
	{
		DEBUG(" Flag_Zero");
	}
	DEBUG("\n")
}
