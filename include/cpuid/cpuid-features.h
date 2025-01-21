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

#ifndef CPUID_PRJ_INCLUDE_CPUID_FEATURES_H
#define CPUID_PRJ_INCLUDE_CPUID_FEATURES_H

typedef struct {
	union {
		u32 ecx;
		struct {
			u32 lahf_sahf           : 1; u32 cmp_legacy          : 1;
			u32 svm                 : 1; u32 ext_apic_space      : 1;
			u32 alt_mov_cr8         : 1; u32 lzcnt               : 1;
			u32 sse4a               : 1; u32 misaligned_sse      : 1;
			u32 prefetchw           : 1; u32 osvw                : 1;
			u32 ibs                 : 1; u32 xop                 : 1;
			u32 skinit              : 1; u32 wdt                 : 1;
			u32 reserved_0          : 1; u32 lwp                 : 1;
			u32 fma4                : 1; u32 tce                 : 1;
			u32 reserved_1          : 1; u32 reserved_2          : 1;
			u32 reserved_3          : 1; u32 tbm                 : 1;
			u32 topology_extensions : 1; u32 perf_ctr_ext_core   : 1;
			u32 perf_ctr_ext_nb     : 1; u32 reserved_4          : 1;
			u32 data_bkpt_ext       : 1; u32 perf_tsc            : 1;
			u32 perf_ctr_ext_llc    : 1; u32 monitor_x           : 1;
			u32 addr_mask_ext       : 1; u32 reserved_5          : 1;
		};
	};

	union {
		u32 edx;
		struct {
			u32 fpu            : 1; u32 vme            : 1;
			u32 de             : 1; u32 pse            : 1;
			u32 tsc            : 1; u32 msr            : 1;
			u32 pae            : 1; u32 mce            : 1;
			u32 cmpxchg8b      : 1; u32 apic           : 1;
			u32 reserved_6     : 1; u32 syscall_sysret : 1;
			u32 mtrr           : 1; u32 pge            : 1;
			u32 mca            : 1; u32 cmov           : 1;
			u32 pat            : 1; u32 pse36          : 1;
			u32 reserved_7     : 1; u32 reserved_8     : 1;
			u32 nx             : 1; u32 reserved_9     : 1;
			u32 mmx_ext        : 1; u32 mmx            : 1;
			u32 fxsr           : 1; u32 ffxsr          : 1;
			u32 page_1gb       : 1; u32 rdtscp         : 1;
			u32 reserved_10    : 1; u32 long_mode      : 1;
			u32 now_3d_ext     : 1; u32 now_3d         : 1;
		};
	};
} CpuidExtendedFeatures;

static CPUID_ERROR
cpuid_get_cpu_extended_features(CpuidExtendedFeatures *features)
{
	u32 max_fn;
	u32 reserved;
	u32 eax;

	if (!features)
		return CPUID_RETURN_PARAMETER_IS_NULL;

	max_fn = cpuid_get_max_fn_ext();
	if (max_fn < CPUID_FN_EXT_CPU_INFO_AND_FEATURES)
		return CPUID_FUNCTION_NOT_AVAILABLE;

	eax = CPUID_FN_EXT_CPU_INFO_AND_FEATURES;
	cpuid_native(&eax, &reserved,
	             &features->ecx, &features->edx);

	return CPUID_SUCCESS;
}

#endif // CPUID_PRJ_INCLUDE_CPUID_FEATURES_H

