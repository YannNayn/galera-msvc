// Copyright (C) 2009 Codership Oy <info@codership.com>

/**
 * @file Miscellaneous C++-related macros
 *
 * $Id: gu_macros.hpp 3196 2013-08-09 07:26:28Z vlad $
 */

#ifndef _gu_macros_hpp_
#define _gu_macros_hpp_

/* To protect against "old-style" casts in libc macros
 * must be included after respective libc headers */
#if defined(SIG_IGN)
extern "C" { static void (* const GU_SIG_IGN)(int) = SIG_IGN; }
#endif

#if defined(MAP_FAILED)
extern "C" { static const void* const GU_MAP_FAILED = MAP_FAILED; }
#endif

#endif /* _gu_macros_hpp_ */
