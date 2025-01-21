#ifndef SRC_TETS_C
#define SRC_TETS_C

#include "tests.h"

static void
test1_print_main_arguments(buf8 *b,
                           i32 argc,
                           c7 **argv,
                           c7 **envp)
{
	if (argc) {
		print_argument_count(b, argc);
		buf8_append_lf(b);
	}

	if (argv) {
		print_argument_vector(b, argc, argv);
		buf8_append_lf(b);
	}

	if (envp) {
		print_environment_variables(b, envp);
		buf8_append_lf(b);
	}

	buf8_flush(b);
}

static void
test2_print_cpuid_information(buf8 *b,
                              void *buf_top,
                              usz   buf_size)
{
	i32 err;
	u32 xor_id;

	buf8_append(b, s8("======================\n"));
	buf8_append(b, s8("= CPUID Availability =\n"));
	buf8_append(b, s8("======================\n"));
	err = has_cpuid();
	if (err) {
		buf8_append(b, s8("CPUID availability: No\nExiting . . .\n\n"));
		buf8_flush(b);
		return;
	}
	buf8_append(b, s8("CPUID availability: Yes\n\n"));

	xor_id = print_cpuid_vendor(b);
	buf8_append_lf(b);

	print_cpuid_processor_name(b);
	buf8_append_lf(b);

	print_basic_cpu_info(b, xor_id);
	buf8_append_lf(b);

	print_features(b);
	buf8_append_lf(b);

	if (xor_id == CPUID_XORID_AuthenticAMD)
		print_basic_cache_properties(b);

	print_cache_topology(b, buf_top, buf_size);

	buf8_append_lf(b);
	buf8_flush(b);
}

static void
print_argument_count(buf8 *b,
                     i32 argc)
{
	buf8_append_cstr(b, "Argument Count: ");
	buf8_append_isz(b, argc);
	buf8_append_lf(b);
}

static void
print_argument_vector(buf8 *b,
                      i32 argc,
                      c7 **argv)
{
	for (i32 i = 0; i < argc; ++i) {
		buf8_append(b, s8("\tIndex "));
		buf8_append_isz(b, i);
		buf8_append_byte(b, ':');
		buf8_append_byte(b, ' ');
		buf8_append_cstr(b, argv[i]);
		buf8_append_lf(b);
	}
}

static void
print_environment_variables(buf8 *b,
                            c7 **envp)
{
	while (*envp) {
		buf8_append_cstr(b, *envp);
		buf8_append_lf(b);
		envp++;
	}
}

static u32
print_cpuid_vendor(buf8 *b)
{
	u32 max_fn;
	CpuidVendor vendor_name = {0};

	max_fn = cpuid_get_vendor_and_max_fn_std(&vendor_name);
	buf8_append(b, s8("==================================\n"));
	buf8_append(b, s8("= Vendor Name and Max Extened Fn =\n"));
	buf8_append(b, s8("==================================\n"));

	buf8_append(b, s8("Vendor Name: "));
	buf8_append_mem(b, vendor_name.data, 12);

	buf8_append(b, s8(" (xor_id: "));
	buf8_append_hex(b, vendor_name.xor_id);
	buf8_append(b, s8(")\n"));

	buf8_append(b, s8("CPUID Max Standard Function Value: "));
	buf8_append_hex(b, max_fn);
	buf8_append_lf(b);

	return vendor_name.xor_id;
}

static void
print_cpuid_processor_name(buf8 *b)
{
	u32 max_fn;
	CpuidCpuName cpu_name = {0};

	max_fn = cpuid_get_max_fn_ext();
	cpuid_get_cpu_name(&cpu_name);
	buf8_append(b, s8("=====================================\n"));
	buf8_append(b, s8("= Processor Name and Max Extened Fn =\n"));
	buf8_append(b, s8("=====================================\n"));

	buf8_append(b, s8("Processor Name: "));
	buf8_append_mem(b, cpu_name.data, 48);
	buf8_append_lf(b);

	buf8_append(b, s8("CPUID Max Extended Function Value: "));
	buf8_append_hex(b, max_fn);
	buf8_append_lf(b);
}

