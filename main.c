#include "type.h"
#include "instructions.h"

#include <stdio.h>
#include <assert.h>

#include "memory.c"
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
	if (Memory_load_file(&mem, *argv))
	{
		return 1;
	}
	decode(&mem);
}
