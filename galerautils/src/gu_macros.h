// Copyright (C) 2007-2013 Codership Oy <info@codership.com>

/**
 * @file Miscellaneous macros
 *
 * $Id: gu_macros.h 3196 2013-08-09 07:26:28Z vlad $
 */

#ifndef _gu_macros_h_
#define _gu_macros_h_

/*
 * Platform-dependent macros
 */

#if defined(_MSC_VER)
#define ELAST 1076
#define EBADFD WSAENOTSOCK
# define EREMCHG          (ELAST+2)
# define ENOTUNIQ         (ELAST+3)
# define ERESTART         (ELAST+4)

#define inline __inline
#define snprintf _snprintf
#ifndef GU_NORETURN
#  define GU_NORETURN      
#endif
#  define GU_INLINE        __inline
#  define GU_FORCE_INLINE  __inline
#  define GU_UNUSED
#  define GU_LONG(x)       (x)
#  define GU_ULONG(x)      (x)
#  define GU_LONG_LONG(x)  (x)
#  define GU_ULONG_LONG(x) (x)
#define usleep(ms) Sleep(ms/1000)
#else   /* !defined(_MSC_VER) */
#  define GU_NORETURN      __attribute__((noreturn))
#  define GU_INLINE        inline
#  define GU_FORCE_INLINE  inline __attribute__((always_inline))
#  define GU_UNUSED        __attribute__((unused))
#  define GU_LONG(x)       (x##L)
#  define GU_ULONG(x)      (x##LU)
#  define GU_LONG_LONG(x)  (x##LL)
#  define GU_ULONG_LONG(x) (x##LLU)
#endif /* !defined(_MSC_VER) */

/*
 * End of paltform-dependent macros
 */

/* "Shamelessly stolen" (tm) goods from Linux kernel */
/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#if 0 // typeof() is not in C99
#define GU_MAX(x,y) ({       \
        typeof(x) _x = (x);  \
        typeof(y) _y = (y);  \
        (void) (&_x == &_y); \
        _x > _y ? _x : _y; })

#define GU_MIN(x,y) ({       \
        typeof(x) _x = (x);  \
        typeof(y) _y = (y);  \
        (void) (&_x == &_y); \
        _x < _y ? _x : _y; })
#endif

#define gu_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#if __GNUC__ >= 3
#  define gu_likely(x)   __builtin_expect((x), 1)
#  define gu_unlikely(x) __builtin_expect((x), 0)
#else
#  define gu_likely(x)   (x)
#  define gu_unlikely(x) (x)
#endif

#endif /* _gu_macros_h_ */
