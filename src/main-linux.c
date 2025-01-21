#include "asmref/syscall-linux-x86_64.h"

static void *
my_mmap(usz size)
{
	return sys_mmap(0, size, 0x3, 0x22, -1, 0);
}

static void
my_write(buf8 *b)
{
	sys_write((isz)b->fd, b->data, b->len); 
}

i32
main(i32 argc,
     c7 **argv,
     c7 **envp)
{
	buf8  b = {0};
	void *buf_top;
	usz   buf_size;

	buf_size= KB(4);
	b.cap  = KB(4);

	b.data  = my_mmap(buf_size + b.cap);
	buf_top = b.data + b.cap;

	b.fd = 0;             // Explicitly write to standard output
	os_write = my_write;
//	test1_print_main_arguments(&b, argc, argv, envp)
	test2_print_cpuid_information(&b, buf_top, buf_size);

	sys_munmap(b.data, buf_size + b.cap);
	return 0;
}