static void
print_basic_cpu_info(buf8 *b,
                     u32 xor_id)
{
	CpuidVendor        vendor       = {0};
	CpuidBasicCpuInfo  cpu_info     = {0};

	cpuid_get_cpu_info_and_basic_features(&cpu_info, 0);
	buf8_append(b, s8("=========================\n"));
	buf8_append(b, s8("= Basic CPU Information =\n"));
	buf8_append(b, s8("=========================\n"));
	buf8_append(b, s8("Basic CPU Info: \n"));

	buf8_append(b, s8("\tModel ID (Hex):    "));
	buf8_append_hex(b, cpu_info.model);
	buf8_append(b, s8("  |   Extended Model ID (Hex):  "));
	buf8_append_hex(b, cpu_info.model_ext);
	buf8_append_lf(b);

	buf8_append(b, s8("\tFamily ID (Hex):   "));
	buf8_append_hex(b, cpu_info.family);
	buf8_append(b, s8("  |   Extended Family ID (Hex): "));
	buf8_append_hex(b, cpu_info.family_ext);
	buf8_append_lf(b);


	buf8_append(b, s8("\tStepping ID (Hex): "));
	buf8_append_hex(b, cpu_info.stepping_id);

	cpuid_get_vendor_and_max_fn_std(&vendor);
	if (vendor.xor_id == CPUID_XORID_GenuineIntel) {
		buf8_append(b, s8("  |   Processor Type (Hex): "));
		buf8_append_hex(b, cpu_info.cpu_type);
	}

	buf8_append_lf(b);
}

static void
print_features(buf8 *b)
{
	CpuidVendor           vendor           = {0};
	CpuidBasicFeatures    cpu_features_std = {0};
	CpuidExtendedFeatures cpu_features_ext = {0};

	cpuid_get_vendor_and_max_fn_std(&vendor);
	cpuid_get_cpu_info_and_basic_features(0, &cpu_features_std);
	cpuid_get_cpu_extended_features(&cpu_features_ext);
	
	buf8_append(b, s8("======================\n"));
	buf8_append(b, s8("= Basic CPU Features =\n"));
	buf8_append(b, s8("======================\n"));

	buf8_append(b, s8("\tfpu:    "));
	buf8_append_usz(b, cpu_features_std.fpu);
	buf8_append(b, s8(" | f16c:    "));
	buf8_append_usz(b, cpu_features_std.f16c);
	buf8_append_lf(b);

	buf8_append(b, s8("\tmmx:    "));
	buf8_append_usz(b, cpu_features_std.mmx);
	buf8_append(b, s8(" | aes:     "));
	buf8_append_usz(b, cpu_features_std.aes);
	buf8_append_lf(b);

	buf8_append(b, s8("\tsse:    "));
	buf8_append_usz(b, cpu_features_std.sse);
	buf8_append(b, s8(" | rdrand:  "));
	buf8_append_usz(b, cpu_features_std.rdrand);
	buf8_append_lf(b);

	buf8_append(b, s8("\tsse2:   "));
	buf8_append_usz(b, cpu_features_std.sse2);
	buf8_append(b, s8(" | fxsr:    "));
	buf8_append_usz(b, cpu_features_std.fxsr);
	buf8_append_lf(b);

	buf8_append(b, s8("\tsse3:   "));
	buf8_append_usz(b, cpu_features_std.sse3);
	buf8_append(b, s8(" | monitor: "));
	buf8_append_usz(b, cpu_features_std.monitor);
	buf8_append_lf(b);

	buf8_append(b, s8("\tssse3:  "));
	buf8_append_usz(b, cpu_features_std.ssse3);
	buf8_append(b, s8(" | tsc:     "));
	buf8_append_usz(b, cpu_features_std.tsc);
	buf8_append_lf(b);

	buf8_append(b, s8("\tsse4_1: "));
	buf8_append_usz(b, cpu_features_std.sse4_1);
	buf8_append(b, s8(" | msr:     "));
	buf8_append_usz(b, cpu_features_std.msr);
	buf8_append_lf(b);

	buf8_append(b, s8("\tsse4_2: "));
	buf8_append_usz(b, cpu_features_std.sse4_2);
	buf8_append(b, s8(" | mtrr:    "));
	buf8_append_usz(b, cpu_features_std.mtrr);
	buf8_append_lf(b);

	buf8_append(b, s8("\tfma:    "));
	buf8_append_usz(b, cpu_features_std.fma);
	buf8_append(b, s8(" | mca:     "));
	buf8_append_usz(b, cpu_features_std.mca);
	buf8_append_lf(b);

	buf8_append(b, s8("\tavx:    "));
	buf8_append_usz(b, cpu_features_std.avx);
	buf8_append(b, s8(" | apic:    "));
	buf8_append_usz(b, cpu_features_std.apic);
	buf8_append_lf(b);

	buf8_append_lf(b);
	buf8_append(b, s8("=========================\n"));
	buf8_append(b, s8("= Extended CPU Features =\n"));
	buf8_append(b, s8("=========================\n"));

	buf8_append(b, s8("\tlahf & sahf: "));
	buf8_append_usz(b, cpu_features_ext.lahf_sahf);
	buf8_append(b, s8("   | lzcnt:            "));
	buf8_append_usz(b, cpu_features_ext.lzcnt);
	buf8_append_lf(b);

	buf8_append(b, s8("\tprefetchw:   "));
	buf8_append_usz(b, cpu_features_ext.prefetchw);
	buf8_append(b, s8("   | syscall / sysret: "));
	buf8_append_usz(b, cpu_features_ext.syscall_sysret);
	buf8_append_lf(b);

	buf8_append(b, s8("\tnx:          "));
	buf8_append_usz(b, cpu_features_ext.nx);
	buf8_append(b, s8("   | 1GB Pages:        "));
	buf8_append_usz(b, cpu_features_ext.page_1gb);
	buf8_append_lf(b);

	buf8_append(b, s8("\tRDTSCP:      "));
	buf8_append_usz(b, cpu_features_ext.rdtscp);
	buf8_append(b, s8("   | Long mode:        "));
	buf8_append_usz(b, cpu_features_ext.long_mode);
	buf8_append_lf(b);

	if (vendor.xor_id == CPUID_XORID_AuthenticAMD) {
		buf8_append_lf(b);
		buf8_append(b, s8("\t[AMD]\n"));

		buf8_append(b, s8("\t3D Now:      "));
		buf8_append_usz(b, cpu_features_ext.now_3d);
		buf8_append(b, s8("   | 3D Now Extended:   "));
		buf8_append_usz(b, cpu_features_ext.now_3d_ext);
		buf8_append_lf(b);

		buf8_append(b, s8("\tMMX (basic): "));
		buf8_append_usz(b, cpu_features_ext.mmx);
		buf8_append(b, s8("   | MMX Extended:      "));
		buf8_append_usz(b, cpu_features_ext.mmx_ext);
		buf8_append_lf(b);

		buf8_append(b, s8("\tSSE4a:       "));
		buf8_append_usz(b, cpu_features_ext.sse4a);
		buf8_append(b, s8("   | Misaligned SSE:    "));
		buf8_append_usz(b, cpu_features_ext.misaligned_sse);
		buf8_append_lf(b);

		buf8_append(b, s8("\tMonitor Ext: "));
		buf8_append_usz(b, cpu_features_ext.monitor_x);
		buf8_append(b, s8("   | Topology Extended: "));
		buf8_append_usz(b, cpu_features_ext.topology_extensions);
		buf8_append_lf(b);
	}
}

