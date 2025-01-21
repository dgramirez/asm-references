/* Major Note:
 *       PLEASE REFER TO ONE OF THESE TWO MANUALS:
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
 */

#ifndef CPUID_PRJ_INCLUDE_CPUID_ID_H
#define CPUID_PRJ_INCLUDE_CPUID_ID_H

#include "cpuid-asm.h"

typedef enum {
	// These Magic Numbers is obtained when xoring ebx, edx and ecx (in
	// that order) upon receiving the vendor string.
	CPUID_XORID_GenuineIntel = 0x506e7f40,
	CPUID_XORID_AuthenticAMD = 0x454d5a47,
} CpuidXorId;

typedef union {
	c8     data[16];
	u32 data_u32[4];
	struct { u32 ebx, edx, ecx, xor_id; };
} CpuidVendor;

typedef struct {
	union {
		u32 eax;
		struct {
			u32 stepping_id : 4;
			u32 model       : 4;
			u32 family      : 4;
			u32 cpu_type    : 2;
			u32 reserved_1  : 2;
			u32 model_ext   : 4;
			u32 family_ext  : 8;
			u32 reserved_2  : 4;
		};
	};

	union {
		u32 ebx;
		struct {
			u8 brand_id;
			u8 clflush_size;
		    u8 logical_cpu_count;
			u8 local_apic;
		};
	};
} CpuidBasicCpuInfo;

typedef struct {
	union {
		u32 ecx;
		struct {
			u32 sse3                : 1; u32 pclmulqdq           : 1;
			u32 dtes64              : 1; u32 monitor             : 1;
			u32 ds_cpl              : 1; u32 vmx                 : 1;
			u32 smx                 : 1; u32 eist                : 1;
			u32 tm2                 : 1; u32 ssse3               : 1;
			u32 cnxtid              : 1; u32 sdbg                : 1;
			u32 fma                 : 1; u32 cmpxchg16b          : 1;
			u32 xtpr_update_control : 1; u32 pdcm                : 1;
			u32 reserved            : 1; u32 pcid                : 1;
			u32 dca                 : 1; u32 sse4_1              : 1;
			u32 sse4_2              : 1; u32 x2apic              : 1;
			u32 movbe               : 1; u32 popcnt              : 1;
			u32 tsc_deadline        : 1; u32 aes                 : 1;
			u32 xsave               : 1; u32 osxsave             : 1;
			u32 avx                 : 1; u32 f16c                : 1;
			u32 rdrand              : 1; u32 raz                 : 1;
		};
	};

	union {
		u32 edx;
		struct {
			u32 fpu              : 1; u32 vme              : 1;
			u32 de               : 1; u32 pse              : 1;
			u32 tsc              : 1; u32 msr              : 1;
			u32 pae              : 1; u32 mce              : 1;
			u32 cmpxchg8b        : 1; u32 apic             : 1;
			u32 reserved_3       : 1; u32 sysenter_sysexit : 1;
			u32 mtrr             : 1; u32 pge              : 1;
			u32 mca              : 1; u32 cmov             : 1;
			u32 pat              : 1; u32 pse36            : 1;
			u32 psn              : 1; u32 clfsh            : 1;
			u32 reserved_4       : 1; u32 ds               : 1;
			u32 acpi             : 1; u32 mmx              : 1;
			u32 fxsr             : 1; u32 sse              : 1;
			u32 sse2             : 1; u32 ss               : 1;
			u32 htt              : 1; u32 tm               : 1;
			u32 reserved_5       : 1; u32 pbe              : 1;
		};
	};
} CpuidBasicFeatures;

typedef union {
	c8  data[48];
	u32 data_u32[12];
} CpuidCpuName;

static u32
cpuid_get_max_fn_std()
{
	u32 eax;
	u32 reserved;

	eax = CPUID_FN_STD_VENDOR_AND_MAX_FN;
	cpuid_native(&eax, &reserved, &reserved, &reserved);

	return eax;
}

static u32
cpuid_get_vendor_and_max_fn_std(CpuidVendor *v) {
	u32 eax;

	if (!v)
		return CPUID_RETURN_PARAMETER_IS_NULL;

	eax = CPUID_FN_STD_VENDOR_AND_MAX_FN;
	cpuid_native(&eax, &v->ebx, &v->ecx, &v->edx);

	v->xor_id = v->ebx ^ v->edx ^ v->ecx;
	return eax;
}

static CPUID_ERROR
cpuid_get_cpu_info_and_basic_features(CpuidBasicCpuInfo  *cpu_info,
                                      CpuidBasicFeatures *features)
{
	u32 max_fn;
	u32 reserved;
	u32 *eax, *ebx, *ecx, *edx;

	max_fn = cpuid_get_max_fn_std();
	if (max_fn < CPUID_FN_STD_BASIC_CPU_INFO_AND_FEATURES)
		return CPUID_FUNCTION_NOT_AVAILABLE;

	eax = &reserved;
	ebx = &reserved;
	ecx = &reserved;
	edx = &reserved;

	if (cpu_info) {
		eax = &cpu_info->eax;
		ebx = &cpu_info->ebx;
	}

	if (features) {
		ecx = &features->ecx;
		edx = &features->edx;
	}

	*eax = CPUID_FN_STD_BASIC_CPU_INFO_AND_FEATURES;
	cpuid_native(eax, ebx, ecx, edx);

	return CPUID_SUCCESS;
}

static u32
cpuid_get_max_fn_ext()
{
	u32 eax;
	u32 reserved;

	eax = CPUID_FN_EXT_MAX_FN;
	cpuid_native(&eax, &reserved, &reserved, &reserved);

	return eax;
}

static CPUID_ERROR
cpuid_get_cpu_name(CpuidCpuName *cpu_name)
{
	u32 max_fn;

	if (!cpu_name)
		return CPUID_RETURN_PARAMETER_IS_NULL;

	max_fn = cpuid_get_max_fn_ext();
	if (max_fn < CPUID_FN_EXT_CPU_NAME_00_15)
		return CPUID_FUNCTION_NOT_AVAILABLE;

	cpu_name->data_u32[0] = CPUID_FN_EXT_CPU_NAME_00_15;
	cpu_name->data_u32[4] = CPUID_FN_EXT_CPU_NAME_16_31;
	cpu_name->data_u32[8] = CPUID_FN_EXT_CPU_NAME_32_47;

	cpuid_native(&cpu_name->data_u32[0], &cpu_name->data_u32[1],
				 &cpu_name->data_u32[2], &cpu_name->data_u32[3]);

	if (max_fn >= CPUID_FN_EXT_CPU_NAME_16_31) {
		cpuid_native(&cpu_name->data_u32[4], &cpu_name->data_u32[5],
					 &cpu_name->data_u32[6], &cpu_name->data_u32[7]);
	}

	if (max_fn >= CPUID_FN_EXT_CPU_NAME_32_47) {
		cpuid_native(&cpu_name->data_u32[8],  &cpu_name->data_u32[9],
					 &cpu_name->data_u32[10], &cpu_name->data_u32[11]);
	}

	return CPUID_SUCCESS;
}

#endif // CPUID_PRJ_INCLUDE_CPUID_ID_H

