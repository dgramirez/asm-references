#ifndef COMMON_INCLUDE_S8_H
#define COMMON_INCLUDE_S8_H

#include "types.h"

#define countof(str)  (usz)(sizeof(str) / sizeof(*(str)))
#define lengthof(str) (countof(str) - 1)
#define s8(str) (s8){(c8 *)str, lengthof(str)}
typedef struct {
	u8 *data;
	usz len;
} s8;

static usz
c_strlen(const char *cstr)
{
	const char *end;

	end = cstr;
	while (*end) end++;

	return end - cstr;
}

#endif // COMMON_INCLUDE_S8_H

