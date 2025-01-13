#ifndef __DP_PROFILER_H__
#define __DP_PROFILER_H__

#ifdef ENABLE_PROFILER
#include <iostream>
#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER 1
#endif
#include "easy/arbitrary_value.h"
#include "easy/profiler.h"

#define DP_PROF_BLOCK(name)                                   EASY_BLOCK(name, profiler::colors::RichGreen)
#define DP_PROF_END_BLOCK()                                   EASY_END_BLOCK
#define DP_PROF_NONSCOPED_BLOCK(name)                         EASY_NONSCOPED_BLOCK(name, profiler::colors::DarkGreen)
#define DP_PROF_FUNCTION()                                    EASY_FUNCTION(profiler::colors::Blue)
#define DP_PROF_EVENT(name)                                   EASY_EVENT(name, profiler::colors::Amber)
#define DP_PROF_THREAD(name)                                  EASY_THREAD(name)
#define DP_PROF_THREAD_SCOPE(name)                            EASY_THREAD_SCOPE(name)
#define DP_PROF_MAIN_THREAD()                                 EASY_MAIN_THREAD
#define DP_PROF_EVENT_TRACING_ENABLED(isEnabled)              EASY_SET_EVENT_TRACING_ENABLED(isEnabled)
#define DP_PROF_SET_LOW_PRIORITY_EVENT_TRACING(isLowPriority) EASY_SET_LOW_PRIORITY_EVENT_TRACING(isLowPriority)
#define DP_PROF_EVENT_TRACING_SET_LOG(filename)               EASY_EVENT_TRACING_SET_LOG(filename)
#define DP_PROF_EVENT_TRACING_LOG()                           EASY_EVENT_TRACING_LOG
#define DP_PROF_ENABLE()                                 \
    do                                                   \
    {                                                    \
        EASY_PROFILER_ENABLE;                            \
        std::cout << "Dp profiler enabled" << std::endl; \
    } while (0)

#define DP_PROF_DISABLE()                                 \
    do                                                    \
    {                                                     \
        EASY_PROFILER_DISABLE;                            \
        std::cout << "Dp profiler disabled" << std::endl; \
    } while (0)

#define DP_PROF_DUMP(filename)                                               \
    do                                                                       \
    {                                                                        \
        profiler::dumpBlocksToFile(filename);                                \
        std::cout << "Dp profiler data dumped to " << filename << std::endl; \
    } while (0)

#define DP_PROF_VALUE(name, value)       EASY_VALUE(name, value, profiler::colors::DarkBrown)
#define DP_PROF_ARRAY(name, value, size) EASY_ARRAY(name, value, size, profiler::colors::DarkBrown)
#define DP_PROF_TEXT(name, text)         EASY_TEXT(name, text, profiler::colors::RichYellow)
#define DP_PROF_STRING(name, text, size) EASY_STRING(name, text, size, profiler::colors::RichYellow)
#else
#define DP_PROF_BLOCK(name)
#define DP_PROF_END_BLOCK()
#define DP_PROF_NONSCOPED_BLOCK(name)
#define DP_PROF_FUNCTION()
#define DP_PROF_ENABLE()
#define DP_PROF_DISABLE()
#define DP_PROF_EVENT(name)
#define DP_PROF_THREAD(name)
#define DP_PROF_THREAD_SCOPE(name)
#define DP_PROF_MAIN_THREAD()
#define DP_PROF_EVENT_TRACING_ENABLED(isEnabled)
#define DP_PROF_SET_LOW_PRIORITY_EVENT_TRACING(isLowPriority)
#define DP_PROF_EVENT_TRACING_SET_LOG(filename)
#define DP_PROF_EVENT_TRACING_LOG()
#define DP_PROF_DUMP(filename)
#define DP_PROF_VALUE(name, value)
#define DP_PROF_ARRAY(name, value, size)
#define DP_PROF_TEXT(name, text)
#define DP_PROF_STRING(name, text, size)
#endif

#endif  // __DP_PROFILER_H__