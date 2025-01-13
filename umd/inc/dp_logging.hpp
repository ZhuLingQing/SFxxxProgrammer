#ifndef _DP_LOGGING_HPP_
#define _DP_LOGGING_HPP_

#define PLOG_GLOBAL
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>
#include <csignal>

#ifdef NDEBUG
#define PLOG_DISABLE_LOGGING
#endif

#define DP_LOG_INST_ID (0xDEADBEE)

// Init
#define DP_LOG_INIT(sev, filename, filesize, maxfiles)                         \
    do                                                                         \
    {                                                                          \
        plog::init<DP_LOG_INST_ID>((sev), (filename), (filesize), (maxfiles)); \
    } while (0)

#define DP_LOG_INIT_WITH_CONSOLE(sev, filename, filesize, maxfiles)            \
    do                                                                         \
    {                                                                          \
        plog::init<DP_LOG_INST_ID>((sev), (filename), (filesize), (maxfiles)); \
        static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender; \
        plog::get<DP_LOG_INST_ID>()->addAppender(&consoleAppender);            \
    } while (0)

#define DP_LOG_INIT_CONSOLE_ONLY(sev)            \
    do                                                                         \
    {                                                                          \
        plog::init<DP_LOG_INST_ID>((sev)); \
        static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender; \
        plog::get<DP_LOG_INST_ID>()->addAppender(&consoleAppender);            \
    } while (0)

#define FATAL   (plog::fatal)
#define ERROR   (plog::error)
#define WARNING (plog::warning)
#define INFO    (plog::info)

#define DP_LOG(severity) PLOG_(DP_LOG_INST_ID, severity)
#ifndef NDEBUG
#define _DP_DLOG(severity) PLOG_(DP_LOG_INST_ID, severity)
#else
#define _DP_DLOG(severity) PLOG_(DP_LOG_INST_ID, severity)
#endif
#define DP_DLOG(severity) _DP_DLOG(severity)
// ---------------------------- CHECK macros ---------------------------------
// Check for a given boolean condition.
#define _DP_CHECK(condition)                    \
    if (!(condition)) signal(SIGABRT, SIG_DFL); \
    PLOG_IF_(DP_LOG_INST_ID, FATAL, !(condition)) << "Check failed: " #condition

#ifndef NDEBUG
// Debug only version of CHECK
#define _DP_DCHECK(condition) _DP_CHECK(condition)
#else
// Optimized version - generates no code.
#define _DP_DCHECK(condition) _DP_CHECK(condition)
#endif  // NDEBUG

#define DP_CHECK(condition)  _DP_CHECK(condition)
#define DP_DCHECK(condition) _DP_DCHECK(condition)
// ------------------------- CHECK_OP macros ---------------------------------
namespace op_detail
{
#define __DP_RELATIONAL_OP(name, op)                             \
    template <typename L, typename R>                            \
    bool __DP_RELATIONAL_OP_##name(const L& lval, const R& rval) \
    {                                                            \
        if (lval op rval)                                        \
        {                                                        \
            return lval op rval;                                 \
        }                                                        \
        else                                                     \
        {                                                        \
            abort();                                             \
        }                                                        \
    }  // namespace op_detail

__DP_RELATIONAL_OP(EQ, ==)
__DP_RELATIONAL_OP(NE, !=)
__DP_RELATIONAL_OP(LT, <)
__DP_RELATIONAL_OP(GT, >)
__DP_RELATIONAL_OP(LE, <=)
__DP_RELATIONAL_OP(GE, >=)
}  // namespace op_detail

// Check OP, print error message if check result is false.
#define _CHECK_OP(lval, rval, name, op)                                                  \
    PLOG_IF_(DP_LOG_INST_ID, FATAL, !(op_detail::__DP_RELATIONAL_OP_##name(lval, rval))) \
        << "Check failed: " #lval #op #rval

#ifndef NDEBUG
// Optimized version
#define _DCHECK_OP(lval, rval, name, op) _CHECK_OP(lval, rval, name, op)
#else
// Debug only version
#define _DCHECK_OP(lval, rval, name, op)
#endif

// Check_op macro definitions
#define DP_CHECK_EQ(lval, rval) _CHECK_OP(lval, rval, EQ, ==)
#define DP_CHECK_NE(lval, rval) _CHECK_OP(lval, rval, NE, !=)
#define DP_CHECK_LE(lval, rval) _CHECK_OP(lval, rval, LE, <=)
#define DP_CHECK_LT(lval, rval) _CHECK_OP(lval, rval, LT, <)
#define DP_CHECK_GE(lval, rval) _CHECK_OP(lval, rval, GE, >=)
#define DP_CHECK_GT(lval, rval) _CHECK_OP(lval, rval, GT, >)

// Debug only versions of CHECK_OP macros.
#define DP_DCHECK_EQ(lval, rval) _DCHECK_OP(lval, rval, EQ, ==)
#define DP_DCHECK_NE(lval, rval) _DCHECK_OP(lval, rval, NE, !=)
#define DP_DCHECK_LE(lval, rval) _DCHECK_OP(lval, rval, LT, <=)
#define DP_DCHECK_LT(lval, rval) _DCHECK_OP(lval, rval, LT, <)
#define DP_DCHECK_GE(lval, rval) _DCHECK_OP(lval, rval, GE, >=)
#define DP_DCHECK_GT(lval, rval) _DCHECK_OP(lval, rval, GT, >)

// ---------------------------CHECK_NOTNULL macros ---------------------------
// Helpers for CHECK_NOTNULL(). Two are necessary to support both raw pointers
// and smart pointers.
template <typename T>
T& __CheckNullPtrCommon(const char* file, int line, const char* msg, T& t)
{
    if (t == nullptr)
    {
        DP_LOG(FATAL) << msg << ", file:" << file << " line:" << line;
        DP_CHECK(false);
    }

    return t;
}

template <typename T>
T* __CheckNotNullPtr(const char* file, int line, const char* msg, T* t)
{
    return __CheckNullPtrCommon(file, line, msg, t);
}

template <typename T>
T& __CheckNotNullPtr(const char* file, int line, const char* msg, T& t)
{
    return __CheckNullPtrCommon(file, line, msg, t);
}

// Check that a pointer is not null.
#define _DP_CHECK_NOTNULL(ptr) __CheckNotNullPtr(__FILE__, __LINE__, "'" #ptr "' *Must be non nullptr*", (ptr))

#ifndef NDEBUG
// Debug only version of CHECK_NOTNULL
#define _DP_DCHECK_NOTNULL(ptr) _DP_CHECK_NOTNULL(prt)
#else
// Optimized version - generates no code.
#define _DP_DCHECK_NOTNULL(ptr)
#endif  // NDEBUG

#define DP_CHECK_NOTNULL(ptr)  _DP_CHECK_NOTNULL(ptr)
#define DP_DCHECK_NOTNULL(ptr) _DP_DCHECK_NOTNULL(ptr)

#endif
