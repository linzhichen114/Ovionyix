/* ==> .../kernel/include/ovionyix.h
 *  -> The info of kernel.
 *
 * --------------------------------------------------
 *
 * By linzhichen114, write on 2025-6-25
 * Copyright © 2025 linzhichen114 and contributors, Based on GPL-3.0 open source agreement.
 */

#pragma once

#define BUILD_DATE __DATE__     // Compilation Date
#define BUILD_TIME __TIME__     // Compile time
#define KERNL_VERS "<dev>" // Version format: [major version].[patch version].[YY-MM-DD]

// Compiler judgment
#if defined(__clang__)
#    define COMPILER_NAME    "clang (clang)"
#    define STRINGIFY(x)     #x
#    define EXPAND(x)        STRINGIFY(x)
#    define COMPILER_VERSION EXPAND(__clang_major__.__clang_minor__.__clang_patchlevel__)
#elif defined(__GNUC__)
#    define COMPILER_NAME    "gcc (GCC)"
#    define STRINGIFY(x)     #x
#    define EXPAND(x)        STRINGIFY(x)
#    define COMPILER_VERSION EXPAND(__GNUC__.__GNUC_MINOR__.__GNUC_PATCHLEVEL__)
#else
#    define COMPILER_NAME    "unknown (Unknown)"
#    define STRINGIFY(x)     #x
#    define EXPAND(x)        STRINGIFY(x)
#    define COMPILER_VERSION "unknown"
#endif

#define Noreturn __attribute__((noreturn))
#define Deprecated __attribute__((__deprecated__))