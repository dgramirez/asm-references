#ifndef COMMON_INCLUDE_DEFINES_H
#define COMMON_INCLUDE_DEFINES_H

#define minmax_bits(x, y) (((x) ^ (y)) & -((x) < (y)))
#define imin(x, y) ((y) ^ minmax_bits((x), (y)))
#define imax(x, y) ((x) ^ minmax_bits((x), (y)))

#define minmaxq_bits(x, y, t) (((x) - (y)) & (((x) - (y)) >> \
	                          ((sizeof(t) << 3) - 1)))
#define iminq(x, y, t) ((y) + minmaxq_bits((x), (y), t))
#define imaxq(x, y, t) ((x) - minmaxq_bits((x), (y), t))

#define isinf(x) (((x) & 0x7FF0000000000000) == 0x7FF0000000000000)
#define isnan(x) (((x) & 0x7FFFFFFFFFFFFFFF)  > 0x7FF0000000000000)


#endif // COMMON_INCLUDE_DEFINES_H

