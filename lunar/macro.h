#ifndef __LUNAR_MACRO_H__
#define __LUNAR_MACRO_H__

#include "log.h"
#include "util.h"
#include <assert.h>
#include <string.h>

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#define LUNAR_LIKELY(x) __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#define LUNAR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LUNAR_LIKELY(x) (x)
#define LUNAR_UNLIKELY(x) (x)
#endif

/// 断言宏封装
#define LUNAR_ASSERT(x)                                                        \
    if (LUNAR_UNLIKELY(!(x)))                                                  \
    {                                                                          \
        LUNAR_LOG_ERROR(LUNAR_LOG_ROOT())                                      \
            << "ASSERTION: " #x << "\nbacktrace:\n"                            \
            << lunar::BacktraceToString(100, 2, "    ");                       \
        assert(x);                                                             \
    }

/// 断言宏封装
#define LUNAR_ASSERT2(x, w)                                                    \
    if (LUNAR_UNLIKELY(!(x)))                                                  \
    {                                                                          \
        LUNAR_LOG_ERROR(LUNAR_LOG_ROOT())                                      \
            << "ASSERTION: " #x << "\n"                                        \
            << w << "\nbacktrace:\n"                                           \
            << lunar::BacktraceToString(100, 2, "    ");                       \
        assert(x);                                                             \
    }

#endif