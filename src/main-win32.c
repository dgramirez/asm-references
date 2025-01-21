#include <windows.h>

#ifdef _ZIG
	unsigned int _tls_index = 0;
#endif

static void
win32_write(buf8 *b)
{
	DWORD written;
	WriteFile((HANDLE)b->fd, b->data, (DWORD)b->len, &written, 0);
}

i32
_start()
{
	buf8  b = {0};
	void *buf_top;
	usz   buf_size;

	buf_size = KB(4);
	b.cap  = KB(4);

	b.data  = VirtualAlloc(0,
	                       b.cap + buf_size,
	                       MEM_COMMIT | MEM_RESERVE,
	                       PAGE_READWRITE);
	buf_top = b.data + b.cap;

	b.fd = GetStdHandle(STD_OUTPUT_HANDLE);
	os_write = win32_write;
	test2_print_cpuid_information(&b, buf_top, buf_size);

	return 0;
}

