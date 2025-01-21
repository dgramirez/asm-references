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

#ifndef CPUID_PRJ_INCLUDE_CPUID_CACHE_H
#define CPUID_PRJ_INCLUDE_CPUID_CACHE_H

#include "cpuid-asm.h"
#include "cpuid-id.h"

typedef struct {
	union {
		u32 eax;
		struct {
			u32 type                     :  5;
			u32 level                    :  3;
			u32 self_initialized         :  1;
			u32 fully_associative        :  1;
			u32 reserved_1               :  4;
			u32 logical_cpu_share_count  : 12;
			u32 physical_cpu_share_count :  6;
		};
	};

	union {
		u32 ebx;
		struct {
			u32 line_size          : 12;
			u32 physical_partition : 10;
			u32 way_count          : 10;
		};
	};

	union {
		u32 ecx;
		u32 set_count;
	};

	union {
		u32 edx;
		struct {
			u32 write_back_invalidation :  1;
			u32 inclusivity             :  1;
			u32 complex_indexing        :  1;
			u32 reserved_3              : 29;
		};
	};

	usz size;
} CpuidCacheTopologyItem;

typedef struct {
	union {
		u32 eax;
		struct { 
			u32 i_tlb_2mb_4mb_size        : 8;
			u32 i_tlb_2mb_4mb_associative : 8;
			u32 d_tlb_2mb_4mb_size        : 8;
			u32 d_tlb_2mb_4mb_associative : 8;
		};
	};

	union {
		u32 ebx;
		struct { 
			u32 i_tlb_4k_size        : 8;
			u32 i_tlb_4k_associative : 8;
			u32 d_tlb_4k_size        : 8;
			u32 d_tlb_4k_associative : 8;
		};
	};

	union {
		u32 ecx;
		struct {
			u32 d_cache_line_size     : 8;
			u32 d_cache_lines_per_tag : 8;
			u32 d_associative         : 8;
			u32 d_cache_size          : 8;
		};
	};

	union {
		u32 edx;
		struct {
			u32 i_cache_line_size     : 8;
			u32 i_cache_lines_per_tag : 8;
			u32 i_associative         : 8;
			u32 i_cache_size          : 8;
		};
	};
} CpuidL1CacheTlbAmd;

typedef struct {
	union {
		u32 eax;
		struct { 
			u32 i_tlb_2mb_4mb_size        : 12;
			u32 i_tlb_2mb_4mb_associative : 4;
			u32 d_tlb_2mb_4mb_size        : 12;
			u32 d_tlb_2mb_4mb_associative : 4;
		};
	};

	union {
		u32 ebx;
		struct { 
			u32 i_tlb_4k_size        : 12;
			u32 i_tlb_4k_associative : 4;
			u32 d_tlb_4k_size        : 12;
			u32 d_tlb_4k_associative : 4;
		};
	};

	union {
		u32 ecx;
		struct {
			u32 cache_line_size     : 8;
			u32 cache_lines_per_tag : 4;
			u32 associative         : 4;
			u32 cache_size          : 16;
		};
	};
} CpuidL2CacheTlbAmd;

typedef struct {
	union {
		u32 edx;
		struct {
			u32 cache_line_size     : 8;
			u32 cache_lines_per_tag : 4;
			u32 associative         : 4;
			u32 reserved            : 2;
			u32 cache_size          : 14;
		};
	};
} CpuidL3CacheAmd;

static const char* __cpuid_cache_level[8] =
{
	"Reserved",  "Level 1",  "Level 2",  "Level 3",
	"Reserved", "Reserved", "Reserved",	"Reserved"
};

static const char* __cpuid_cache_type[16] =
{
	"Reserved", "Data Cache", "Instruction Cache", "Unified Cache",
	"Reserved",   "Reserved",          "Reserved",      "Reserved",
	"Reserved",	  "Reserved",          "Reserved",      "Reserved",
	"Reserved",	  "Reserved",          "Reserved",      "Reserved"
};

