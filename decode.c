operand operand_register(u8 reg, u8 w, u8 s)
{
	operand res = {0};

	if (w)
	{
		reg |= 0b1000;
	}
	if (s)
	{
		// reg &= 0x10;
	}
	res.typ = Operand_Register;
	res.register_access.index = reg;
	return res;
}

operand operand_memory(enum registers term0, enum registers term1, i32 displacement)
{
	operand res = {0};

	res.typ = Operand_Memory;
	res.address.terms[0] = (effective_address_term){ { .index = term0 }, 0 };
	res.address.terms[1] = (effective_address_term){ { .index = term1 }, 0 };
	res.address.displacement = displacement;
	return res;
}

operand operand_inmediate(i16 data)
{
	operand res = {0};

	res.typ = Operand_Inmediate;
	res.inmediate.count = data;
	return res;
}

u16 parse_value(memory_access* at, u8 exists, u8 wide)
{
	u16 res = 0;
	if (exists)
	{
		res = memory_access_bits_u8(at, 8);
		if (wide)
		{
			u8	d2 = memory_access_bits_u8(at, 8);
			res = (d2 << 8) | res;
		}
	}
	return res;
}

instruction try_decode_instruction(memory_access* at, instruction_encoding* inst) {
	instruction	res = {0};

	u8	valid = 1;
	u8	has[Bit_Len] = {0};
	u16	bits[Bit_Len] = {0};

	u32 iter_bits = 0;
	for (; valid && (inst->bits[iter_bits].typ != Bit_End); ++iter_bits)
	{
		instruction_bit test_bit = inst->bits[iter_bits];
		u8 val = test_bit.value;
		if (test_bit.count != 0) {
			val = memory_access_bits_u8(at, test_bit.count);
		}
		if (test_bit.typ == Bit_Literal)
		{
			valid = valid && (test_bit.value == val);
		}
		else
		{
			has[test_bit.typ] = 1;
			bits[test_bit.typ] = val;
		}
	}

	if (!valid)
	{
		return res;
	}

	u8 w = bits[Bit_W];
	u8 d = bits[Bit_D];
	u8 s = bits[Bit_S];
	u8 rm = bits[Bit_RM];
	u8 mod = bits[Bit_MOD];

	u8 has_direct_address = ((mod == 0b00) && (rm == 0b110));
	has[Bit_Disp] = ((has[Bit_Disp]) || (mod == 0b01) || (mod == 0b10) || has_direct_address);

	u8 displacement_is_w = (has[Bit_Disp_W] || (mod == 0b10) || has_direct_address);
	u8 data_is_w = (!s && w);

	bits[Bit_Disp] |= parse_value(at, has[Bit_Disp], displacement_is_w);
	bits[Bit_Data] |= parse_value(at, has[Bit_Data], data_is_w);

	res.op = inst->op;

	if (w)
	{
		res.flags |= Inst_Wide;
	}

	i16 displacement = (i16)bits[Bit_Disp];

	operand*	mod_operand = &res.operands[d ? 1 : 0];
	operand*	reg_operand = &res.operands[d ? 0 : 1];

	if (has[Bit_SR])
	{
		*reg_operand = operand_register(bits[Bit_SR] & 0x3, 0, 1);
	}
	if (has[Bit_REG])
	{
		*reg_operand = operand_register(bits[Bit_REG] & 0x7, w, 0);
	}
	if (has[Bit_MOD])
	{
		if (mod == 0b11)
		{
			*mod_operand = operand_register(rm & 0x7, w, 0);
		}
		else
		{
			enum registers terms0[8] = {Register_bx, Register_bx, Register_bp, Register_bp, Register_si, Register_di, Register_bp, Register_bx};
			enum registers terms1[8] = {Register_si, Register_di, Register_si, Register_di};

			u8 i = rm & 0x07;
			enum registers term0 = terms0[i];
			enum registers term1 = terms1[i];
			if ((mod == 0b00) && (rm == 0b110))
			{
				term0 = 0;
				term1 = 0;
			}
			*mod_operand = operand_memory(term0, term1, displacement);
		}
	}

	if (has[Bit_Data])
	{
		res.operands[1] = operand_inmediate(bits[Bit_Data]);
	}

	return res;
}

void print_instuction(instruction inst)
{
	fprintf(stdout, "%s ", op_names[inst.op]);
	for (i32 i = 0; i < 2; i++)
	{
		operand operand = inst.operands[i];
		switch (operand.typ)
		{
			case Operand_Register:
				fprintf(stdout, "%s", register_names[operand.register_access.index]);
				break;
			case Operand_Memory:
				fprintf(stdout, "[");
				if (operand.address.terms[0].reg.index != 0)
				{
					fprintf(stdout, "%s", register_names[operand.address.terms[0].reg.index]);
				}
				if (operand.address.terms[1].reg.index != 0)
				{
					fprintf(stdout, " + %s", register_names[operand.address.terms[1].reg.index]);
				}
				if (operand.address.displacement != 0)
				{
					if (operand.address.terms[0].reg.index != 0)
					{
						fprintf(stdout, " + ");
					}
					fprintf(stdout, "%d", operand.address.displacement);
				}
				fprintf(stdout, "]");
				break;
			case Operand_Inmediate:
				if (inst.operands[0].typ != Operand_Register)
				{
					fprintf(stdout, "%s ", inst.flags & Inst_Wide ? "word" : "byte");
				}
				fprintf(stdout, "%d", operand.inmediate.count);
				break;
			default:
				fprintf(stdout, "OO");
		}
		fprintf(stdout, i ? "\n" : ", ");
	}
}

void decode(Memory* mem)
{
	u8 cur = 0;

	while (cur < mem->len)
	{
		instruction instruction;
		for (u8 iter_inst = 0; iter_inst < 38; ++iter_inst) {
			memory_access at = { .mem = mem, .cur = cur, .off = 0 };
			instruction_encoding inst = instruction_table_8086[iter_inst];

			instruction = try_decode_instruction(&at, &inst);
			if (instruction.op == Op_None)
			{
				continue;
			}
			instruction.size = at.cur - cur;
			cur += instruction.size;
			print_instuction(instruction);
			break;
		}
		if (instruction.op == Op_None)
		{
			fprintf(stderr, "[ERROR] Unknown instruction\n");
			return;
		}
	}
}
