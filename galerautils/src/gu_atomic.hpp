//
// Copyright (C) 2010 Codership Oy <info@codership.com>
//

//
// @todo Check that the at least the following gcc versions are supported
// gcc version 4.1.2 20080704 (Red Hat 4.1.2-48)
//

#ifndef GU_ATOMIC_HPP
#define GU_ATOMIC_HPP
#ifdef _MSC_VER
#ifdef __cplusplus
/*    Copyright 2012 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 * Implementation of the AtomicIntrinsics<T>::* operations for Windows systems.
 */

#pragma once

#include <boost/utility.hpp>

//#include "mongo/platform/windows_basic.h"
#include <windows.h>
#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange64)
//#pragma intrinsic(_InterlockedAdd)
//#pragma intrinsic(_InterlockedAdd64)
namespace mongo {

    /**
     * Default instantiation of AtomicIntrinsics<>, for unsupported types.
     */
    template <typename T, typename _IsTSupported=void>
    class AtomicIntrinsics {
    private:
        AtomicIntrinsics();
        ~AtomicIntrinsics();
    };

    /**
     * Instantiation of AtomicIntrinsics<> for 32-bit word sizes (i.e., unsigned).
     */
    template <typename T>
    class AtomicIntrinsics<T, typename boost::enable_if_c<sizeof(T) == sizeof(LONG)>::type> {
    public:

        static T compareAndSwap(volatile T* dest, T expected, T newValue) {
            return InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(dest),
                                              LONG(newValue),
                                              LONG(expected));
        }

        static T swap(volatile T* dest, T newValue) {
            return InterlockedExchange(reinterpret_cast<volatile LONG*>(dest), LONG(newValue));
        }

        static T load(volatile const T* value) {
            MemoryBarrier();
            T result = *value;
            MemoryBarrier();
            return result;
        }

        static T loadRelaxed(volatile const T* value) {
            return *value;
        }

        static void store(volatile T* dest, T newValue) {
            MemoryBarrier();
            *dest = newValue;
            MemoryBarrier();
        }

