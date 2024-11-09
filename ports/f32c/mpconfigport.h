/*
 * f32c port
 */

#include <stdint.h>

// Enable floating point by default.
#define MICROPY_FLOAT_IMPL		(MICROPY_FLOAT_IMPL_DOUBLE)

// Enable long-ints by default.
#define MICROPY_LONGINT_IMPL		(MICROPY_LONGINT_IMPL_LONGLONG)

// options to control how MicroPython is built
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES)

// Enable the unix-specific "time" module.
#define MICROPY_PY_TIME                (1)
#define MICROPY_PY_TIME_TIME_TIME_NS   (1)
//#define MICROPY_PY_TIME_CUSTOM_SLEEP   (1)
//#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (0)
#define MICROPY_PY_TIME_INCLUDEFILE    "ports/f32c/modtime.c"

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER		(1)

#define MICROPY_ENABLE_GC		(1)

// Enable the VFS, and enable the posix "filesystem".
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_VFS                 (1)
#define MICROPY_READER_VFS          (1)
#define MICROPY_HELPER_LEXER_UNIX   (1)
#define MICROPY_VFS_POSIX           (1)
#define MICROPY_READER_POSIX        (1)
#define MICROPY_PY_OS               (1)

#define MICROPY_HELPER_REPL		(1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT	(1)

#define MICROPY_ALLOC_PATH_MAX		(256)

// Use the minimum headroom in the chunk allocator for parse nodes.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT	(16)

// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <stdlib.h>

#define MICROPY_HEAP_SIZE		(25600) // heap size 25 kilobytes

#ifdef F32C
#define MICROPY_HW_BOARD_NAME "ulx3s"
#define MICROPY_HW_MCU_NAME "f32c"
#else
#define MICROPY_HW_BOARD_NAME "unknown"
#define MICROPY_HW_MCU_NAME "unknown"
#endif

#define MP_STATE_PORT MP_STATE_VM
