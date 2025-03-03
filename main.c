#include "type.h"
#include "instructions.h"

#include <stdio.h>
#include <assert.h>

#ifdef DEBUG
# undef DEBUG
# define DEBUG(...) fprintf(stderr, __VA_ARGS__);
#endif

#include "memory.c"
#include "exec.c"
#include "decode.c"

int main(int argc, char** argv)
{
	argc--; argv++;
	Memory mem = {0};
	if (!argc)
	{
		fprintf(stderr, "Missing file\n");
		return 1;
	}

	mem.registers = (u16*)mem.buf;
	mem.cur += 28;
	mem.len += mem.cur;
	if (Memory_load_file(&mem, *argv))
	{
		return 1;
	}
	decode(&mem);
}