        static T fetchAndAdd(volatile T* dest, T increment) {
            return InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(dest), LONG(increment));
        }

    private:
        AtomicIntrinsics();
        ~AtomicIntrinsics();
    };

    /**
     * Instantiation of AtomicIntrinsics<> for 64-bit word sizes.
     */
    template <typename T>
    class AtomicIntrinsics<T, typename boost::enable_if_c<sizeof(T) == sizeof(LONGLONG)>::type> {
    public:

#if defined(NTDDI_VERSION) && defined(NTDDI_WS03SP2) && (NTDDI_VERSION >= NTDDI_WS03SP2)
        static const bool kHaveInterlocked64 = true;
#else
        static const bool kHaveInterlocked64 = false;
#endif

        static T compareAndSwap(volatile T* dest, T expected, T newValue) {
            return InterlockedImpl<kHaveInterlocked64>::compareAndSwap(dest, expected, newValue);
        }

        static T swap(volatile T* dest, T newValue) {
            return InterlockedImpl<kHaveInterlocked64>::swap(dest, newValue);
        }

        static T load(volatile const T* value) {
            return LoadStoreImpl<T>::load(value);
        }

        static void store(volatile T* dest, T newValue) {
            LoadStoreImpl<T>::store(dest, newValue);
        }

        static T fetchAndAdd(volatile T* dest, T increment) {
            return InterlockedImpl<kHaveInterlocked64>::fetchAndAdd(dest, increment);
        }

    private:
        AtomicIntrinsics();
        ~AtomicIntrinsics();

        template <bool>
        struct InterlockedImpl;

        // Implementation of 64-bit Interlocked operations via Windows API calls.
        template<>
        struct InterlockedImpl<true> {
            static T compareAndSwap(volatile T* dest, T expected, T newValue) {
                return InterlockedCompareExchange64(
                    reinterpret_cast<volatile LONGLONG*>(dest),
                    LONGLONG(newValue),
                    LONGLONG(expected));
            }

            static T swap(volatile T* dest, T newValue) {
                return InterlockedExchange64(
                    reinterpret_cast<volatile LONGLONG*>(dest),
                    LONGLONG(newValue));
            }

            static T fetchAndAdd(volatile T* dest, T increment) {
                return InterlockedExchangeAdd64(
                    reinterpret_cast<volatile LONGLONG*>(dest),
                    LONGLONG(increment));
            }
        };

        // Implementation of 64-bit Interlocked operations for systems where the API does not
        // yet provide the Interlocked...64 operations.
        template<>
        struct InterlockedImpl<false> {
            static T compareAndSwap(volatile T* dest, T expected, T newValue) {
                // NOTE: We must use the compiler intrinsic here: WinXP does not offer
                // InterlockedCompareExchange64 as an API call.
                return _InterlockedCompareExchange64(
                    reinterpret_cast<volatile LONGLONG*>(dest),
                    LONGLONG(newValue),
                    LONGLONG(expected));
            }

            static T swap(volatile T* dest, T newValue) {
                // NOTE: You may be tempted to replace this with
                // 'InterlockedExchange64'. Resist! It will compile just fine despite not being
                // listed in the docs as available on XP, but the compiler may replace it with
                // calls to the non-intrinsic 'InterlockedCompareExchange64', which does not
                // exist on XP. We work around this by rolling our own synthetic in terms of
                // compareAndSwap which we have explicitly formulated in terms of the compiler
                // provided _InterlockedCompareExchange64 intrinsic.
                T currentValue = *dest;
                while (true) {
                    const T result = compareAndSwap(dest, currentValue, newValue);
                    if (result == currentValue)
                        return result;
                    currentValue = result;
                }
            }

            static T fetchAndAdd(volatile T* dest, T increment) {
                // NOTE: See note for 'swap' on why we roll this ourselves.
                T currentValue = *dest;
                while (true) {
                    const T incremented = currentValue + increment;
                    const T result = compareAndSwap(dest, currentValue, incremented);
                    if (result == currentValue)
                        return result;
                    currentValue = result;
                }
            }
        };

        // On 32-bit IA-32 systems, 64-bit load and store must be implemented in terms of
        // Interlocked operations, but on 64-bit systems they support a simpler, native
        // implementation.  The LoadStoreImpl type represents the abstract implementation of
        // loading and storing 64-bit values.
        template <typename U, typename _IsTTooBig=void>
        class LoadStoreImpl{};

        // Implementation on 64-bit systems.
        template <typename U>
        class LoadStoreImpl<U, typename boost::enable_if_c<sizeof(U) <= sizeof(void*)>::type> {
        public:
            static U load(volatile const U* value) {
                MemoryBarrier();
                U result = *value;
                MemoryBarrier();
                return result;
            }


            static void store(volatile U* dest, U newValue) {
                MemoryBarrier();
                *dest = newValue;
                MemoryBarrier();
            }
        };

        // Implementation on 32-bit systems.
        template <typename U>
        class LoadStoreImpl<U, typename boost::disable_if_c<sizeof(U) <= sizeof(void*)>::type> {
        public:
            static U load(volatile const U* value) {
                return AtomicIntrinsics<U>::compareAndSwap(const_cast<volatile U*>(value),
                                                           U(0),
                                                           U(0));
            }

            static void store(volatile U* dest, U newValue) {
                AtomicIntrinsics<U>::swap(dest, newValue);
            }
        };
    };

}  // namespace mongo
namespace Ha {

    /**
     * Default instantiation of AtomicIntrinsics<>, for unsupported types.
     */
    template <typename T, typename _IsTSupported=void>
    class AtomicIntrinsics {
    private:
        AtomicIntrinsics();
        ~AtomicIntrinsics();
    };
template <typename T>
    class AtomicIntrinsics<T, typename boost::enable_if_c<sizeof(T) == sizeof(LONG)>::type> {
    public:




        static T compareAndSwap(volatile T* dest, T expected, T newValue) {
            // NOTE: We must use the compiler intrinsic here: WinXP does not offer
            // InterlockedCompareExchange64 as an API call.
            return _InterlockedCompareExchange(
                reinterpret_cast<volatile LONG*>(dest),
                LONG(newValue),
                LONG(expected));
        }


        static T addAndFetch(volatile T* dest, T increment) {
            // NOTE: See note for 'swap' on why we roll this ourselves.
            T currentValue = *dest;
            while (true) {
                const T incremented = currentValue + increment;
                const T result = compareAndSwap(dest, currentValue, incremented);
                if (result == currentValue)
                    return incremented;
                currentValue = result;
            }
		}

    private:
        AtomicIntrinsics();
        ~AtomicIntrinsics();
    };

    /**
     * Instantiation of AtomicIntrinsics<> for 64-bit word sizes.
     */
    template <typename T>
    class AtomicIntrinsics<T, typename boost::enable_if_c<sizeof(T) == sizeof(LONGLONG)>::type> {
    public:

#if defined(NTDDI_VERSION) && defined(NTDDI_WS03SP2) && (NTDDI_VERSION >= NTDDI_WS03SP2) && 1==0
        static const bool kHaveInterlocked64 = true;
#else
        static const bool kHaveInterlocked64 = false;
#endif

        static T addAndFetch(volatile T* dest, T increment)
        {
            return InterlockedImpl<kHaveInterlocked64>::addAndFetch(dest, increment);
        }


    private:
        AtomicIntrinsics();
        ~AtomicIntrinsics();

