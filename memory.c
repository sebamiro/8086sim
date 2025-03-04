#define MAX_MEMORY 1024 * 1024
typedef struct Memory {
	u8		buf[MAX_MEMORY];
	u16		registers[14];
	u16		cur;
	u16		len;
} Memory;

enum index_registers {
	IndexRegister_Accumulator,
	IndexRegister_Base,
	IndexRegister_Count,
	IndexRegister_Data,
	IndexRegister_StackPointer,
	IndexRegister_BasePointer,
	IndexRegister_SourceIndex,
	IndexRegister_DestinationIndex,
	IndexRegister_CodeSegment,
	IndexRegister_DataSegment,
	IndexRegister_StackSegment,
	IndexRegister_ExtraSegment,
	IndexRegister_IP,
	IndexRegister_Flags,
	IndexRegister_Len,
};
enum flags {
	Flag_Zero = 0x1,
	Flag_Sign = 0x2,
};
typedef struct memory_access {
	Memory*	mem;
	u16		cur;
	u16		off;
} memory_access;


u8 Memory_load_file(Memory* m, char* file_name)
{
	FILE* in;

	in = fopen(file_name, "r");
	if (!in) {
		perror("Loading file:");
		return 1;
	}
	m->len += fread(m->buf + m->cur, 1, MAX_MEMORY, in);
	fclose(in);
	return 0;
}

u8 memory_access_bits_u8(memory_access* m, u8 count) {
	assert(count > 0);
	assert(count <= 8 - m->off);
	u8 res;

	res = (m->mem->buf[m->cur] & (0xFF >> m->off)) >> (8 - m->off - count);
	m->off += count;
	if (m->off >= 8)
	{
		m->off = 0;
		m->cur += 1;
	}
	return res;
}
