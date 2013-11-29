// Copyright (C) 2012 Codership Oy <info@codership.com>

/**
 * @file Location of some "standard" types definitions
 *
 * $Id: gu_types.h 3196 2013-08-09 07:26:28Z vlad $
 */

#ifndef _gu_types_h_
#define _gu_types_h_

#include <stdint.h>  /* intXX_t and friends */
#include <stdbool.h> /* bool */
#ifndef _MSC_VER
#include <unistd.h>  /* ssize_t */
#else
#include <msvc_sup.h>  /* ssize_t */
#define __attribute__(x)
#define GU_NORETURN
#define inline __inline
#endif
#endif /* _gu_types_h_ */
