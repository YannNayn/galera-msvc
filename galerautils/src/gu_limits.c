// Copyright (C) 2013 Codership Oy <info@codership.com>

/**
 * @file system limit macros
 *
 * $Id:$
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>

#include "gu_limits.h"
#include "gu_log.h"
#include "gu_fnv.h"

#if defined(__APPLE__)

#include <mach/mach.h>

long gu_darwin_phys_pages (void)
{
    /* Note: singleton pattern would be useful here */
    vm_statistics64_data_t vm_stat;
    unsigned int count = HOST_VM_INFO64_COUNT;
    kern_return_t ret = host_statistics64 (mach_host_self (), HOST_VM_INFO64, (host_info64_t) &vm_stat, &count);
    if (ret != KERN_SUCCESS)
    {
        gu_error ("host_statistics64 failed with code %d", ret);
        return 0;
    }
    /* This gives a value a little less than physical memory of computer */
    return vm_stat.free_count + vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count;
    /* Exact value may be obtain via sysctl ({CTL_HW, HW_MEMSIZE}) */
    /* Note: sysctl is 60% slower compared to host_statistics64 */
}

long gu_darwin_avphys_pages (void)
{
    vm_statistics64_data_t vm_stat;
    unsigned int count = HOST_VM_INFO64_COUNT;
    kern_return_t ret = host_statistics64 (mach_host_self (), HOST_VM_INFO64, (host_info64_t) &vm_stat, &count);
    if (ret != KERN_SUCCESS)
    {
        gu_error ("host_statistics64 failed with code %d", ret);
        return 0;
    }
    /* Note: vm_stat.free_count == vm_page_free_count + vm_page_speculative_count */
    return vm_stat.free_count - vm_stat.speculative_count;
}

#elif defined(__FreeBSD__)

#include <vm/vm_param.h> // VM_TOTAL
#include <sys/vmmeter.h> // struct vmtotal

long gu_freebsd_avphys_pages (void)
{
    /* TODO: 1) sysctlnametomib may be called once */
    /*       2) vm.stats.vm.v_cache_count is potentially free memory too */
    int mib_vm_stats_vm_v_free_count[4];
    size_t mib_sz = 4;
    int rc = sysctlnametomib ("vm.stats.vm.v_free_count", mib_vm_stats_vm_v_free_count, &mib_sz);
    if (rc != 0)
    {
        gu_error ("sysctlnametomib(vm.stats.vm.v_free_count) failed with code %d", rc);
        return 0;
    }

    unsigned int vm_stats_vm_v_free_count;
    size_t sz = sizeof (vm_stats_vm_v_free_count);
    rc = sysctl (mib_vm_stats_vm_v_free_count, mib_sz, &vm_stats_vm_v_free_count, &sz, NULL, 0);
    if (rc != 0)
    {
        gu_error ("sysctl(vm.stats.vm.v_free_count) failed with code %d", rc);
        return 0;
    }
    return vm_stats_vm_v_free_count;
}

#endif /* __FreeBSD__ */

static gu_uint128_t _GU_FNV128_PRIME;
//GU_SET128(GU_FNV128_PRIME, 0x0000000001000000ULL, 0x000000000000013BULL);

static gu_uint128_t _GU_FNV128_SEED;
//GU_SET128(GU_FNV128_SEED,  0x6C62272E07BB0142ULL, 0x62B821756295C58DULL);

const gu_uint128_t *GET_GU_FNV128_PRIME()
{
        static init_done=0;
        if(!init_done)
        {
#if defined(GU_LITTLE_ENDIAN)
            _GU_FNV128_PRIME.u64[1]=0x0000000001000000ULL;
            _GU_FNV128_PRIME.u64[0]=0x000000000000013BULL;
#else
            _GU_FNV128_PRIME.u64[0]=0x0000000001000000ULL;
            _GU_FNV128_PRIME.u64[1]=0x000000000000013BULL;
#endif            
            init_done=1;
        }
        return &_GU_FNV128_PRIME;
}
const gu_uint128_t *GET_GU_FNV128_SEED()
{
    static init_done=0;
        if(!init_done)
        {
#if defined(GU_LITTLE_ENDIAN)
            _GU_FNV128_SEED.u64[1]=0x6C62272E07BB0142ULL;
            _GU_FNV128_SEED.u64[0]=0x62B821756295C58DULL;
#else
            _GU_FNV128_SEED.u64[0]=0x6C62272E07BB0142ULL;
            _GU_FNV128_SEED.u64[1]=0x62B821756295C58DULL;
#endif            
            init_done=1;
        }
        return &_GU_FNV128_SEED;
}
