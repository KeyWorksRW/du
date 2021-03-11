// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

#if defined(_WIN32)

// Reduce the number of Windows files that get read

    #define NOATOM
    #define NOCOMM
    #define NODRIVERS
    #define NOEXTDEVMODEPROPSHEET
    #define NOIME
    #define NOKANJI
    #define NOLOGERROR
    #define NOMCX
    #define NOPROFILER
    #define NOSCALABLEFONT
    #define NOSERVICE
    #define NOSOUND
    #define NOWINDOWSX
    #define NOENHMETAFILE

    #define OEMRESOURCE
    #define _CRT_SECURE_NO_WARNINGS

    #define STRICT
    #define WIN32_LEAN_AND_MEAN

    #define WINVER       0x0A00  // Windows 10
    #define _WIN32_WINNT 0x0A00

#endif  // _WIN32

#include <string>
#include <string_view>

#include <ttlibspace.h>  // Master header file for ttLib

#if defined(_WIN32)
    #include <ttdebug.h>  // ttAssertionMsg()
#endif

#include <ttcstr.h>     // Classes for handling zero-terminated char strings.
#include <ttstrings.h>  // Functions accessing translatable strings

inline constexpr const char* txtVersion = "du 1.2.0";
inline constexpr const char* txtCopyRight = "Copyright (c) 2018-2020";
inline constexpr const char* txtAppname = "du";

// Use THROW to throw this exception class
class CExcept
{
public:
    CExcept(const ttlib::cstr& what) : m_what(what) {}

    const ttlib::cstr& what() const { return m_what; }

private:
    ttlib::cstr m_what;
};

/////////////////////// macros ////////////////////////////////////////

#if defined(NDEBUG)

    #define ASSERT(cond)
    #define ASSERT_MSG(cond, msg)
    #define FAIL_MSG(msg)
    #define THROW(msg) throw CExcept(msg)

#else  // not defined(NDEBUG)

    #if defined(_WIN32) && !defined(NONWIN_TEST)
        #define ASSERT(cond)                                                            \
            {                                                                           \
                if (!(cond) && ttAssertionMsg(__FILE__, __func__, __LINE__, #cond, "")) \
                {                                                                       \
                    __debugbreak();                                                     \
                }                                                                       \
            }

        #define ASSERT_MSG(cond, msg)                                                  \
            if (!(cond) && ttAssertionMsg(__FILE__, __func__, __LINE__, #cond, (msg))) \
            {                                                                          \
                __debugbreak();                                                        \
            }

        #define FAIL_MSG(msg)                                               \
            if (ttAssertionMsg(__FILE__, __func__, __LINE__, nullptr, msg)) \
            {                                                               \
                __debugbreak();                                             \
            }

        // In _DEBUG builds this will display an assertion dialog first then it will throw
        // an excpetion. In Release builds, only the exception is thrown.
        #define THROW(msg)                                                      \
            {                                                                   \
                if (ttAssertionMsg(__FILE__, __func__, __LINE__, nullptr, msg)) \
                {                                                               \
                    __debugbreak();                                             \
                }                                                               \
                throw CExcept(msg);                                             \
            }

    #else  // not defined(_WIN32)
        #define ASSERT(cond)          wxASSERT(cond)
        #define ASSERT_MSG(cond, msg) wxASSERT_MSG(cond, msg)
        #define FAIL_MSG(msg)         wxFAIL_MSG(msg)

        // In _DEBUG builds this will display an assertion dialog first then it will throw
        // an excpetion. In Release builds, only the exception is thrown.
        #define THROW(msg)         \
            {                      \
                FAIL_MSG(msg);     \
                throw CExcept(msg) \
            }
    #endif  // _WIN32

#endif  // defined(NDEBUG)
