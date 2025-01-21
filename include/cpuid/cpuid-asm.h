/* Major Note:
 *       FOR ALL MAGIC NUMBERS, PLEASE REFER TO ONE OF THESE TWO MANUALS:
 *          - Intel 64 and IA-32 Architectures
 *            Software Development Manual Vol. 2: CPUID - CPU Identification
 *            (Pg. 3-222 - Pg. 3-270)
 *
 *          - AMD64 Architecture Programmer's Manual
 *            Vol. 3: General-Purpose and System Instructions
 *            Appendix E: Obtaining Processor Information Via the CPUID
 *                        Instruction (Pg. 599 - Pg. 648)
 *
 *        This little project is not meant to be a library, but a reference
 *        on obtaining information on a cpu, like what CPU vendor a user
 *        is using, their cache / tlb values, supported feature sets, etc.
 *
 *        AGAIN, THIS IS NOT A LIBRARY!
 *
 *        All code presented here is within the public domain, under
 *        The Unlicense. For more information, please refer to
 *        <https://unlicense.org/>
 */

#ifndef CPUID_PRJ_INCLUDE_CPUID_ASM_H
#define CPUID_PRJ_INCLUDE_CPUID_ASM_H

i32
has_cpuid();

void
cpuid_native(u32 *eax, u32 *ebx,
             u32 *ecx, u32 *edx);

typedef enum {
	CPUID_FN_ERROR_CPU_NOT_IMPLEMENTED = -1,

	CPUID_SUCCESS = 0,
	CPUID_FUNCTION_NOT_AVAILABLE,
	CPUID_RETURN_PARAMETER_IS_NULL,
	CPUID_NOT_IMPLEMENTED_OR_NOT_EXIST,
	CPUID_ENUMERATION_MISSING_COUNT,
} CPUID_ERROR;

typedef enum {
	CPUID_FN_STD_VENDOR_AND_MAX_FN           = 0x00000000,
	CPUID_FN_STD_BASIC_CPU_INFO_AND_FEATURES = 0x00000001,
	CPUID_FN_STD_CACHE_TOPOLOGY_INTEL        = 0x00000004,

	CPUID_FN_EXT_MAX_FN                      = 0x80000000,
	CPUID_FN_EXT_CPU_INFO_AND_FEATURES       = 0x80000001,
	CPUID_FN_EXT_CPU_NAME_00_15              = 0x80000002,
	CPUID_FN_EXT_CPU_NAME_16_31              = 0x80000003,
	CPUID_FN_EXT_CPU_NAME_32_47              = 0x80000004,
	CPUID_FN_EXT_L1_CACHE_PROPERTIES_AMD     = 0x80000005,
	CPUID_FN_EXT_L2_L3_CACHE_PROPERTIES_AMD  = 0x80000006,
	CPUID_FN_EXT_CACHE_TOPOLOGY_AMD          = 0x8000001D,
} CPUID_FUNCTIONS;

#endif // CPUID_PRJ_INCLUDE_CPUID_H

