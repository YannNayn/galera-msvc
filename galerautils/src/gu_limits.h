// Copyright (C) 2008 Codership Oy <info@codership.com>

/**
 * @file system limit macros
 *
 * $Id: gu_limits.h 3217 2013-08-23 12:09:40Z alex $
 */

#ifndef _gu_limits_h_
#define _gu_limits_h_
#ifdef _MSC_VER
#include <windows.h>
#define inline __inline
#else
#include <unistd.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
# if defined(__APPLE__)
long gu_darwin_phys_pages (void);
long gu_darwin_avphys_pages (void);
# elif defined(__FreeBSD__)
long gu_freebsd_avphys_pages (void);
# endif
#ifdef __cplusplus
}
#endif

#if defined(__APPLE__)
static inline size_t gu_page_size()    { return getpagesize();             }
static inline size_t gu_phys_pages()   { return gu_darwin_phys_pages();    }
static inline size_t gu_avphys_pages() { return gu_darwin_avphys_pages();  }
#elif defined(__FreeBSD__)
static inline size_t gu_page_size()    { return sysconf(_SC_PAGESIZE);     }
static inline size_t gu_phys_pages()   { return sysconf(_SC_PHYS_PAGES);   }
static inline size_t gu_avphys_pages() { return gu_freebsd_avphys_pages(); }
#elif defined(_MSC_VER)
#define _SC_PAGESIZE 1
#define _SC_PHYS_PAGES 2
#define _SC_OPEN_MAX 3
#define _SC_AVPHYS_PAGES 4
#define _SC_PAGE_SIZE _SC_PAGESIZE
extern size_t sysconf(int);
static inline size_t gu_page_size()    { return sysconf(_SC_PAGESIZE);     }
static inline size_t gu_phys_pages()   { return sysconf(_SC_PHYS_PAGES);   }
static inline size_t gu_avphys_pages() { return sysconf(_SC_AVPHYS_PAGES); }
#else /* !__APPLE__ && !__FreeBSD__ */
static inline size_t gu_page_size()    { return sysconf(_SC_PAGESIZE);     }
static inline size_t gu_phys_pages()   { return sysconf(_SC_PHYS_PAGES);   }
static inline size_t gu_avphys_pages() { return sysconf(_SC_AVPHYS_PAGES); }
#endif /* !__APPLE__ && !__FreeBSD__ */

static inline size_t gu_avphys_bytes()
{
    // to detect overflow on systems with >4G of RAM, see #776
    size_t _max = -1;
    unsigned long long avphys = gu_avphys_pages(); avphys *= gu_page_size();
    
    return (avphys < _max ? avphys : _max);
}

#include <limits.h>

#define GU_ULONG_MAX      ULONG_MAX
#define GU_LONG_MAX       LONG_MAX
#define GU_LONG_MIN       LONG_MIN
#define GU_ULONG_LONG_MAX ULLONG_MAX
#define GU_LONG_LONG_MAX  LLONG_MAX
#define GU_LONG_LONG_MIN  LLONG_MIN

#endif /* _gu_limits_h_ */