static void
print_basic_cache_properties(buf8 *b)
{

	CpuidL1CacheTlbAmd l1 = {0};
	CpuidL2CacheTlbAmd l2 = {0};
	CpuidL3CacheAmd    l3 = {0};

	cpuid_get_cache_properties(&l1, &l2, &l3);
	buf8_append(b, s8("==========================\n"));
	buf8_append(b, s8("= [AMD] Cache Properties =\n"));
	buf8_append(b, s8("==========================\n"));

	buf8_append(b, s8("Cache Size:\n"));
	buf8_append(b, s8("\tL1 Inst:    ["));
	buf8_append_usz(b, l1.i_cache_size);
	buf8_append(b, s8("KB, "));
	buf8_append_usz(b, l1.i_associative);
	buf8_append(b, s8("-way associative]\n"));

	buf8_append(b, s8("\tL1 Data:    ["));
	buf8_append_usz(b, l1.d_cache_size);
	buf8_append(b, s8("KB, "));
	buf8_append_usz(b, l1.d_associative);
	buf8_append(b, s8("-way associative]\n"));

	buf8_append(b, s8("\tL2 Unified: ["));
	buf8_append_usz(b, l2.cache_size);
	buf8_append(b, s8("KB, "));
	buf8_append_usz(b, l2.associative);
	buf8_append(b, s8("-way associative]\n"));

	buf8_append(b, s8("\tL3 Unified: ["));
	buf8_append_usz(b, (l3.cache_size << 19) >> 10);
	buf8_append(b, s8("KB, "));
	buf8_append_usz(b, l3.associative);
	buf8_append(b, s8("-way associative]\n\n"));

	buf8_append(b, s8("TLB (4KB):\n"));
	buf8_append(b, s8("\tL1 Inst: ["));
	buf8_append_usz(b, l1.i_tlb_4k_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l1.i_tlb_4k_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));
	buf8_append(b, s8("\tL1 Data: ["));
	buf8_append_usz(b, l1.d_tlb_4k_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l1.d_tlb_4k_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));

	buf8_append(b, s8("\tL2 Inst: ["));
	buf8_append_usz(b, l2.i_tlb_4k_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l2.i_tlb_4k_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));
	buf8_append(b, s8("\tL2 Data: ["));
	buf8_append_usz(b, l2.d_tlb_4k_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l2.d_tlb_4k_associative);
	buf8_append(b, s8("-way Encoded Associative]\n\n"));

	buf8_append(b, s8("TLB (2MB & 4MB):\n"));
	buf8_append(b, s8("\tL1 Inst: ["));
	buf8_append_usz(b, l1.i_tlb_2mb_4mb_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l1.i_tlb_2mb_4mb_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));
	buf8_append(b, s8("\tL1 Data: ["));
	buf8_append_usz(b, l1.d_tlb_2mb_4mb_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l1.d_tlb_2mb_4mb_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));

	buf8_append(b, s8("\tL2 Inst: ["));
	buf8_append_usz(b, l2.i_tlb_2mb_4mb_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l2.i_tlb_2mb_4mb_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));
	buf8_append(b, s8("\tL2 Data: ["));
	buf8_append_usz(b, l2.d_tlb_2mb_4mb_size);
	buf8_append(b, s8(" Entries, "));
	buf8_append_usz(b, l2.d_tlb_2mb_4mb_associative);
	buf8_append(b, s8("-way Encoded Associative]\n"));

	buf8_append_lf(b);
}

