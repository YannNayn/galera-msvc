// Copyright (C) 2012 Codership Oy <info@codership.com>

/**
 * @file CPU architecture related functions/macros
 *
 * $Id: gu_arch.h 3196 2013-08-09 07:26:28Z vlad $
 */

#ifndef _gu_arch_h_
#define _gu_arch_h_

#if defined(HAVE_ENDIAN_H)
# include <endian.h>
#elif defined(HAVE_SYS_ENDIAN_H) /* FreeBSD */
# include <sys/endian.h>
#elif defined(HAVE_SYS_BYTEORDER_H)
# include <sys/byteorder.h>
#elif defined(__APPLE__)
# include <machine/endian.h>
#elif defined(WIN32) || defined(_MSC_VER)
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
# error "No byte order header file detected"
#endif

#if defined(__BYTE_ORDER)
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define GU_LITTLE_ENDIAN
# endif
#elif defined(_BYTE_ORDER) /* FreeBSD */
# if _BYTE_ORDER == _LITTLE_ENDIAN
#  define GU_LITTLE_ENDIAN
# endif
#elif defined(__APPLE__) && defined(__DARWIN_BYTE_ORDER)
# if __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
#  define GU_LITTLE_ENDIAN
# endif
#elif defined(__sun__)
# if !defined(_BIG_ENDIAN)
#  define GU_LITTLE_ENDIAN
# endif
#else
# error "Byte order not defined"
#endif

#if defined(__sun__)
# define GU_WORDSIZE 64 /* Solaris 11 is only 64-bit ATM */
#elif defined(__APPLE__) || defined(__FreeBSD__)
# include <stdint.h>
# define GU_WORDSIZE __WORDSIZE
#elif defined(WIN32)|| defined(_MSC_VER)
#if defined(_M_X64)
#define GU_WORDSIZE 64
#else
#define GU_WORDSIZE 32
#endif
#else
# include <bits/wordsize.h>
# define GU_WORDSIZE __WORDSIZE
#endif

#if (GU_WORDSIZE != 32) && (GU_WORDSIZE != 64)
# error "Unsupported wordsize"
#endif

/* I'm not aware of the platforms that don't, but still */
#define GU_ALLOW_UNALIGNED_READS 1

#endif /* _gu_arch_h_ */
