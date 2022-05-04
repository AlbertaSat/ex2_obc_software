/*
 * ex2_new.cpp
 *
 *  Created on: Mar. 10, 2022
 *      Author: Robert Taylor
 */
#include <stdlib.h>
#include "new"
#include <cstdint>
#include "os_projdefs.h"
#include "os_portable.h"
#if !defined(__GLIBCXX__) &&                                                   \
    (!defined(_LIBCPP_ABI_MICROSOFT) || defined(_LIBCPP_NO_VCRUNTIME)) &&      \
    !defined(_LIBCPP_DISABLE_NEW_DELETE_DEFINITIONS)

void *
operator new(std::size_t size) _THROW_BAD_ALLOC
{
    if (size == 0)
        size = 1;
    void* p;
    while ((p = pvPortMalloc(size)) == 0)
    {
        // If malloc fails and there is a new_handler,
        // call it to try free up memory.
        std::new_handler nh = std::get_new_handler();
        if (nh)
            nh();
        else
#ifndef _LIBCPP_NO_EXCEPTIONS
            throw std::bad_alloc();
#else
            break;
#endif
    }
    return p;
}


void*
operator new(size_t size, const std::nothrow_t&) _NOEXCEPT
{
    void* p = 0;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        p = ::operator new(size);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
    return p;
}


void*
operator new[](size_t size) _THROW_BAD_ALLOC
{
    return ::operator new(size);
}


void*
operator new[](size_t size, const std::nothrow_t&) _NOEXCEPT
{
    void* p = 0;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        p = ::operator new[](size);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
    return p;
}


void
operator delete(void* ptr) _NOEXCEPT
{
    if (ptr)
        free(ptr);
}


void
operator delete(void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    ::operator delete(ptr);
}


void
operator delete(void* ptr, size_t) _NOEXCEPT
{
    ::operator delete(ptr);
}


void
operator delete[] (void* ptr) _NOEXCEPT
{
    ::operator delete(ptr);
}


void
operator delete[] (void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    ::operator delete[](ptr);
}


void
operator delete[] (void* ptr, size_t) _NOEXCEPT
{
    ::operator delete[](ptr);
}

#if !defined(_LIBCPP_HAS_NO_ALIGNED_ALLOCATION)


void *
operator new(std::size_t size, std::align_val_t alignment) _THROW_BAD_ALLOC
{
    if (size == 0)
        size = 1;
    if (static_cast<size_t>(alignment) < sizeof(void*))
      alignment = std::align_val_t(sizeof(void*));
    void* p;
#if defined(_LIBCPP_MSVCRT_LIKE)
    while ((p = _aligned_malloc(size, static_cast<size_t>(alignment))) == nullptr)
#else
    while (::posix_memalign(&p, static_cast<size_t>(alignment), size) != 0)
#endif
    {
        // If posix_memalign fails and there is a new_handler,
        // call it to try free up memory.
        std::new_handler nh = std::get_new_handler();
        if (nh)
            nh();
        else {
#ifndef _LIBCPP_NO_EXCEPTIONS
            throw std::bad_alloc();
#else
            p = nullptr; // posix_memalign doesn't initialize 'p' on failure
            break;
#endif
        }
    }
    return p;
}


void*
operator new(size_t size, std::align_val_t alignment, const std::nothrow_t&) _NOEXCEPT
{
    void* p = 0;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        p = ::operator new(size, alignment);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
    return p;
}


void*
operator new[](size_t size, std::align_val_t alignment) _THROW_BAD_ALLOC
{
    return ::operator new(size, alignment);
}


void*
operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) _NOEXCEPT
{
    void* p = 0;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        p = ::operator new[](size, alignment);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
    return p;
}


void
operator delete(void* ptr, std::align_val_t) _NOEXCEPT
{
    if (ptr)
#if defined(_LIBCPP_MSVCRT_LIKE)
        ::_aligned_free(ptr);
#else
        ::vPortFree(ptr);
#endif
}


void
operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) _NOEXCEPT
{
    ::operator delete(ptr, alignment);
}


void
operator delete(void* ptr, size_t, std::align_val_t alignment) _NOEXCEPT
{
    ::operator delete(ptr, alignment);
}


void
operator delete[] (void* ptr, std::align_val_t alignment) _NOEXCEPT
{
    ::operator delete(ptr, alignment);
}


void
operator delete[] (void* ptr, std::align_val_t alignment, const std::nothrow_t&) _NOEXCEPT
{
    ::operator delete[](ptr, alignment);
}


void
operator delete[] (void* ptr, size_t, std::align_val_t alignment) _NOEXCEPT
{
    ::operator delete[](ptr, alignment);
}

#endif // !_LIBCPP_HAS_NO_ALIGNED_ALLOCATION
#endif // !__GLIBCXX__ && (!_LIBCPP_ABI_MICROSOFT || _LIBCPP_NO_VCRUNTIME) && !_LIBCPP_DISABLE_NEW_DELETE_DEFINITIONS
