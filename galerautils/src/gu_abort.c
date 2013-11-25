// Copyright (C) 2011-2013 Codership Oy <info@codership.com>

/**
 * @file Clean abort function
 *
 * $Id: gu_abort.c 3241 2013-08-28 10:22:21Z alex $
 */


#include "gu_abort.h"

#include "gu_system.h"
#include "gu_log.h"
#ifndef _MSC_VER
#include <sys/resource.h> /* for setrlimit() */
#endif
#include <signal.h>       /* for signal()    */
#include <stdlib.h>       /* for abort()     */

void
gu_abort (void)
{
    /* avoid coredump */
#ifndef _MSC_VER    
    struct rlimit core_limits = { 0, 0 };
    setrlimit (RLIMIT_CORE, &core_limits);
#endif
    /* restore default SIGABRT handler */
    signal (SIGABRT, SIG_DFL);

#if defined(GU_SYS_PROGRAM_NAME)
    gu_info ("%s: Terminated.", GU_SYS_PROGRAM_NAME);
#else
    gu_info ("Program terminated.");
#endif

    abort();
}

