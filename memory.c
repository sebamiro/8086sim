#define MAX_MEMORY 1024 * 1024
typedef struct Memory {
	u8	buf[MAX_MEMORY];
	u8	len;
} Memory;

typedef struct memory_access {
	Memory*	mem;
	u8		cur;
	u8		off;
} memory_access;

u8 Memory_load_file(Memory* m, char* file_name)
{
	FILE* in;

	in = fopen(file_name, "r");
	if (!in) {
		perror("Loading file:");
		return 1;
	}
	m->len += fread(m->buf, 1, MAX_MEMORY, in);
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
