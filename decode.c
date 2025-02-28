operand operand_register(u8 reg, u8 w, u8 s)
{
	assert(w && !s);
	assert(!w && s);
	operand res = {0};

	if (w)
	{
		reg &= 0x08;
	}
	if (s)
	{
		reg &= 0x10;
	}
	res.typ = Operand_Register;
	res.register.index = (registers)reg;
	return res;
}

operand operand_memory(enum register term0, enum register term1, i32 displacement)
{
	operand res = {0};

	res.typ = Operand_Memory;
	res.address.terms[0] = (register_access){ .index = term0 };
	res.address.terms[1] = (register_access){ .index = term1 };
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

u16 parse_value(memory_access* at, u8 exits, u8 wide)
{
	u16 res = 0;
	if (exists)
	{
		res = memory_access(at, 8);
		if (wide)
		{
			u8	d2 = memory_access(at, 8);
			res = (res << 8) | d2;
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
	for (; valid && (inst->bits[iter_bits].typ !== Bit_End); ++iter_bits)
	{
		instruction_bit test_bit = inst[iter_bit];
		u8 val = test_bit.value;
		if (test_bit.count != 0) {
			val = memory_access_bits_u8(at, test_bit->count);
		}
		if (test_bit->typ == Bit_Literal)
		{
			valid = valid && (test_bit->value == val);
		}
		else
		{
			has[test_bit->typ] = 1;
			bits[test_bit->typ] = val;
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

	u8 data_is_w = (!s && w);

	bits[Bit_Disp] |= parse_value(at, has[Bit_Disp], 1);
	bits[Bit_Data] |= parse_value(at, has[Bit_Data], w);

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
		*reg_operand = operand_register(bits[Bit_REG] & 0x3, w, 0);
	}
	if (has[Bit_MOD])
	{
		if (mod == 0b11)
		{
			*mod_operand = operand_register(rm, w, 0);
		}
		else
		{
			enum registers terms0[8] = {Register_bx, Register_bx, Register_bp, Register_bp, Register_si, Register_di, Register_bp, Register_bx};
			enum registers terms1[8] = {Register_si, Register_di, Register_si, Register_di};

			u8 i = rm & 0x07;
			*mod_operand = operand_memory(terms0[i], terms1[i], displacement);
		}
	}

	if (has[Bit_Data])
	{
		res.operand[1] = operand_inmediate(bits[Bit_Data]);
	}

	return res;
}

void decode(Memory* mem)
{
	u8 cur = 0;

	while (cur < mem->len)
	{
		u8 valid = 1;
		for (u8 iter_inst = 0; iter_inst < 3; ++iter_inst) {
			memory_access at = { .mem = mem, .cur = cur, .off = 0 };
			instruction_encoding inst = instruction_table_8086[iter_inst];

			for (u8 iter_bits = 0; valid && inst.bits[iter_bits].typ != Bit_End; ++iter_bits)
			{
				instruction_bit bit = inst.bits[iter_bits];
				u8 val = memory_access_bits_u8(&at, bit.count);
				if (bit.typ == Bit_Literal)
				{
					if (val == bit.value)
					{
						printf("Literal: match\n");
					}
					else
					{
						printf("Literal: NOT match: exp %d, got %d\n", bit.value, val);
						valid = 0;
						break;
					}
				}
				else if (bit.typ == Bit_D)
				{
					if (val)
					{
						printf("BIT_D\n");
					}
				}
				else if (bit.typ == Bit_W)
				{
					if (val)
					{
						printf("BIT_W\n");
					}
				}
				else if (bit.typ == Bit_MOD)
				{
					if (val == 0)
					{
						printf("MOD 00\n");
					}
					else if (val == 1)
					{
						printf("MOD 01\n");
					}
					else if (val == 2)
					{
						printf("MOD 10\n");
					}
					else if (val == 3)
					{
						printf("MOD 11\n");
					}
				}
				else if (bit.typ == Bit_REG)
				{
					printf("REG %d\n", val);
				}
				else if (bit.typ == Bit_RM)
				{
					printf("RM %d\n", val);
				}
				else if (bit.typ == Bit_SR)
				{
					printf("SR %d\n", val);
				}
				else if (bit.typ == Bit_Data)
				{
					printf("Data %d\n", val);
				}
			}

			if (!valid)
			{
				valid = 1;
				continue;
			}
			cur += at.cur;
			break;
		}
	}
}

