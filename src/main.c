#include "tests.c"

#define KB(x) ((x) << 10)

#ifdef __linux__
	#include "main-linux.c"
#endif

#ifdef __WIN32
	#include "main-win32.c"
#endif

