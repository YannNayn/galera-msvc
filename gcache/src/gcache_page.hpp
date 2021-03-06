/*
 * Copyright (C) 2010 Codership Oy <info@codership.com>
 */

/*! @file page file class */

#ifndef _gcache_page_hpp_
#define _gcache_page_hpp_

#include "gcache_memops.hpp"
#include "gcache_fd.hpp"
#include "gcache_mmap.hpp"
#include "gcache_bh.hpp"

#include <string>

namespace gcache
{
    class Page : public MemOps
    {
    public:

        Page (const std::string& name, ssize_t size);
        ~Page () {}

        void* malloc  (ssize_t size);

        void  free    (const void* ptr)
        {
            assert (ptr > mmap_.ptr);
            assert (ptr <= (static_cast<uint8_t*>(mmap_.ptr) + mmap_.size));
            assert (used_ > 0);
            used_--;
            BH_release (ptr2BH(ptr));
        }

        void* realloc (void* ptr, ssize_t size);

        void discard (BufferHeader* ptr) {}

        ssize_t used () const { return used_; }

        ssize_t size () const /* total page size */
        { return mmap_.size - sizeof(BufferHeader); }

        const std::string& name() const { return fd_.get_name(); }

        void reset ();

        /* Drop filesystem cache on the file */
        void drop_fs_cache() const;

    private:

        FileDescriptor fd_;
        MMap           mmap_;
        uint8_t*       next_;
        ssize_t        space_;
        ssize_t        used_;

        Page(const gcache::Page&);
        Page& operator=(const gcache::Page&);
    };
}

#endif /* _gcache_page_hpp_ */
