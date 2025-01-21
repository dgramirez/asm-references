#ifndef COMMON_INCLUDE_BUF8_H
#define COMMON_INCLUDE_BUF8_H

#include "defines.h"
#include "types.h"
#include "s8.h"

typedef struct {
	union {
		s8 str;
		struct {
			u8 *data;
			usz len;
		};
	};

	void *fd;
	usz cap;
	i32 err;
} buf8;

typedef void
(*pfn_os_write)(buf8 *b);
static void stub_os_write(buf8 *b) { (void)sizeof(b); }
static pfn_os_write os_write = stub_os_write;


static void
buf8_flush(buf8 *b)
{
	b->err |= ((isz)b->fd < 0);
	if (!b->err && b->len) {
		os_write(b);
		b->len = 0;
	}
}

static void
buf8_append(buf8 *b,
            s8 str)
{
	u8 *end;
	usz count;
	usz remainder;
	usz i;

	end = str.data + str.len;
	while (!b->err && str.data < end) {
		remainder = end - str.data;
		count = iminq(end - str.data, remainder, usz);

		for (i = 0; i < count; ++i)
			b->data[b->len+i] = str.data[i];

		b->len += count;
		str.data += count;

		if (count < remainder) {
			buf8_flush(b);
		}
	}

}

static void
buf8_append_cstr(buf8 *b,
                 const char *cstr)
{
	s8 str;

	str.data = (u8*)cstr;
	str.len = c_strlen(cstr);
	buf8_append(b, str);
}

static void
buf8_append_byte(buf8 *b,
                 u8 c)
{
	if (b->len == b->cap)
		buf8_flush(b);

	b->data[b->len++] = c;
}

static void
buf8_append_lf(buf8 *b)
{
	#if _WIN32_WINNT < _WIN32_WINNT_WIN8
		buf8_append_byte(b, '\r'); // CR
	#endif
	buf8_append_byte(b, '\n'); // LF
}

static void
buf8_append_crlf(buf8 *b)
{
	buf8_append_byte(b, '\r'); // CR
	buf8_append_byte(b, '\n'); // LF
}

static void
buf8_append_isz(buf8 *b,
                isz x)
{
	u8 tmp[32] = {0};
	s8 input;
	u8 *end = tmp + sizeof(tmp);
	u8 *beg = end;

	if (x < 0) {
		tmp[0] = x < 0;
		x = -x;
	}

	do {
		*--beg = '0' + (x % 10);
	} while (x /= 10);

	if (tmp[0]) {
		*--beg = '-';
	}

	input.data = beg;
	input.len = end - beg;
	buf8_append(b, input);
}

static void
buf8_append_usz(buf8 *b,
                usz x)
{
	u8 tmp[32] = {0};
	s8 input;
	u8 *end;
	u8 *beg;

	end = tmp + sizeof(tmp);
	beg = end;
	do {
		*--beg = '0' + (x % 10);
	} while (x /= 10);

	input.data = beg;
	input.len = end - beg;
	buf8_append(b, input);
}

static void
buf8_append_hex(buf8 *b,
                usz x)
{
	static const u8 hex_table[] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};

	u8 tmp[32] = {0};
	s8 input;
	u8 *end;
	u8 *beg;

	end = tmp + sizeof(tmp);
	beg = end;
	do {
		*--beg = hex_table[x & 0xF];
	} while(x >>= 4);

	*--beg = 'x';
	*--beg = '0';

	input.data = beg;
	input.len = end - beg;
	buf8_append(b, input);
}

static void
buf8_append_f64(buf8 *b,
                double d) // [Insert Joke Here.]
{
	usz integral;
	usz fractional; 
	usz dbits;
	i32 precision;
	usz i;

	if (d < 0) {
		buf8_append_byte(b, '-');
		d = -d;
	}

	d += 0.0000005;
	dbits = *(usz*)(&d);

	if (isinf(dbits)) {
		buf8_append(b, s8("inf"));
		return;
	}

	if (isnan(dbits)) {
		buf8_append(b, s8("nan"));
		return;
	}

	precision = 1000000;
	integral = (usz)d;
	fractional = (usz)((d - integral) * precision);

	buf8_append_usz(b, integral);
	buf8_append_byte(b, '.');
	for (i = precision/10; i > 1; i /= 10) {
		if (i > fractional)
			buf8_append_byte(b, '0');
	}

	buf8_append_usz(b, fractional);
}

static void
buf8_append_mem(buf8 *b,
                void *mem,
                usz   len)
{
	s8 str;
	str.data = mem;
	str.len  = len;

	buf8_append(b, str);
}

#endif // COMMON_INCLUDE_BUF8_H

