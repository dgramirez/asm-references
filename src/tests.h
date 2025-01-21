#ifndef SRC_TESTS_H
#define SRC_TESTS_H

#include "types.h"
#include "s8.h"
#include "buf8.h"
#include "cpuid/cpuid.h"

static void
print_argument_count(buf8 *b,
                     i32 argc);
static void
print_argument_vector(buf8 *b,
                      i32 argc,
                      c7 **argv);

static void
print_environment_variables(buf8 *b,
                            c7 **envp);

static u32
print_cpuid_vendor(buf8 *b);

static void
print_cpuid_processor_name(buf8 *b);

static void
print_basic_cpu_info(buf8 *b,
                     u32 xor_id);

static void
print_features(buf8 *b);

static void
print_basic_cache_properties(buf8 *b);

static void
print_cache_topology(buf8 *b,
                     void *buf_top,
                     usz buf_size);

#endif // SRC_TESTS_H