        template <bool>
        struct InterlockedImpl;

        // Implementation of 64-bit Interlocked operations via Windows API calls.
        template<>
        struct InterlockedImpl<true> {
            static T addAndFetch(volatile T* dest, T increment) {
                return _InterlockedAdd64(
                    reinterpret_cast<volatile LONGLONG*>(dest),
                    LONGLONG(increment));
            }
        };

        // Implementation of 64-bit Interlocked operations for systems where the API does not
        // yet provide the Interlocked...64 operations.
        template<>
        struct InterlockedImpl<false> {
            static T compareAndSwap(volatile T* dest, T expected, T newValue) {
                // NOTE: We must use the compiler intrinsic here: WinXP does not offer
                // InterlockedCompareExchange64 as an API call.
                return _InterlockedCompareExchange64(
                    reinterpret_cast<volatile LONGLONG*>(dest),
                    LONGLONG(newValue),
                    LONGLONG(expected));
            }


            static T addAndFetch(volatile T* dest, T increment) {
                // NOTE: See note for 'swap' on why we roll this ourselves.
                T currentValue = *dest;
                while (true) {
                    const T incremented = currentValue + increment;
                    const T result = compareAndSwap(dest, currentValue, incremented);
                    if (result == currentValue)
                        return incremented;
                    currentValue = result;
                }
            }	
        };
    };

}
#endif // __cplusplus

namespace gu
{
    template <typename I>
    class Atomic
    {
    public:
        Atomic<I>(const I i = 0) : i_(i) { }

        I operator()() const
        {
            //__sync_synchronize();
            return mongo::AtomicIntrinsics<I>::load(&i_);
        }

        Atomic<I>& operator=(const I i)
        {
            mongo::AtomicIntrinsics<I>::swap(&i_,i);
            return *this;
        }

        I fetch_and_zero()
        {
            return mongo::AtomicIntrinsics<I>::swap(&i_,0);
            //return __sync_fetch_and_and(&i_, 0);
        }

        I fetch_and_add(const I i)
        {
            return mongo::AtomicIntrinsics<I>::fetchAndAdd(&i_,i);
            //return __sync_fetch_and_add(&i_, i);
        }

        I add_and_fetch(const I i)
        {
			return Ha::AtomicIntrinsics<I>::addAndFetch(&i_,i);
			/*
            I ret = mongo::AtomicIntrinsics<I>::load(&i_) + i;
            mongo::AtomicIntrinsics<I>::swap(&i_,ret);
            return ret;
			*/
            //return __sync_add_and_fetch(&i_, i);
        }

        I sub_and_fetch(const I i)
        {
			return Ha::AtomicIntrinsics<I>::addAndFetch(&i_,-i);
			/*
            I ret = mongo::AtomicIntrinsics<I>::load(&i_) - i;
            mongo::AtomicIntrinsics<I>::swap(&i_,ret);
            return ret;
			*/
            //return __sync_sub_and_fetch(&i_, i);
        }

        Atomic<I>& operator++()
        {
            mongo::AtomicIntrinsics<I>::fetchAndAdd(&i_,1);
            return *this;
        }
        Atomic<I>& operator--()
        {
            mongo::AtomicIntrinsics<I>::fetchAndAdd(&i_,-1);
            return *this;
        }

        Atomic<I>& operator+=(const I i)
        {
            mongo::AtomicIntrinsics<I>::fetchAndAdd(&i_,i);
            return *this;
        }
    private:
        I i_;
    };
}

#else // _MSC_VER


#include <memory>

namespace gu
{
    template <typename I>
    class Atomic
    {
    public:
        Atomic<I>(const I i = 0) : i_(i) { }

        I operator()() const
        {
            __sync_synchronize();
            return i_;
        }

        Atomic<I>& operator=(const I i)
        {
            i_ = i;
            __sync_synchronize();
            return *this;
        }

        I fetch_and_zero()
        {
            return __sync_fetch_and_and(&i_, 0);
        }

        I fetch_and_add(const I i)
        {
            return __sync_fetch_and_add(&i_, i);
        }

        I add_and_fetch(const I i)
        {
            return __sync_add_and_fetch(&i_, i);
        }

        I sub_and_fetch(const I i)
        {
            return __sync_sub_and_fetch(&i_, i);
        }

        Atomic<I>& operator++()
        {
            __sync_fetch_and_add(&i_, 1);
            return *this;
        }
        Atomic<I>& operator--()
        {
            __sync_fetch_and_sub(&i_, 1);
            return *this;
        }

        Atomic<I>& operator+=(const I i)
        {
            __sync_fetch_and_add(&i_, i);
            return *this;
        }
    private:
        I i_;
    };
}
#endif // _MSC_VER

#endif // GU_ATOMIC_HPP