static void
print_cache_topology(buf8 *b,
                     void *buf_top,
                     usz buf_size)
{
	CpuidVendor vendor;
	CpuidCacheTopologyItem *cache;
	u32 top_count;
	u32 i;

	buf8_append(b, s8("==================\n"));
	buf8_append(b, s8("= Cache Topology =\n"));
	buf8_append(b, s8("==================\n"));

	top_count = 0;
	cache = buf_top;
	cpuid_enumerate_cache_topology(&top_count, 0);
	if ((top_count * sizeof(CpuidCacheTopologyItem)) > buf_size) {
		buf8_append(b, s8("Buffer Size too small!\n Please increase "));
		buf8_append(b, s8("buf_size inside the main function!\n\n"));

		buf8_append(b, s8("Required Buffer Size: "));
		buf8_append_usz(b, top_count * sizeof(CpuidCacheTopologyItem));
		buf8_append_lf(b);

		buf8_append(b, s8("Current Buffer Size: "));
		buf8_append_usz(b, buf_size);
		buf8_append_lf(b);
		buf8_append_lf(b);

		buf8_append(b, s8("PS: Dynamic Memory allocation will be "));
		buf8_append(b, s8("available in the future of this program.\n\n"));

		return;
	}

	cpuid_enumerate_cache_topology(&top_count, &cache);
	cpuid_get_vendor_and_max_fn_std(&vendor);

	for (i = 0; i < top_count; ++i) {
		buf8_append(b, s8("Cache #"));
		buf8_append_usz(b, i);
		buf8_append(b, s8(":\n"));

		buf8_append(b, s8("\tCache Level:              "));
		buf8_append_usz(b, cache[i].level);
		buf8_append(b, s8("   |   Cache Type: "));
		buf8_append_usz(b, cache[i].type);
		buf8_append(b, s8(" [ "));
		buf8_append_cstr(b, __cpuid_cache_type[cache[i].type]);
		buf8_append(b, s8(" ]"));
		buf8_append_lf(b);

		buf8_append(b, s8("\tSelf Initialization:      "));
		buf8_append_usz(b, cache[i].self_initialized);
		buf8_append(b, s8("   |   Physical Partition:  "));
		buf8_append_usz(b, cache[i].physical_partition + 1);
		buf8_append_lf(b);

		buf8_append(b, s8("\tFully Associative:        "));
		buf8_append_usz(b, cache[i].fully_associative);
		buf8_append(b, s8("   |   Associative Count: "));
		buf8_append_usz(b, cache[i].way_count + 1);
		buf8_append(b, s8("-way associative"));
		buf8_append_lf(b);

		buf8_append(b, s8("\tWrite-Back Invalidation:  "));
		buf8_append_usz(b, cache[i].write_back_invalidation);
		buf8_append(b, s8("   |   Cache Line Size: "));
		buf8_append_usz(b, cache[i].line_size + 1);
		buf8_append_lf(b);

		buf8_append(b, s8("\tInclusivity:              "));
		buf8_append_usz(b, cache[i].inclusivity);
		if (!cache[i].fully_associative) {
			buf8_append(b, s8("   |   Cache Set Count: "));
			buf8_append_usz(b, cache[i].set_count + 1);
		}
		buf8_append_lf(b);

		if (vendor.xor_id == CPUID_XORID_GenuineIntel) {
			buf8_append(b, s8("\tComplex Indexing          "));
			buf8_append_usz(b, cache[i].complex_indexing);
			buf8_append(b, s8("   |   Total Cache Size: "));
			buf8_append_usz(b, cache[i].size);
		}
		else {
			buf8_append(b, s8("\tTotal Cache Size (bytes): "));
			buf8_append_usz(b, cache[i].size);
		}

		buf8_append_lf(b);
		buf8_append_lf(b);
	}
}

#endif // SRC_TETS_C