static CPUID_ERROR
cpuid_get_cache_properties(CpuidL1CacheTlbAmd *l1,
                           CpuidL2CacheTlbAmd *l2,
                           CpuidL3CacheAmd    *l3)
{
	CpuidVendor vendor;
	u32 *eax, *ebx, *ecx, *edx;
	u32 max_fn_std;
	u32 max_fn_ext;
	u32 cpuid_eax;
	u32 reserved;
	u32 eax_fn_l1;
	u32 eax_fn_l2l3;

	max_fn_std = cpuid_get_vendor_and_max_fn_std(&vendor);
	switch(vendor.xor_id) {
		case CPUID_XORID_GenuineIntel: {
			if (max_fn_std < CPUID_FN_STD_CACHE_TOPOLOGY_INTEL)
				return CPUID_FUNCTION_NOT_AVAILABLE;

			return CPUID_FN_ERROR_CPU_NOT_IMPLEMENTED;
		} break;

		case CPUID_XORID_AuthenticAMD: {
			max_fn_ext = cpuid_get_max_fn_ext();
			if (max_fn_ext < CPUID_FN_EXT_L1_CACHE_PROPERTIES_AMD)
				return CPUID_FUNCTION_NOT_AVAILABLE;

			eax_fn_l1   = CPUID_FN_EXT_L1_CACHE_PROPERTIES_AMD;
			eax_fn_l2l3 = CPUID_FN_EXT_L2_L3_CACHE_PROPERTIES_AMD;
		} break;

		default: return CPUID_FN_ERROR_CPU_NOT_IMPLEMENTED;
	}

	// Note: This doesn't work for **all** CPUs.
	if (l1) {
		l1->eax = eax_fn_l1;
		cpuid_native(&l1->eax, &l1->ebx, &l1->ecx, &l1->edx);
	}

	if ((max_fn_ext >= eax_fn_l2l3) && (l2 || l3)) {
		if (l2) {
			eax = &l2->eax;
			ebx = &l2->ebx;
			ecx = &l2->ecx;
		}
		else {
			eax = &cpuid_eax;
			ebx = &reserved;
			ecx = &reserved;
		}

		if (l3)
			edx = &l3->edx;
		else
			edx = &reserved;

		*eax = eax_fn_l2l3;
		cpuid_native(eax, ebx, ecx, edx);
	}

	return 0;
}

static CPUID_ERROR
cpuid_enumerate_cache_topology(u32 *count,
                               CpuidCacheTopologyItem **out)
{
	CpuidVendor vendor;
	CpuidCacheTopologyItem *cache;
	u32 eax, ebx, ecx, edx;
	u32 max_fn_std;
	u32 max_fn_ext;
	u32 eax_fn;
	u32 reserved;
	CPUID_ERROR err;

	max_fn_std = cpuid_get_vendor_and_max_fn_std(&vendor);
	switch(vendor.xor_id) {
		case CPUID_XORID_GenuineIntel: {
			if (max_fn_std < CPUID_FN_STD_CACHE_TOPOLOGY_INTEL) {
				*count = 0;
				return CPUID_FUNCTION_NOT_AVAILABLE;
			}

			eax_fn = CPUID_FN_STD_CACHE_TOPOLOGY_INTEL;
		} break;

		case CPUID_XORID_AuthenticAMD: {
			// TODO: Look into 0x80000001 ECX[TopologyExtension].
			//       Fail this function if ECX[TopologyExtension] == 0

			max_fn_ext = cpuid_get_max_fn_ext();
			if (max_fn_ext <= CPUID_FN_EXT_CACHE_TOPOLOGY_AMD) {
				*count = 0;
				return CPUID_FUNCTION_NOT_AVAILABLE;
			}

			eax_fn = CPUID_FN_EXT_CACHE_TOPOLOGY_AMD;
		} break;

		default: return CPUID_FN_ERROR_CPU_NOT_IMPLEMENTED;
	}

	err = CPUID_ENUMERATION_MISSING_COUNT;
	if (count && (*count == 0)) {
		do {
			eax = eax_fn;
			ecx = *count;
			cpuid_native(&eax, &reserved, &ecx, &reserved);

			*count += 1;
		} while (eax & 0xF);

		*count -= 1;
		err = CPUID_SUCCESS;
	}

	if (count && out) {
		*count = 0;
		cache = *out;

		while (1) {
			eax = eax_fn;
			ecx = *count;
			cpuid_native(&eax, &ebx, &ecx, &edx);
			if ((eax & 0xF) == 0)
				break;

			cache[*count].eax = eax;
			cache[*count].ebx = ebx;
			cache[*count].ecx = ecx;
			cache[*count].edx = edx;

			cache[*count].size = (cache[*count].way_count + 1) *
			                     (cache[*count].line_size + 1) *
			                     (cache[*count].physical_partition + 1) *
			                     (cache[*count].set_count + 1);

			*count += 1;
		}

		err = CPUID_SUCCESS;
	}

	return err;
}

#endif // CPUID_PRJ_INCLUDE_CPUID_CACHE_H

