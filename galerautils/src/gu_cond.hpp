/*
 * Copyright (C) 2009 Codership Oy <info@codership.com>
 */

#ifndef __GU_COND__
#define __GU_COND__

#include <pthread.h>
#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <cerrno>

#include "gu_macros.h"
#include "gu_exception.hpp"

// TODO: make exceptions more verbose

namespace gu
{
    class Cond
    {

        friend class Lock;
        // non-copyable
        Cond(const Cond&);
        void operator=(const Cond&);
    protected:

        pthread_cond_t mutable cond;
        long           mutable ref_count;

    public:

        Cond () : cond(), ref_count(0)
        {
            pthread_cond_init (&cond, NULL);
        }

        ~Cond ()
        {
            register int ret;
            while (EBUSY == (ret = pthread_cond_destroy(&cond)))
            {
#ifdef _MSC_VER
				usleep (1000); 
#else
				usleep (100); 
#endif
			}
            if (gu_unlikely(ret != 0))
                throw Exception("pthread_cond_destroy() failed", ret);
        }

        inline void signal () const
        {
            if (ref_count > 0) {
                register int ret = pthread_cond_signal (&cond);
                if (gu_unlikely(ret != 0))
                    throw Exception("pthread_cond_signal() failed", ret);
            }
        }

        inline void broadcast () const
        {
            if (ref_count > 0) {
                register int ret = pthread_cond_broadcast (&cond);
                if (gu_unlikely(ret != 0))
                    throw Exception("pthread_cond_broadcast() failed", ret);
            }
        }

    };
}

#endif // __GU_COND__
